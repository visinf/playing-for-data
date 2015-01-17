/******************************************************************************
 * The MIT License (MIT)
 * 
 * Copyright (c) 2014 Crytek
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 ******************************************************************************/


#include "gl_replay.h"
#include "gl_driver.h"
#include "gl_resources.h"

#include "data/glsl/debuguniforms.h"

#include "serialise/string_utils.h"

GLReplay::GLReplay()
{
	m_pDriver = NULL;
	m_Proxy = false;

	RDCEraseEl(m_ReplayCtx);
	m_DebugCtx = NULL;

	m_OutputWindowID = 1;
}

void GLReplay::Shutdown()
{
	DeleteDebugData();

	DestroyOutputWindow(m_DebugID);

	CloseReplayContext();

	delete m_pDriver;
}

#pragma region Implemented

void GLReplay::ReadLogInitialisation()
{
	MakeCurrentReplayContext(&m_ReplayCtx);
	m_pDriver->ReadLogInitialisation();
}

void GLReplay::ReplayLog(uint32_t frameID, uint32_t startEventID, uint32_t endEventID, ReplayLogType replayType)
{
	MakeCurrentReplayContext(&m_ReplayCtx);
	m_pDriver->ReplayLog(frameID, startEventID, endEventID, replayType);
}

vector<FetchFrameRecord> GLReplay::GetFrameRecord()
{
	return m_pDriver->GetFrameRecord();
}

ResourceId GLReplay::GetLiveID(ResourceId id)
{
	return m_pDriver->GetResourceManager()->GetLiveID(id);
}

APIProperties GLReplay::GetAPIProperties()
{
	APIProperties ret;

	ret.pipelineType = ePipelineState_OpenGL;

	return ret;
}

vector<ResourceId> GLReplay::GetBuffers()
{
	vector<ResourceId> ret;
	
	for(auto it=m_pDriver->m_Buffers.begin(); it != m_pDriver->m_Buffers.end(); ++it)
		ret.push_back(it->first);

	return ret;
}

vector<ResourceId> GLReplay::GetTextures()
{
	vector<ResourceId> ret;
	ret.reserve(m_pDriver->m_Textures.size());
	
	for(auto it=m_pDriver->m_Textures.begin(); it != m_pDriver->m_Textures.end(); ++it)
	{
		auto &res = m_pDriver->m_Textures[it->first];

		// skip textures that aren't from the log (except the 'default backbuffer' textures)
		if(res.resource.name != m_pDriver->m_FakeBB_Color &&
		   res.resource.name != m_pDriver->m_FakeBB_DepthStencil &&
		   m_pDriver->GetResourceManager()->GetOriginalID(it->first) == it->first) continue;

		ret.push_back(it->first);
		CacheTexture(it->first);
	}

	return ret;
}

void GLReplay::SetReplayData(GLWindowingData data)
{
	m_ReplayCtx = data;
	
	InitDebugData();
}

void GLReplay::InitCallstackResolver()
{
	m_pDriver->GetSerialiser()->InitCallstackResolver();
}

bool GLReplay::HasCallstacks()
{
	return m_pDriver->GetSerialiser()->HasCallstacks();
}

Callstack::StackResolver *GLReplay::GetCallstackResolver()
{
	return m_pDriver->GetSerialiser()->GetCallstackResolver();
}

void GLReplay::CreateOutputWindowBackbuffer(OutputWindow &outwin)
{
	if(m_pDriver == NULL) return;
	
	MakeCurrentReplayContext(m_DebugCtx);
	
	WrappedOpenGL &gl = *m_pDriver;
	
	// create fake backbuffer for this output window.
	// We'll make an FBO for this backbuffer on the replay context, so we can
	// use the replay context to do the hard work of rendering to it, then just
	// blit across to the real default framebuffer on the output window context
	gl.glGenFramebuffers(1, &outwin.BlitData.windowFBO);
	gl.glBindFramebuffer(eGL_FRAMEBUFFER, outwin.BlitData.windowFBO);

	gl.glGenTextures(1, &outwin.BlitData.backbuffer);
	gl.glBindTexture(eGL_TEXTURE_2D, outwin.BlitData.backbuffer);
	
	gl.glTexStorage2D(eGL_TEXTURE_2D, 1, eGL_SRGB8, outwin.width, outwin.height); 
	gl.glTexParameteri(eGL_TEXTURE_2D, eGL_TEXTURE_MIN_FILTER, eGL_NEAREST);
	gl.glTexParameteri(eGL_TEXTURE_2D, eGL_TEXTURE_MAG_FILTER, eGL_NEAREST);
	gl.glTexParameteri(eGL_TEXTURE_2D, eGL_TEXTURE_WRAP_S, eGL_CLAMP_TO_EDGE);
	gl.glTexParameteri(eGL_TEXTURE_2D, eGL_TEXTURE_WRAP_T, eGL_CLAMP_TO_EDGE);
	gl.glFramebufferTexture(eGL_FRAMEBUFFER, eGL_COLOR_ATTACHMENT0, outwin.BlitData.backbuffer, 0);

	outwin.BlitData.replayFBO = 0;
}

void GLReplay::InitOutputWindow(OutputWindow &outwin)
{
	if(m_pDriver == NULL) return;
	
	MakeCurrentReplayContext(&outwin);
	
	WrappedOpenGL &gl = *m_pDriver;

	gl.glGenVertexArrays(1, &outwin.BlitData.emptyVAO);
	gl.glBindVertexArray(outwin.BlitData.emptyVAO);
}

bool GLReplay::CheckResizeOutputWindow(uint64_t id)
{
	if(id == 0 || m_OutputWindows.find(id) == m_OutputWindows.end())
		return false;
	
	OutputWindow &outw = m_OutputWindows[id];

	if(outw.wnd == 0)
		return false;

	int32_t w, h;
	GetOutputWindowDimensions(id, w, h);

	if(w != outw.width || h != outw.height)
	{
		outw.width = w;
		outw.height = h;
		
		MakeCurrentReplayContext(m_DebugCtx);
		
		WrappedOpenGL &gl = *m_pDriver;
	
		gl.glDeleteTextures(1, &outw.BlitData.backbuffer);
		gl.glDeleteFramebuffers(1, &outw.BlitData.windowFBO);

		CreateOutputWindowBackbuffer(outw);

		return true;
	}

	return false;
}

void GLReplay::BindOutputWindow(uint64_t id, bool depth)
{
	if(id == 0 || m_OutputWindows.find(id) == m_OutputWindows.end())
		return;
	
	OutputWindow &outw = m_OutputWindows[id];
	
	MakeCurrentReplayContext(m_DebugCtx);

	m_pDriver->glBindFramebuffer(eGL_FRAMEBUFFER, outw.BlitData.windowFBO);
	m_pDriver->glViewport(0, 0, outw.width, outw.height);

	DebugData.outWidth = float(outw.width); DebugData.outHeight = float(outw.height);
}

void GLReplay::ClearOutputWindowColour(uint64_t id, float col[4])
{
	if(id == 0 || m_OutputWindows.find(id) == m_OutputWindows.end())
		return;
	
	OutputWindow &outw = m_OutputWindows[id];
	
	MakeCurrentReplayContext(m_DebugCtx);

	m_pDriver->glClearBufferfv(eGL_COLOR, 0, col);
}

void GLReplay::ClearOutputWindowDepth(uint64_t id, float depth, uint8_t stencil)
{
	if(id == 0 || m_OutputWindows.find(id) == m_OutputWindows.end())
		return;
	
	OutputWindow &outw = m_OutputWindows[id];
	
	MakeCurrentReplayContext(&outw);

	m_pDriver->glClearBufferfv(eGL_DEPTH, 0, &depth);
}

void GLReplay::FlipOutputWindow(uint64_t id)
{
	if(id == 0 || m_OutputWindows.find(id) == m_OutputWindows.end())
		return;
	
	OutputWindow &outw = m_OutputWindows[id];
	
	MakeCurrentReplayContext(&outw);

	WrappedOpenGL &gl = *m_pDriver;

	// go directly to real function so we don't try to bind the 'fake' backbuffer FBO.
	gl.m_Real.glBindFramebuffer(eGL_FRAMEBUFFER, 0);
	gl.glViewport(0, 0, outw.width, outw.height);
	
	gl.glUseProgram(DebugData.blitProg);

	gl.glActiveTexture(eGL_TEXTURE0);
	gl.glBindTexture(eGL_TEXTURE_2D, outw.BlitData.backbuffer);
	gl.glEnable(eGL_FRAMEBUFFER_SRGB);
	
	gl.glBindVertexArray(outw.BlitData.emptyVAO);
	gl.glDrawArrays(eGL_TRIANGLE_STRIP, 0, 4);

	SwapBuffers(&outw);
}

vector<byte> GLReplay::GetBufferData(ResourceId buff, uint32_t offset, uint32_t len)
{
	vector<byte> ret;

	if(m_pDriver->m_Buffers.find(buff) == m_pDriver->m_Buffers.end())
	{
		RDCWARN("Requesting data for non-existant buffer %llu", buff);
		return ret;
	}

	auto &buf = m_pDriver->m_Buffers[buff];

	uint32_t bufsize = (uint32_t)buf.size;
	
	if(len > 0 && offset+len > buf.size)
	{
		RDCWARN("Attempting to read off the end of the array. Will be clamped");

		if(offset < buf.size)
			len = ~0U; // min below will clamp to max size size
		else
			return ret; // offset past buffer size, return empty array
	}
	else if(len == 0)
	{
		len = bufsize;
	}
	
	// need to ensure len+offset doesn't overrun buffer or the glGetBufferSubData call
	// will fail.
	len = RDCMIN(len, bufsize-offset);

	if(len == 0) return ret;
	
	ret.resize(len);
	
	WrappedOpenGL &gl = *m_pDriver;
	
	MakeCurrentReplayContext(m_DebugCtx);

	gl.glBindBuffer(eGL_COPY_READ_BUFFER, buf.resource.name);

	gl.glGetBufferSubData(eGL_COPY_READ_BUFFER, (GLintptr)offset, (GLsizeiptr)len, &ret[0]);

	return ret;
}

bool GLReplay::IsRenderOutput(ResourceId id)
{
	for(int32_t i=0; i < m_CurPipelineState.m_FB.Color.count; i++)
	{
		if(m_CurPipelineState.m_FB.Color[i] == id)
				return true;
	}
	
	if(m_CurPipelineState.m_FB.Depth == id ||
		 m_CurPipelineState.m_FB.Stencil == id)
			return true;

	return false;
}

void GLReplay::CacheTexture(ResourceId id)
{
	FetchTexture tex;
	
	MakeCurrentReplayContext(&m_ReplayCtx);
	
	auto &res = m_pDriver->m_Textures[id];
	WrappedOpenGL &gl = *m_pDriver;
	
	tex.ID = m_pDriver->GetResourceManager()->GetOriginalID(id);
	
	if(res.resource.Namespace == eResUnknown || res.curType == eGL_NONE)
	{
		if(res.resource.Namespace == eResUnknown)
			RDCERR("Details for invalid texture id %llu requested", id);

		tex.name = "<Uninitialised Texture>";
		tex.customName = false;
		tex.format = ResourceFormat();
		tex.dimension = 1;
		tex.resType = eResType_None;
		tex.width = tex.height = tex.depth = 1;
		tex.cubemap = false;
		tex.mips = 1;
		tex.arraysize = 1;
		tex.numSubresources = 1;
		tex.creationFlags = 0;
		tex.msQual = 0;
		tex.msSamp = 1;
		tex.byteSize = 1;

		m_CachedTextures[id] = tex;
		return;
	}
	
	if(res.resource.Namespace == eResRenderbuffer || res.curType == eGL_RENDERBUFFER)
	{
		tex.dimension = 2;
		tex.resType = eResType_Texture2D;
		tex.width = res.width;
		tex.height = res.height;
		tex.depth = 1;
		tex.cubemap = false;
		tex.mips = 1;
		tex.arraysize = 1;
		tex.numSubresources = 1;
		tex.creationFlags = eTextureCreate_RTV;
		tex.msQual = 0;
		tex.msSamp = res.samples;

		tex.format = MakeResourceFormat(gl, eGL_TEXTURE_2D, res.internalFormat);

		if(IsDepthStencilFormat(res.internalFormat))
			tex.creationFlags |= eTextureCreate_DSV;
		
		tex.byteSize = (tex.width*tex.height)*(tex.format.compByteWidth*tex.format.compCount);

		string str = "";
		char name[128] = {0};
		gl.glGetObjectLabel(eGL_RENDERBUFFER, res.resource.name, 127, NULL, name);
		str = name;
		tex.customName = true;

		if(str == "")
		{
			const char *suffix = "";
			const char *ms = "";

			if(tex.msSamp > 1)
				ms = "MS";

			if(tex.creationFlags & eTextureCreate_RTV)
				suffix = " RTV";
			if(tex.creationFlags & eTextureCreate_DSV)
				suffix = " DSV";

			tex.customName = false;

			str = StringFormat::Fmt("Renderbuffer%s%s %llu", ms, suffix, tex.ID);
		}

		tex.name = str;

		m_CachedTextures[id] = tex;
		return;
	}
	
	GLenum target = TextureTarget(res.curType);

	GLenum levelQueryType = target;
	if(levelQueryType == eGL_TEXTURE_CUBE_MAP)
		levelQueryType = eGL_TEXTURE_CUBE_MAP_POSITIVE_X;

	GLint width = 1, height = 1, depth = 1, samples=1;
	gl.glGetTextureLevelParameterivEXT(res.resource.name, levelQueryType, 0, eGL_TEXTURE_WIDTH, &width);
	gl.glGetTextureLevelParameterivEXT(res.resource.name, levelQueryType, 0, eGL_TEXTURE_HEIGHT, &height);
	gl.glGetTextureLevelParameterivEXT(res.resource.name, levelQueryType, 0, eGL_TEXTURE_DEPTH, &depth);
	gl.glGetTextureLevelParameterivEXT(res.resource.name, levelQueryType, 0, eGL_TEXTURE_SAMPLES, &samples);

	if(res.width == 0 && width > 0)
	{
		RDCWARN("TextureData::width didn't get filled out, setting at last minute");
		res.width = width;
	}
	if(res.height == 0 && height > 0)
	{
		RDCWARN("TextureData::height didn't get filled out, setting at last minute");
		res.height = height;
	}
	if(res.depth == 0 && depth > 0)
	{
		RDCWARN("TextureData::depth didn't get filled out, setting at last minute");
		res.depth = depth;
	}

	// reasonably common defaults
	tex.msQual = 0;
	tex.msSamp = 1;
	tex.width = tex.height = tex.depth = tex.arraysize = 1;
	tex.cubemap = false;
	
	switch(target)
	{
		case eGL_TEXTURE_BUFFER:
			tex.resType = eResType_Buffer;
			break;
		case eGL_TEXTURE_1D:
			tex.resType = eResType_Texture1D;
			break;
		case eGL_TEXTURE_2D:
			tex.resType = eResType_Texture2D;
			break;
		case eGL_TEXTURE_3D:
			tex.resType = eResType_Texture3D;
			break;
		case eGL_TEXTURE_1D_ARRAY:
			tex.resType = eResType_Texture1DArray;
			break;
		case eGL_TEXTURE_2D_ARRAY:
			tex.resType = eResType_Texture2DArray;
			break;
		case eGL_TEXTURE_RECTANGLE:
			tex.resType = eResType_TextureRect;
			break;
		case eGL_TEXTURE_2D_MULTISAMPLE:
			tex.resType = eResType_Texture2DMS;
			break;
		case eGL_TEXTURE_2D_MULTISAMPLE_ARRAY:
			tex.resType = eResType_Texture2DMSArray;
			break;
		case eGL_TEXTURE_CUBE_MAP:
			tex.resType = eResType_TextureCube;
			break;
		case eGL_TEXTURE_CUBE_MAP_ARRAY:
			tex.resType = eResType_TextureCubeArray;
			break;

		default:
			tex.resType = eResType_None;
			RDCERR("Unexpected texture enum %s", ToStr::Get(target).c_str());
	}
	
	switch(target)
	{
		case eGL_TEXTURE_1D:
		case eGL_TEXTURE_BUFFER:
			tex.dimension = 1;
			tex.width = (uint32_t)width;
			break;
		case eGL_TEXTURE_1D_ARRAY:
			tex.dimension = 1;
			tex.width = (uint32_t)width;
			tex.arraysize = depth;
			break;
		case eGL_TEXTURE_2D:
		case eGL_TEXTURE_RECTANGLE:
		case eGL_TEXTURE_2D_MULTISAMPLE:
		case eGL_TEXTURE_CUBE_MAP:
			tex.dimension = 2;
			tex.width = (uint32_t)width;
			tex.height = (uint32_t)height;
			tex.depth = (target == eGL_TEXTURE_CUBE_MAP ? 6 : 1);
			tex.cubemap = (target == eGL_TEXTURE_CUBE_MAP);
			tex.msSamp = (target == eGL_TEXTURE_2D_MULTISAMPLE ? samples : 1);
			break;
		case eGL_TEXTURE_2D_ARRAY:
		case eGL_TEXTURE_2D_MULTISAMPLE_ARRAY:
		case eGL_TEXTURE_CUBE_MAP_ARRAY:
			tex.dimension = 2;
			tex.width = (uint32_t)width;
			tex.height = (uint32_t)height;
			tex.depth = (target == eGL_TEXTURE_CUBE_MAP ? 6 : 1);
			tex.arraysize = depth;
			tex.cubemap = (target == eGL_TEXTURE_CUBE_MAP_ARRAY);
			tex.msSamp = (target == eGL_TEXTURE_2D_MULTISAMPLE_ARRAY ? samples : 1);
			break;
		case eGL_TEXTURE_3D:
			tex.dimension = 3;
			tex.width = (uint32_t)width;
			tex.height = (uint32_t)height;
			tex.depth = (uint32_t)depth;
			break;

		default:
			tex.dimension = 2;
			RDCERR("Unexpected texture enum %s", ToStr::Get(target).c_str());
	}
	
	tex.creationFlags = res.creationFlags;
	if(res.resource.name == gl.m_FakeBB_Color || res.resource.name == gl.m_FakeBB_DepthStencil)
		tex.creationFlags |= eTextureCreate_SwapBuffer;

	// surely this will be the same for each level... right? that would be insane if it wasn't
	GLint fmt = 0;
	gl.glGetTextureLevelParameterivEXT(res.resource.name, levelQueryType, 0, eGL_TEXTURE_INTERNAL_FORMAT, &fmt);

	tex.format = MakeResourceFormat(gl, target, (GLenum)fmt);
	
	if(tex.format.compType == eCompType_Depth)
		tex.creationFlags |= eTextureCreate_DSV;

	string str = "";
	char name[128] = {0};
	gl.glGetObjectLabel(eGL_TEXTURE, res.resource.name, 127, NULL, name);
	str = name;
	tex.customName = true;

	if(str == "")
	{
		const char *suffix = "";
		const char *ms = "";

		if(tex.msSamp > 1)
			ms = "MS";

		if(tex.creationFlags & eTextureCreate_RTV)
			suffix = " RTV";
		if(tex.creationFlags & eTextureCreate_DSV)
			suffix = " DSV";

		tex.customName = false;

		if(tex.cubemap)
		{
			if(tex.arraysize > 6)
				str = StringFormat::Fmt("TextureCube%sArray%s %llu", ms, suffix, tex.ID);
			else
				str = StringFormat::Fmt("TextureCube%s%s %llu", ms, suffix, tex.ID);
		}
		else
		{
			if(tex.arraysize > 1)
				str = StringFormat::Fmt("Texture%dD%sArray%s %llu", tex.dimension, ms, suffix, tex.ID);
			else
				str = StringFormat::Fmt("Texture%dD%s%s %llu", tex.dimension, ms, suffix, tex.ID);
		}
	}

	tex.name = str;

	if(target == eGL_TEXTURE_BUFFER)
	{
		tex.dimension = 1;
		tex.width = tex.height = tex.depth = 1;
		tex.cubemap = false;
		tex.mips = 1;
		tex.arraysize = 1;
		tex.numSubresources = 1;
		tex.creationFlags = eTextureCreate_SRV;
		tex.msQual = tex.msSamp = 0;
		tex.byteSize = 0;

		gl.glGetTextureLevelParameterivEXT(res.resource.name, levelQueryType, 0, eGL_TEXTURE_BUFFER_SIZE, (GLint *)&tex.byteSize);
		tex.width = uint32_t(tex.byteSize/(tex.format.compByteWidth*tex.format.compCount));
		
		m_CachedTextures[id] = tex;
		return;
	}

	tex.mips = GetNumMips(gl.m_Real, target, res.resource.name, tex.width, tex.height, tex.depth);

	tex.numSubresources = tex.mips*tex.arraysize;
	
	GLint compressed;
	gl.glGetTextureLevelParameterivEXT(res.resource.name, levelQueryType, 0, eGL_TEXTURE_COMPRESSED, &compressed);
	tex.byteSize = 0;
	for(uint32_t a=0; a < tex.arraysize; a++)
	{
		for(uint32_t m=0; m < tex.mips; m++)
		{
			if(compressed)
			{
				gl.glGetTextureLevelParameterivEXT(res.resource.name, levelQueryType, m, eGL_TEXTURE_COMPRESSED_IMAGE_SIZE, &compressed);
				tex.byteSize += compressed;
			}
			else if(tex.format.special)
			{
				tex.byteSize += GetByteSize(RDCMAX(1U, tex.width>>m), RDCMAX(1U, tex.height>>m), RDCMAX(1U, tex.depth>>m), 
																		GetBaseFormat((GLenum)fmt), GetDataType((GLenum)fmt));
			}
			else
			{
				tex.byteSize += RDCMAX(1U, tex.width>>m)*RDCMAX(1U, tex.height>>m)*RDCMAX(1U, tex.depth>>m)*
													tex.format.compByteWidth*tex.format.compCount;
			}
		}
	}

	m_CachedTextures[id] = tex;
}

FetchBuffer GLReplay::GetBuffer(ResourceId id)
{
	FetchBuffer ret;
	
	MakeCurrentReplayContext(&m_ReplayCtx);
	
	auto &res = m_pDriver->m_Buffers[id];

	if(res.resource.Namespace == eResUnknown)
	{
		RDCERR("Details for invalid buffer id %llu requested", id);
		RDCEraseEl(ret);
		return ret;
	}
	
	WrappedOpenGL &gl = *m_pDriver;
	
	ret.ID = m_pDriver->GetResourceManager()->GetOriginalID(id);

	if(res.curType == eGL_NONE)
	{
		ret.byteSize = 0;
		ret.creationFlags = 0;
		ret.customName = false;
		ret.length = 0;
		ret.structureSize = 0;
		return ret;
	}

	gl.glBindBuffer(res.curType, res.resource.name);

	ret.structureSize = 0;

	ret.creationFlags = 0;
	switch(res.curType)
	{
		case eGL_ARRAY_BUFFER:
			ret.creationFlags = eBufferCreate_VB;
			break;
		case eGL_ELEMENT_ARRAY_BUFFER:
			ret.creationFlags = eBufferCreate_IB;
			break;
		case eGL_UNIFORM_BUFFER:
			ret.creationFlags = eBufferCreate_CB;
			break;
		case eGL_SHADER_STORAGE_BUFFER:
			ret.creationFlags = eBufferCreate_UAV;
			break;
		case eGL_DRAW_INDIRECT_BUFFER:
		case eGL_DISPATCH_INDIRECT_BUFFER:
		case eGL_PARAMETER_BUFFER_ARB:
			ret.creationFlags = eBufferCreate_Indirect;
			break;
		case eGL_PIXEL_PACK_BUFFER:
		case eGL_PIXEL_UNPACK_BUFFER:
		case eGL_COPY_WRITE_BUFFER:
		case eGL_COPY_READ_BUFFER:
		case eGL_QUERY_BUFFER:
		case eGL_TEXTURE_BUFFER:
		case eGL_TRANSFORM_FEEDBACK_BUFFER:
		case eGL_ATOMIC_COUNTER_BUFFER:
			break;
		default:
			RDCERR("Unexpected buffer type %s", ToStr::Get(res.curType).c_str());
	}

	GLint size;
	gl.glGetBufferParameteriv(res.curType, eGL_BUFFER_SIZE, &size);

	ret.byteSize = ret.length = (uint32_t)size;
	
	if(res.size == 0)
	{
		RDCWARN("BufferData::size didn't get filled out, setting at last minute");
		res.size = ret.byteSize;
	}

	string str = "";
	char name[128] = {0};
	gl.glGetObjectLabel(eGL_BUFFER, res.resource.name, 127, NULL, name);
	str = name;
	ret.customName = true;

	if(str == "")
	{
		ret.customName = false;
		str = StringFormat::Fmt("Buffer %llu", ret.ID);
	}

	ret.name = str;

	return ret;
}

vector<DebugMessage> GLReplay::GetDebugMessages()
{
	return m_pDriver->GetDebugMessages();
}

ShaderReflection *GLReplay::GetShader(ResourceId id)
{
	WrappedOpenGL &gl = *m_pDriver;
	
	MakeCurrentReplayContext(&m_ReplayCtx);
	
	void *ctx = m_ReplayCtx.ctx;
	
	auto &shaderDetails = m_pDriver->m_Shaders[id];
	
	if(shaderDetails.prog == 0)
	{
		RDCERR("Can't get shader details without separable program");
		return NULL;
	}

	return &shaderDetails.reflection;
}

#pragma endregion

#pragma region Mostly Implemented

void GLReplay::GetMapping(WrappedOpenGL &gl, GLuint curProg, int shadIdx, ShaderReflection *refl, ShaderBindpointMapping &mapping)
{
	// in case of bugs, we readback into this array instead of
	GLint dummyReadback[32];

#if !defined(RELEASE)
	for(size_t i=1; i < ARRAY_COUNT(dummyReadback); i++)
		dummyReadback[i] = 0x6c7b8a9d;
#endif

	GLenum refEnum[] = {
		eGL_REFERENCED_BY_VERTEX_SHADER,
		eGL_REFERENCED_BY_TESS_CONTROL_SHADER,
		eGL_REFERENCED_BY_TESS_EVALUATION_SHADER,
		eGL_REFERENCED_BY_GEOMETRY_SHADER,
		eGL_REFERENCED_BY_FRAGMENT_SHADER,
		eGL_REFERENCED_BY_COMPUTE_SHADER,
	};
	
	create_array_uninit(mapping.Resources, refl->Resources.count);
	for(int32_t i=0; i < refl->Resources.count; i++)
	{
		if(refl->Resources.elems[i].IsSRV && refl->Resources.elems[i].IsTexture)
		{
			GLint loc = gl.glGetUniformLocation(curProg, refl->Resources.elems[i].name.elems);
			if(loc >= 0)
			{
				gl.glGetUniformiv(curProg, loc, dummyReadback);
				mapping.Resources[i].bind = dummyReadback[0];
			}
		}
		else
		{
			mapping.Resources[i].bind = -1;
		}

		GLuint idx = gl.glGetProgramResourceIndex(curProg, eGL_UNIFORM, refl->Resources.elems[i].name.elems);
		if(idx == GL_INVALID_INDEX)
		{
			mapping.Resources[i].used = false;
		}
		else
		{
			GLint used = 0;
			gl.glGetProgramResourceiv(curProg, eGL_UNIFORM, idx, 1, &refEnum[shadIdx], 1, NULL, &used);
			mapping.Resources[i].used = (used != 0);
		}
	}
	
	create_array_uninit(mapping.ConstantBlocks, refl->ConstantBlocks.count);
	for(int32_t i=0; i < refl->ConstantBlocks.count; i++)
	{
		if(refl->ConstantBlocks.elems[i].bufferBacked)
		{
			GLint loc = gl.glGetUniformBlockIndex(curProg, refl->ConstantBlocks.elems[i].name.elems);
			if(loc >= 0)
			{
				gl.glGetActiveUniformBlockiv(curProg, loc, eGL_UNIFORM_BLOCK_BINDING, dummyReadback);
				mapping.ConstantBlocks[i].bind = dummyReadback[0];
			}
		}
		else
		{
			mapping.ConstantBlocks[i].bind = -1;
		}

		if(!refl->ConstantBlocks.elems[i].bufferBacked)
		{
			mapping.ConstantBlocks[i].used = true;
		}
		else
		{
			GLuint idx = gl.glGetProgramResourceIndex(curProg, eGL_UNIFORM_BLOCK, refl->ConstantBlocks.elems[i].name.elems);
			if(idx == GL_INVALID_INDEX)
			{
				mapping.ConstantBlocks[i].used = false;
			}
			else
			{
				GLint used = 0;
				gl.glGetProgramResourceiv(curProg, eGL_UNIFORM_BLOCK, idx, 1, &refEnum[shadIdx], 1, NULL, &used);
				mapping.ConstantBlocks[i].used = (used != 0);
			}
		}
	}
	
	GLint numVAttribBindings = 16;
	gl.glGetIntegerv(eGL_MAX_VERTEX_ATTRIBS, &numVAttribBindings);

	create_array_uninit(mapping.InputAttributes, numVAttribBindings);
	for(int32_t i=0; i < numVAttribBindings; i++)
		mapping.InputAttributes[i] = -1;

	// override identity map with bindings
	if(shadIdx == 0)
	{
		for(int32_t i=0; i < refl->InputSig.count; i++)
		{
			GLint loc = gl.glGetAttribLocation(curProg, refl->InputSig.elems[i].varName.elems);

			if(loc >= 0 && loc < numVAttribBindings)
			{
				mapping.InputAttributes[loc] = i;
			}
		}
	}

#if !defined(RELEASE)
	for(size_t i=1; i < ARRAY_COUNT(dummyReadback); i++)
		if(dummyReadback[i] != 0x6c7b8a9d)
			RDCERR("Invalid uniform readback - data beyond first element modified!");
#endif
}

void GLReplay::SavePipelineState()
{
	GLPipelineState &pipe = m_CurPipelineState;
	WrappedOpenGL &gl = *m_pDriver;
	GLResourceManager *rm = m_pDriver->GetResourceManager();

	MakeCurrentReplayContext(&m_ReplayCtx);
	
	GLRenderState rs(&gl.GetHookset(), NULL, READING);
	rs.FetchState(m_ReplayCtx.ctx, &gl);

	// Index buffer

	void *ctx = m_ReplayCtx.ctx;

	GLuint ibuffer = 0;
	gl.glGetIntegerv(eGL_ELEMENT_ARRAY_BUFFER_BINDING, (GLint*)&ibuffer);
	pipe.m_VtxIn.ibuffer = rm->GetOriginalID(rm->GetID(BufferRes(ctx, ibuffer)));

	pipe.m_VtxIn.primitiveRestart = rs.Enabled[GLRenderState::eEnabled_PrimitiveRestart];
	pipe.m_VtxIn.restartIndex = rs.Enabled[GLRenderState::eEnabled_PrimitiveRestartFixedIndex] ? ~0U : rs.PrimitiveRestartIndex;

	// Vertex buffers and attributes
	GLint numVBufferBindings = 16;
	gl.glGetIntegerv(eGL_MAX_VERTEX_ATTRIB_BINDINGS, &numVBufferBindings);
	
	GLint numVAttribBindings = 16;
	gl.glGetIntegerv(eGL_MAX_VERTEX_ATTRIBS, &numVAttribBindings);

	create_array_uninit(pipe.m_VtxIn.vbuffers, numVBufferBindings);
	create_array_uninit(pipe.m_VtxIn.attributes, numVAttribBindings);

	for(GLuint i=0; i < (GLuint)numVBufferBindings; i++)
	{
		GLuint buffer = GetBoundVertexBuffer(gl.m_Real, i);

		pipe.m_VtxIn.vbuffers[i].Buffer = rm->GetOriginalID(rm->GetID(BufferRes(ctx, buffer)));

		gl.glGetIntegeri_v(eGL_VERTEX_BINDING_STRIDE, i, (GLint *)&pipe.m_VtxIn.vbuffers[i].Stride);
		gl.glGetIntegeri_v(eGL_VERTEX_BINDING_OFFSET, i, (GLint *)&pipe.m_VtxIn.vbuffers[i].Offset);
		gl.glGetIntegeri_v(eGL_VERTEX_BINDING_DIVISOR, i, (GLint *)&pipe.m_VtxIn.vbuffers[i].Divisor);
	}
	
	for(GLuint i=0; i < (GLuint)numVAttribBindings; i++)
	{
		gl.glGetVertexAttribiv(i, eGL_VERTEX_ATTRIB_ARRAY_ENABLED, (GLint *)&pipe.m_VtxIn.attributes[i].Enabled);
		gl.glGetVertexAttribiv(i, eGL_VERTEX_ATTRIB_BINDING, (GLint *)&pipe.m_VtxIn.attributes[i].BufferSlot);
		gl.glGetVertexAttribiv(i, eGL_VERTEX_ATTRIB_RELATIVE_OFFSET, (GLint*)&pipe.m_VtxIn.attributes[i].RelativeOffset);

		GLenum type = eGL_FLOAT;
		GLint normalized = 0;
		
		gl.glGetVertexAttribiv(i, eGL_VERTEX_ATTRIB_ARRAY_TYPE, (GLint *)&type);
		gl.glGetVertexAttribiv(i, eGL_VERTEX_ATTRIB_ARRAY_NORMALIZED, &normalized);

		GLint integer = 0;
		gl.glGetVertexAttribiv(i, eGL_VERTEX_ATTRIB_ARRAY_INTEGER, &integer);
		
		RDCEraseEl(pipe.m_VtxIn.attributes[i].GenericValue);
		gl.glGetVertexAttribfv(i, eGL_CURRENT_VERTEX_ATTRIB, &pipe.m_VtxIn.attributes[i].GenericValue.x);

		ResourceFormat fmt;

		fmt.special = false;
		fmt.compCount = 4;
		gl.glGetVertexAttribiv(i, eGL_VERTEX_ATTRIB_ARRAY_SIZE, (GLint *)&fmt.compCount);

		bool intComponent = !normalized || integer;
		
		switch(type)
		{
			default:
			case eGL_BYTE:
				fmt.compByteWidth = 1;
				fmt.compType = intComponent ? eCompType_SInt : eCompType_SNorm;
				fmt.strname = (fmt.compCount > 1 ? StringFormat::Fmt("GL_BYTE%d", fmt.compCount) : string("GL_BYTE")) + (intComponent ? "" : "_SNORM");
				break;
			case eGL_UNSIGNED_BYTE:
				fmt.compByteWidth = 1;
				fmt.compType = intComponent ? eCompType_UInt : eCompType_UNorm;
				fmt.strname = (fmt.compCount > 1 ? StringFormat::Fmt("GL_UNSIGNED_BYTE%d", fmt.compCount) : string("GL_UNSIGNED_BYTE")) + (intComponent ? "" : "_UNORM");
				break;
			case eGL_SHORT:
				fmt.compByteWidth = 2;
				fmt.compType = intComponent ? eCompType_SInt : eCompType_SNorm;
				fmt.strname = (fmt.compCount > 1 ? StringFormat::Fmt("GL_SHORT%d", fmt.compCount) : string("GL_SHORT")) + (intComponent ? "" : "_SNORM");
				break;
			case eGL_UNSIGNED_SHORT:
				fmt.compByteWidth = 2;
				fmt.compType = intComponent ? eCompType_UInt : eCompType_UNorm;
				fmt.strname = (fmt.compCount > 1 ? StringFormat::Fmt("GL_UNSIGNED_SHORT%d", fmt.compCount) : string("GL_UNSIGNED_SHORT")) + (intComponent ? "" : "_UNORM");
				break;
			case eGL_INT:
				fmt.compByteWidth = 4;
				fmt.compType = intComponent ? eCompType_SInt : eCompType_SNorm;
				fmt.strname = (fmt.compCount > 1 ? StringFormat::Fmt("GL_INT%d", fmt.compCount) : string("GL_INT")) + (intComponent ? "" : "_SNORM");
				break;
			case eGL_UNSIGNED_INT:
				fmt.compByteWidth = 4;
				fmt.compType = intComponent ? eCompType_UInt : eCompType_UNorm;
				fmt.strname = (fmt.compCount > 1 ? StringFormat::Fmt("GL_UNSIGNED_INT%d", fmt.compCount) : string("GL_UNSIGNED_INT")) + (intComponent ? "" : "_UNORM");
				break;
			case eGL_FLOAT:
				fmt.compByteWidth = 4;
				fmt.compType = eCompType_Float;
				fmt.strname = (fmt.compCount > 1 ? StringFormat::Fmt("GL_FLOAT%d", fmt.compCount) : string("GL_FLOAT"));
				break;
			case eGL_DOUBLE:
				fmt.compByteWidth = 8;
				fmt.compType = eCompType_Double;
				fmt.strname = (fmt.compCount > 1 ? StringFormat::Fmt("GL_DOUBLE%d", fmt.compCount) : string("GL_DOUBLE"));
				break;
			case eGL_HALF_FLOAT:
				fmt.compByteWidth = 2;
				fmt.compType = eCompType_Float;
				fmt.strname = (fmt.compCount > 1 ? StringFormat::Fmt("GL_HALF_FLOAT%d", fmt.compCount) : string("GL_HALF_FLOAT"));
				break;
			case eGL_INT_2_10_10_10_REV:
				fmt.special = true;
				fmt.specialFormat = eSpecial_R10G10B10A2;
				fmt.compCount = 4;
				fmt.compType = eCompType_UInt;
				fmt.strname = "GL_INT_2_10_10_10_REV";
				break;
			case eGL_UNSIGNED_INT_2_10_10_10_REV:
				fmt.special = true;
				fmt.specialFormat = eSpecial_R10G10B10A2;
				fmt.compCount = 4;
				fmt.compType = eCompType_SInt;
				fmt.strname = "GL_UNSIGNED_INT_2_10_10_10_REV";
				break;
			case eGL_UNSIGNED_INT_10F_11F_11F_REV:
				fmt.special = true;
				fmt.specialFormat = eSpecial_R11G11B10;
				fmt.compCount = 3;
				fmt.compType = eCompType_SInt;
				fmt.strname = "GL_UNSIGNED_INT_10F_11F_11F_REV";
				break;
		}
		
		if(fmt.compCount == eGL_BGRA)
		{
			fmt.compCount = 4;
			fmt.special = true;
			fmt.specialFormat = eSpecial_B8G8R8A8;
			fmt.compType = eCompType_UNorm;

			if(type == eGL_UNSIGNED_BYTE)
			{
				fmt.specialFormat = eSpecial_B8G8R8A8;
				fmt.compType = eCompType_UNorm;
				fmt.strname = "GL_BGRA8";
			}
			else if(type == eGL_UNSIGNED_INT_2_10_10_10_REV || type == eGL_INT_2_10_10_10_REV)
			{
				fmt.specialFormat = eSpecial_R10G10B10A2;
				fmt.compType = type == eGL_UNSIGNED_INT_2_10_10_10_REV ? eCompType_UInt : eCompType_SInt;
				fmt.strname = type == eGL_UNSIGNED_INT_2_10_10_10_REV ? "GL_UNSIGNED_INT_2_10_10_10_REV" : "GL_INT_2_10_10_10_REV";
			}
			else
			{
				RDCERR("Unexpected BGRA type");
			}
			RDCASSERT(type == eGL_UNSIGNED_BYTE);
		}

		pipe.m_VtxIn.attributes[i].Format = fmt;
	}
	// Shader stages & Textures
	
	GLint numTexUnits = 8;
	gl.glGetIntegerv(eGL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &numTexUnits);
	create_array_uninit(pipe.Textures, numTexUnits);
	create_array_uninit(pipe.Samplers, numTexUnits);

	GLenum activeTexture = eGL_TEXTURE0;
	gl.glGetIntegerv(eGL_ACTIVE_TEXTURE, (GLint*)&activeTexture);

	pipe.m_VS.stage = eShaderStage_Vertex;
	pipe.m_TCS.stage = eShaderStage_Tess_Control;
	pipe.m_TES.stage = eShaderStage_Tess_Eval;
	pipe.m_GS.stage = eShaderStage_Geometry;
	pipe.m_FS.stage = eShaderStage_Fragment;
	pipe.m_CS.stage = eShaderStage_Compute;
	
	GLuint curProg = 0;
	gl.glGetIntegerv(eGL_CURRENT_PROGRAM, (GLint*)&curProg);
	
	GLPipelineState::ShaderStage *stages[6] = {
		&pipe.m_VS,
		&pipe.m_TCS,
		&pipe.m_TES,
		&pipe.m_GS,
		&pipe.m_FS,
		&pipe.m_CS,
	};
	ShaderReflection *refls[6] = { NULL };
	ShaderBindpointMapping *mappings[6] = { NULL };

	for(int i=0; i < 6; i++)
	{
		stages[i]->Shader = ResourceId();
		stages[i]->ShaderDetails = NULL;
		stages[i]->BindpointMapping.ConstantBlocks.Delete();
		stages[i]->BindpointMapping.Resources.Delete();
	}

	if(curProg == 0)
	{
		gl.glGetIntegerv(eGL_PROGRAM_PIPELINE_BINDING, (GLint*)&curProg);
	
		if(curProg == 0)
		{
			for(GLint unit=0; unit < numTexUnits; unit++)
			{
				RDCEraseEl(pipe.Textures[unit]);
				RDCEraseEl(pipe.Samplers[unit]);
			}
		}
		else
		{
			ResourceId id = rm->GetID(ProgramPipeRes(ctx, curProg));
			auto &pipeDetails = m_pDriver->m_Pipelines[id];

			for(size_t i=0; i < ARRAY_COUNT(pipeDetails.stageShaders); i++)
			{
				if(pipeDetails.stageShaders[i] != ResourceId())
				{
					curProg = rm->GetCurrentResource(pipeDetails.stagePrograms[i]).name;
					stages[i]->Shader = rm->GetOriginalID(pipeDetails.stageShaders[i]);
					refls[i] = GetShader(pipeDetails.stageShaders[i]);
					GetMapping(gl, curProg, (int)i, refls[i], stages[i]->BindpointMapping);
					mappings[i] = &stages[i]->BindpointMapping;
				}
				else
				{
					stages[i]->Shader = ResourceId();
				}
			}
		}
	}
	else
	{
		auto &progDetails = m_pDriver->m_Programs[rm->GetID(ProgramRes(ctx, curProg))];
		
		for(size_t i=0; i < ARRAY_COUNT(progDetails.stageShaders); i++)
		{
			if(progDetails.stageShaders[i] != ResourceId())
			{
				stages[i]->Shader = rm->GetOriginalID(progDetails.stageShaders[i]);
				refls[i] = GetShader(progDetails.stageShaders[i]);
				GetMapping(gl, curProg, (int)i, refls[i], stages[i]->BindpointMapping);
				mappings[i] = &stages[i]->BindpointMapping;
			}
		}
	}

	// GL is ass-backwards in its handling of texture units. When a shader is active
	// the types in the glsl samplers inform which targets are used from which texture units
	//
	// So texture unit 5 can have a 2D bound (texture 52) and a Cube bound (texture 77).
	// * if a uniform sampler2D has value 5 then the 2D texture is used, and we sample from 52
	// * if a uniform samplerCube has value 5 then the Cube texture is used, and we sample from 77
	// It's illegal for both a sampler2D and samplerCube to both have the same value (or any two
	// different types). It makes it all rather pointless and needlessly complex.
	//
	// What we have to do then, is consider the program, look at the values of the uniforms, and
	// then get the appropriate current binding based on the uniform type. We can warn/alert the
	// user if we hit the illegal case of two uniforms with different types but the same value
	//
	// Handling is different if no shaders are active, but we don't consider that case.

	for(GLint unit=0; unit < numTexUnits; unit++)
	{
		GLenum binding = eGL_NONE;
		GLenum target = eGL_NONE;
		ShaderResourceType resType = eResType_None;

		bool shadow = false;

		for(size_t s=0; s < ARRAY_COUNT(refls); s++)
		{
			if(refls[s] == NULL) continue;

			for(int32_t r=0; r < refls[s]->Resources.count; r++)
			{
				// bindPoint is the uniform value for this sampler
				if(mappings[s]->Resources[ refls[s]->Resources[r].bindPoint ].bind == unit)
				{
					GLenum t = eGL_NONE;

					if(strstr(refls[s]->Resources[r].variableType.descriptor.name.elems, "Shadow"))
						shadow = true;

					switch(refls[s]->Resources[r].resType)
					{
						case eResType_None:
							target = eGL_NONE;
							break;
						case eResType_Buffer:
							target = eGL_TEXTURE_BUFFER;
							break;
						case eResType_Texture1D:
							target = eGL_TEXTURE_1D;
							break;
						case eResType_Texture1DArray:
							target = eGL_TEXTURE_1D_ARRAY;
							break;
						case eResType_Texture2D:
							target = eGL_TEXTURE_2D;
							break;
						case eResType_TextureRect:
							target = eGL_TEXTURE_RECTANGLE;
							break;
						case eResType_Texture2DArray:
							target = eGL_TEXTURE_2D_ARRAY;
							break;
						case eResType_Texture2DMS:
							target = eGL_TEXTURE_2D_MULTISAMPLE;
							break;
						case eResType_Texture2DMSArray:
							target = eGL_TEXTURE_2D_MULTISAMPLE_ARRAY;
							break;
						case eResType_Texture3D:
							target = eGL_TEXTURE_3D;
							break;
						case eResType_TextureCube:
							target = eGL_TEXTURE_CUBE_MAP;
							break;
						case eResType_TextureCubeArray:
							target = eGL_TEXTURE_CUBE_MAP_ARRAY;
							break;
					}
					
					if(target != eGL_NONE)
						t = TextureBinding(target);

					resType = refls[s]->Resources[r].resType;

					if(binding == eGL_NONE)
					{
						binding = t;
					}
					else if(binding == t)
					{
						// two uniforms with the same type pointing to the same slot is fine
						binding = t;
					}
					else if(binding != t)
					{
						RDCWARN("Two uniforms pointing to texture unit %d with types %s and %s", unit, ToStr::Get(binding).c_str(), ToStr::Get(t).c_str());
					}
				}
			}
		}

		if(binding != eGL_NONE)
		{
			gl.glActiveTexture(GLenum(eGL_TEXTURE0+unit));

			GLuint tex;
			gl.glGetIntegerv(binding, (GLint *)&tex);

			// very bespoke/specific
			GLint firstSlice = 0;

			if(target != eGL_TEXTURE_BUFFER)
				gl.glGetTexParameteriv(target, eGL_TEXTURE_VIEW_MIN_LEVEL, &firstSlice);

			pipe.Textures[unit].Resource = rm->GetOriginalID(rm->GetID(TextureRes(ctx, tex)));
			pipe.Textures[unit].FirstSlice = (uint32_t)firstSlice;
			pipe.Textures[unit].ResType = resType;
			
			GLuint samp;
			gl.glGetIntegerv(eGL_SAMPLER_BINDING, (GLint *)&samp);

			pipe.Samplers[unit].Samp = rm->GetOriginalID(rm->GetID(SamplerRes(ctx, samp)));

			if(target != eGL_TEXTURE_BUFFER)
			{
				if(samp != 0)
					gl.glGetSamplerParameterfv(samp, eGL_TEXTURE_BORDER_COLOR, &pipe.Samplers[unit].BorderColor[0]);
				else
					gl.glGetTexParameterfv(target, eGL_TEXTURE_BORDER_COLOR, &pipe.Samplers[unit].BorderColor[0]);

				pipe.Samplers[unit].UseBorder = false;
				pipe.Samplers[unit].UseComparison = shadow;

				GLint v;
				v=0;
				if(samp != 0)
					gl.glGetSamplerParameteriv(samp, eGL_TEXTURE_WRAP_S, &v);
				else
					gl.glGetTexParameteriv(target, eGL_TEXTURE_WRAP_S, &v);
				pipe.Samplers[unit].AddressS = SamplerString((GLenum)v);
				pipe.Samplers[unit].UseBorder |= (v == eGL_CLAMP_TO_BORDER);

				v=0;
				if(samp != 0)
					gl.glGetSamplerParameteriv(samp, eGL_TEXTURE_WRAP_T, &v);
				else
					gl.glGetTexParameteriv(target, eGL_TEXTURE_WRAP_T, &v);
				pipe.Samplers[unit].AddressT = SamplerString((GLenum)v);
				pipe.Samplers[unit].UseBorder |= (v == eGL_CLAMP_TO_BORDER);

				v=0;
				if(samp != 0)
					gl.glGetSamplerParameteriv(samp, eGL_TEXTURE_WRAP_R, &v);
				else
					gl.glGetTexParameteriv(target, eGL_TEXTURE_WRAP_R, &v);
				pipe.Samplers[unit].AddressR = SamplerString((GLenum)v);
				pipe.Samplers[unit].UseBorder |= (v == eGL_CLAMP_TO_BORDER);

				v=0;
				if(samp != 0)
					gl.glGetSamplerParameteriv(samp, eGL_TEXTURE_COMPARE_FUNC, &v);
				else
					gl.glGetTexParameteriv(target, eGL_TEXTURE_COMPARE_FUNC, &v);
				pipe.Samplers[unit].Comparison = ToStr::Get((GLenum)v).substr(3).c_str();

				v=0;
				if(samp != 0)
					gl.glGetSamplerParameteriv(samp, eGL_TEXTURE_MIN_FILTER, &v);
				else
					gl.glGetTexParameteriv(target, eGL_TEXTURE_MIN_FILTER, &v);
				pipe.Samplers[unit].MinFilter = SamplerString((GLenum)v);

				v=0;
				if(samp != 0)
					gl.glGetSamplerParameteriv(samp, eGL_TEXTURE_MAG_FILTER, &v);
				else
					gl.glGetTexParameteriv(target, eGL_TEXTURE_MAG_FILTER, &v);
				pipe.Samplers[unit].MagFilter = SamplerString((GLenum)v);

				if(samp != 0)
					gl.glGetSamplerParameterfv(samp, eGL_TEXTURE_MAX_ANISOTROPY_EXT, &pipe.Samplers[unit].MaxAniso);
				else
					gl.glGetTexParameterfv(target, eGL_TEXTURE_MAX_ANISOTROPY_EXT, &pipe.Samplers[unit].MaxAniso);

				gl.glGetTexParameterfv(target, eGL_TEXTURE_MAX_LOD, &pipe.Samplers[unit].MaxLOD);
				gl.glGetTexParameterfv(target, eGL_TEXTURE_MIN_LOD, &pipe.Samplers[unit].MinLOD);
				gl.glGetTexParameterfv(target, eGL_TEXTURE_LOD_BIAS, &pipe.Samplers[unit].MipLODBias);
			}
			else
			{
				// texture buffers don't support sampling
				RDCEraseEl(pipe.Samplers[unit].BorderColor);
				pipe.Samplers[unit].AddressS = "";
				pipe.Samplers[unit].AddressT = "";
				pipe.Samplers[unit].AddressR = "";
				pipe.Samplers[unit].Comparison = "";
				pipe.Samplers[unit].MinFilter = "";
				pipe.Samplers[unit].MagFilter = "";
				pipe.Samplers[unit].UseBorder = false;
				pipe.Samplers[unit].UseComparison = false;
				pipe.Samplers[unit].MaxAniso = 0.0f;
				pipe.Samplers[unit].MaxLOD = 0.0f;
				pipe.Samplers[unit].MinLOD = 0.0f;
				pipe.Samplers[unit].MipLODBias = 0.0f;
			}
		}
		else
		{
			// what should we do in this case? there could be something bound just not used,
			// it'd be nice to return that
		}
	}

	gl.glActiveTexture(activeTexture);
	
	create_array_uninit(pipe.UniformBuffers, ARRAY_COUNT(rs.UniformBinding));
	for(int32_t b=0; b < pipe.UniformBuffers.count; b++)
	{
		if(rs.UniformBinding[b].name == 0)
		{
			pipe.UniformBuffers[b].Resource = ResourceId();
			pipe.UniformBuffers[b].Offset = pipe.UniformBuffers[b].Size = 0;
		}
		else
		{
			pipe.UniformBuffers[b].Resource = rm->GetOriginalID(rm->GetID(BufferRes(ctx, rs.UniformBinding[b].name)));
			pipe.UniformBuffers[b].Offset = rs.UniformBinding[b].start;
			pipe.UniformBuffers[b].Size = rs.UniformBinding[b].size;
		}
	}


	// Vertex post processing and rasterization

	RDCCOMPILE_ASSERT(ARRAY_COUNT(rs.Viewports) == ARRAY_COUNT(rs.DepthRanges), "GL Viewport count does not match depth ranges count");
	create_array_uninit(pipe.m_Rasterizer.Viewports, ARRAY_COUNT(rs.Viewports));
	for (int32_t v = 0; v < pipe.m_Rasterizer.Viewports.count; ++v)
	{
		pipe.m_Rasterizer.Viewports[v].Left = rs.Viewports[v].x;
		pipe.m_Rasterizer.Viewports[v].Bottom = rs.Viewports[v].y;
		pipe.m_Rasterizer.Viewports[v].Width = rs.Viewports[v].width;
		pipe.m_Rasterizer.Viewports[v].Height = rs.Viewports[v].height;
		pipe.m_Rasterizer.Viewports[v].MinDepth = rs.DepthRanges[v].nearZ;
		pipe.m_Rasterizer.Viewports[v].MaxDepth = rs.DepthRanges[v].farZ;
	}

	create_array_uninit(pipe.m_Rasterizer.Scissors, ARRAY_COUNT(rs.Scissors));
	for (int32_t s = 0; s < pipe.m_Rasterizer.Scissors.count; ++s)
	{
		pipe.m_Rasterizer.Scissors[s].Left = rs.Scissors[s].x;
		pipe.m_Rasterizer.Scissors[s].Bottom = rs.Scissors[s].y;
		pipe.m_Rasterizer.Scissors[s].Width = rs.Scissors[s].width;
		pipe.m_Rasterizer.Scissors[s].Height = rs.Scissors[s].height;
		pipe.m_Rasterizer.Scissors[s].Enabled = rs.Scissors[s].enabled;
	}

	int polygonOffsetEnableEnum;
	switch (rs.PolygonMode)
	{
		default:
			RDCWARN("Unexpected value for POLYGON_MODE %x", rs.PolygonMode);
		case eGL_FILL:
			pipe.m_Rasterizer.m_State.FillMode = eFill_Solid;
			polygonOffsetEnableEnum = GLRenderState::eEnabled_PolyOffsetFill;
			break;
		case eGL_LINES:
			pipe.m_Rasterizer.m_State.FillMode = eFill_Wireframe;
			polygonOffsetEnableEnum = GLRenderState::eEnabled_PolyOffsetLine;
			break;
		case eGL_POINT:
			pipe.m_Rasterizer.m_State.FillMode = eFill_Point;
			polygonOffsetEnableEnum = GLRenderState::eEnabled_PolyOffsetPoint;
			break;
	}
	if (rs.Enabled[polygonOffsetEnableEnum])
	{
		pipe.m_Rasterizer.m_State.DepthBias = rs.PolygonOffset[1];
		pipe.m_Rasterizer.m_State.SlopeScaledDepthBias = rs.PolygonOffset[0];
		pipe.m_Rasterizer.m_State.OffsetClamp = rs.PolygonOffset[2];
	}
	else
	{
		pipe.m_Rasterizer.m_State.DepthBias = 0.0f;
		pipe.m_Rasterizer.m_State.SlopeScaledDepthBias = 0.0f;
		pipe.m_Rasterizer.m_State.OffsetClamp = 0.0f;
	}

	if (rs.Enabled[GLRenderState::eEnabled_CullFace])
	{
		switch (rs.CullFace)
		{
		default:
			RDCWARN("Unexpected value for CULL_FACE");
		case eGL_BACK:
			pipe.m_Rasterizer.m_State.CullMode = eCull_Back;
			break;
		case eGL_FRONT:
			pipe.m_Rasterizer.m_State.CullMode = eCull_Front;
			break;
		case eGL_FRONT_AND_BACK:
			pipe.m_Rasterizer.m_State.CullMode = eCull_FrontAndBack;
			break;
		}
	}
	else
	{
		pipe.m_Rasterizer.m_State.CullMode = eCull_None;
	}
	
	RDCASSERT(rs.FrontFace == eGL_CCW || rs.FrontFace == eGL_CW);
	pipe.m_Rasterizer.m_State.FrontCCW = rs.FrontFace == eGL_CCW;
	pipe.m_Rasterizer.m_State.DepthClamp = rs.Enabled[GLRenderState::eEnabled_DepthClamp];
	pipe.m_Rasterizer.m_State.MultisampleEnable = rs.Enabled[GLRenderState::eEnabled_Multisample];

	// depth and stencil states

	pipe.m_DepthState.DepthEnable = rs.Enabled[GLRenderState::eEnabled_DepthTest];
	pipe.m_DepthState.DepthWrites = rs.DepthWriteMask != 0;
	pipe.m_DepthState.DepthFunc = ToStr::Get(rs.DepthFunc).substr(3);

	pipe.m_DepthState.DepthBounds = rs.Enabled[GLRenderState::eEnabled_DepthBoundsEXT];
	pipe.m_DepthState.NearBound = rs.DepthBounds.nearZ;
	pipe.m_DepthState.FarBound = rs.DepthBounds.farZ;

	pipe.m_StencilState.StencilEnable = rs.Enabled[GLRenderState::eEnabled_StencilTest];
	pipe.m_StencilState.m_FrontFace.ValueMask = rs.StencilFront.valuemask;
	pipe.m_StencilState.m_FrontFace.WriteMask = rs.StencilFront.writemask;
	pipe.m_StencilState.m_FrontFace.Ref = rs.StencilFront.ref;
	pipe.m_StencilState.m_FrontFace.Func = ToStr::Get(rs.StencilFront.func).substr(3);
	pipe.m_StencilState.m_FrontFace.PassOp = ToStr::Get(rs.StencilFront.pass).substr(3);
	pipe.m_StencilState.m_FrontFace.FailOp = ToStr::Get(rs.StencilFront.stencilFail).substr(3);
	pipe.m_StencilState.m_FrontFace.DepthFailOp = ToStr::Get(rs.StencilFront.depthFail).substr(3);
	pipe.m_StencilState.m_BackFace.ValueMask = rs.StencilBack.valuemask;
	pipe.m_StencilState.m_BackFace.WriteMask = rs.StencilBack.writemask;
	pipe.m_StencilState.m_BackFace.Ref = rs.StencilBack.ref;
	pipe.m_StencilState.m_BackFace.Func = ToStr::Get(rs.StencilBack.func).substr(3);
	pipe.m_StencilState.m_BackFace.PassOp = ToStr::Get(rs.StencilBack.pass).substr(3);
	pipe.m_StencilState.m_BackFace.FailOp = ToStr::Get(rs.StencilBack.stencilFail).substr(3);
	pipe.m_StencilState.m_BackFace.DepthFailOp = ToStr::Get(rs.StencilBack.depthFail).substr(3);

	// Frame buffer

	GLuint curFBO = 0;
	gl.glGetIntegerv(eGL_DRAW_FRAMEBUFFER_BINDING, (GLint*)&curFBO);
	
	GLint numCols = 8;
	gl.glGetIntegerv(eGL_MAX_COLOR_ATTACHMENTS, &numCols);

	bool rbCol[32] = { false };
	bool rbDepth = false;
	bool rbStencil = false;
	GLuint curCol[32] = { 0 };
	GLuint curDepth = 0;
	GLuint curStencil = 0;

	RDCASSERT(numCols <= 32);

	// we should never bind the true default framebuffer - if the app did, we will have our fake bound
	RDCASSERT(curFBO != 0);

	{
		GLenum type = eGL_TEXTURE;
		for(GLint i=0; i < numCols; i++)
		{
			gl.glGetFramebufferAttachmentParameteriv(eGL_DRAW_FRAMEBUFFER, GLenum(eGL_COLOR_ATTACHMENT0+i), eGL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, (GLint*)&curCol[i]);
			gl.glGetFramebufferAttachmentParameteriv(eGL_DRAW_FRAMEBUFFER, GLenum(eGL_COLOR_ATTACHMENT0+i), eGL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, (GLint*)&type);
			if(type == eGL_RENDERBUFFER) rbCol[i] = true;
		}

		gl.glGetFramebufferAttachmentParameteriv(eGL_DRAW_FRAMEBUFFER, eGL_DEPTH_ATTACHMENT, eGL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, (GLint*)&curDepth);
		gl.glGetFramebufferAttachmentParameteriv(eGL_DRAW_FRAMEBUFFER, eGL_DEPTH_ATTACHMENT, eGL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, (GLint*)&type);
		if(type == eGL_RENDERBUFFER) rbDepth = true;
		gl.glGetFramebufferAttachmentParameteriv(eGL_DRAW_FRAMEBUFFER, eGL_STENCIL_ATTACHMENT, eGL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, (GLint*)&curStencil);
		gl.glGetFramebufferAttachmentParameteriv(eGL_DRAW_FRAMEBUFFER, eGL_STENCIL_ATTACHMENT, eGL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, (GLint*)&type);
		if(type == eGL_RENDERBUFFER) rbStencil = true;
	}

	pipe.m_FB.FBO = rm->GetOriginalID(rm->GetID(FramebufferRes(ctx, curFBO)));
	create_array_uninit(pipe.m_FB.Color, numCols);
	for(GLint i=0; i < numCols; i++)
		pipe.m_FB.Color[i] = rm->GetOriginalID(rm->GetID(rbCol[i] ? RenderbufferRes(ctx, curCol[i]) : TextureRes(ctx, curCol[i])));

	pipe.m_FB.Depth = rm->GetOriginalID(rm->GetID(rbDepth ? RenderbufferRes(ctx, curDepth) : TextureRes(ctx, curDepth)));
	pipe.m_FB.Stencil = rm->GetOriginalID(rm->GetID(rbStencil ? RenderbufferRes(ctx, curStencil) : TextureRes(ctx, curStencil)));

	memcpy(pipe.m_FB.m_Blending.BlendFactor, rs.BlendColor, sizeof(rs.BlendColor));

	pipe.m_FB.FramebufferSRGB = rs.Enabled[GLRenderState::eEnabled_FramebufferSRGB];

	RDCCOMPILE_ASSERT(ARRAY_COUNT(rs.Blends) == ARRAY_COUNT(rs.ColorMasks), "Color masks and blends mismatched");
	create_array_uninit(pipe.m_FB.m_Blending.Blends, ARRAY_COUNT(rs.Blends));
	for(size_t i=0; i < ARRAY_COUNT(rs.Blends); i++)
	{
		pipe.m_FB.m_Blending.Blends[i].Enabled = rs.Blends[i].Enabled;
		pipe.m_FB.m_Blending.Blends[i].LogicOp = "";
		if(rs.LogicOp != eGL_NONE && rs.LogicOp != eGL_COPY && rs.Enabled[GLRenderState::eEnabled_ColorLogicOp])
			pipe.m_FB.m_Blending.Blends[i].LogicOp = ToStr::Get(rs.LogicOp).substr(3); // 3 == strlen("GL_")

		pipe.m_FB.m_Blending.Blends[i].m_Blend.Source = BlendString(rs.Blends[i].SourceRGB);
		pipe.m_FB.m_Blending.Blends[i].m_Blend.Destination = BlendString(rs.Blends[i].DestinationRGB);
		pipe.m_FB.m_Blending.Blends[i].m_Blend.Operation = BlendString(rs.Blends[i].EquationRGB);

		pipe.m_FB.m_Blending.Blends[i].m_AlphaBlend.Source = BlendString(rs.Blends[i].SourceAlpha);
		pipe.m_FB.m_Blending.Blends[i].m_AlphaBlend.Destination = BlendString(rs.Blends[i].DestinationAlpha);
		pipe.m_FB.m_Blending.Blends[i].m_AlphaBlend.Operation = BlendString(rs.Blends[i].EquationAlpha);

		pipe.m_FB.m_Blending.Blends[i].WriteMask = 0;
		if(rs.ColorMasks[i].red)   pipe.m_FB.m_Blending.Blends[i].WriteMask |= 1;
		if(rs.ColorMasks[i].green) pipe.m_FB.m_Blending.Blends[i].WriteMask |= 2;
		if(rs.ColorMasks[i].blue)  pipe.m_FB.m_Blending.Blends[i].WriteMask |= 4;
		if(rs.ColorMasks[i].alpha) pipe.m_FB.m_Blending.Blends[i].WriteMask |= 8;
	}
}

void GLReplay::FillCBufferValue(WrappedOpenGL &gl, GLuint prog, bool bufferBacked, bool rowMajor,
								uint32_t offs, uint32_t matStride, const vector<byte> &data, ShaderVariable &outVar)
{
	const byte *bufdata = data.empty() ? NULL : &data[offs];
	size_t datasize = data.size() - offs;
	if(offs > data.size()) datasize = 0;

	if(bufferBacked)
	{
		size_t rangelen = outVar.rows*outVar.columns*sizeof(float);

		if(outVar.rows > 1 && outVar.columns > 1)
		{
			uint32_t *dest = &outVar.value.uv[0];

			uint32_t majorsize = outVar.columns;
			uint32_t minorsize = outVar.rows;

			if(rowMajor)
			{
				majorsize = outVar.rows;
				minorsize = outVar.columns;
			}

			for(uint32_t c=0; c < majorsize; c++)
			{
				if(datasize > 0)
					memcpy((byte *)dest, bufdata, RDCMIN(rangelen, minorsize*sizeof(float)));

				datasize -= RDCMIN(datasize, (size_t)matStride);
				bufdata += matStride;
				dest += minorsize;
			}
		}
		else
		{
			if(datasize > 0)
				memcpy(&outVar.value.uv[0], bufdata, RDCMIN(rangelen, datasize));
		}
	}
	else
	{
		switch(outVar.type)
		{
			case eVar_Float:
				gl.glGetUniformfv(prog, offs, outVar.value.fv);
				break;
			case eVar_Int:
				gl.glGetUniformiv(prog, offs, outVar.value.iv);
				break;
			case eVar_UInt:
				gl.glGetUniformuiv(prog, offs, outVar.value.uv);
				break;
			case eVar_Double:
				gl.glGetUniformdv(prog, offs, outVar.value.dv);
				break;
		}
	}

	if(!rowMajor)
	{
		if(outVar.type != eVar_Double)
		{
			uint32_t uv[16];
			memcpy(&uv[0], &outVar.value.uv[0], sizeof(uv));

			for(uint32_t r=0; r < outVar.rows; r++)
				for(uint32_t c=0; c < outVar.columns; c++)
					outVar.value.uv[r*outVar.columns+c] = uv[c*outVar.rows+r];
		}
		else
		{
			double dv[16];
			memcpy(&dv[0], &outVar.value.dv[0], sizeof(dv));

			for(uint32_t r=0; r < outVar.rows; r++)
				for(uint32_t c=0; c < outVar.columns; c++)
					outVar.value.dv[r*outVar.columns+c] = dv[c*outVar.rows+r];
		}
	}
}

void GLReplay::FillCBufferVariables(WrappedOpenGL &gl, GLuint prog, bool bufferBacked, string prefix,
                                    const rdctype::array<ShaderConstant> &variables, vector<ShaderVariable> &outvars,
                                    const vector<byte> &data)
{
	for(int32_t i=0; i < variables.count; i++)
	{
		auto desc = variables[i].type.descriptor;

		ShaderVariable var;
		var.name = variables[i].name.elems;
		var.rows = desc.rows;
		var.columns = desc.cols;
		var.type = desc.type;

		if(variables[i].type.members.count > 0)
		{
			if(desc.elements == 0)
			{
				vector<ShaderVariable> ov;
				FillCBufferVariables(gl, prog, bufferBacked, prefix + var.name.elems + ".", variables[i].type.members, ov, data);
				var.isStruct = true;
				var.members = ov;
			}
			else
			{
				vector<ShaderVariable> arrelems;
				for(uint32_t a=0; a < desc.elements; a++)
				{
					ShaderVariable arrEl = var;
					arrEl.name = StringFormat::Fmt("%s[%u]", var.name.elems, a);
					
					vector<ShaderVariable> ov;
					FillCBufferVariables(gl, prog, bufferBacked, prefix + arrEl.name.elems + ".", variables[i].type.members, ov, data);
					arrEl.members = ov;

					arrEl.isStruct = true;
					
					arrelems.push_back(arrEl);
				}
				var.members = arrelems;
				var.isStruct = false;
				var.rows = var.columns = 0;
			}
		}
		else
		{
			RDCEraseEl(var.value);
			
			// need to query offset and strides as there's no way to know what layout was used
			// (and if it's not an std layout it's implementation defined :( )
			string fullname = prefix + var.name.elems;

			GLuint idx = gl.glGetProgramResourceIndex(prog, eGL_UNIFORM, fullname.c_str());

			if(idx == GL_INVALID_INDEX)
			{
				RDCERR("Can't find program resource index for %s", fullname.c_str());
			}
			else
			{
				GLenum props[] = { eGL_OFFSET, eGL_MATRIX_STRIDE, eGL_ARRAY_STRIDE, eGL_LOCATION };
				GLint values[] = { 0, 0, 0, 0 };

				gl.glGetProgramResourceiv(prog, eGL_UNIFORM, idx, ARRAY_COUNT(props), props, ARRAY_COUNT(props), NULL, values);

				if(!bufferBacked)
				{
					values[0] = values[3];
					values[2] = 1;
				}

				if(desc.elements == 0)
				{
					FillCBufferValue(gl, prog, bufferBacked, desc.rowMajorStorage ? true : false,
						values[0], values[1], data, var);
				}
				else
				{
					vector<ShaderVariable> elems;
					for(uint32_t a=0; a < desc.elements; a++)
					{
						ShaderVariable el = var;
						el.name = StringFormat::Fmt("%s[%u]", var.name.elems, a);

						FillCBufferValue(gl, prog, bufferBacked, desc.rowMajorStorage ? true : false,
							values[0] + values[2] * a, values[1], data, el);

						el.isStruct = false;

						elems.push_back(el);
					}

					var.members = elems;
					var.isStruct = false;
					var.rows = var.columns = 0;
				}
			}
		}

		outvars.push_back(var);
	}
}

void GLReplay::FillCBufferVariables(ResourceId shader, uint32_t cbufSlot, vector<ShaderVariable> &outvars, const vector<byte> &data)
{
	WrappedOpenGL &gl = *m_pDriver;
	
	MakeCurrentReplayContext(&m_ReplayCtx);

	auto &shaderDetails = m_pDriver->m_Shaders[shader];

	if((int32_t)cbufSlot >= shaderDetails.reflection.ConstantBlocks.count)
	{
		RDCERR("Requesting invalid constant block");
		return;
	}
	
	GLuint curProg = 0;
	gl.glGetIntegerv(eGL_CURRENT_PROGRAM, (GLint*)&curProg);

	if(curProg == 0)
	{
		gl.glGetIntegerv(eGL_PROGRAM_PIPELINE_BINDING, (GLint*)&curProg);
	
		if(curProg == 0)
		{
			RDCERR("No program or pipeline bound");
			return;
		}
		else
		{
			ResourceId id = m_pDriver->GetResourceManager()->GetID(ProgramPipeRes(m_ReplayCtx.ctx, curProg));
			auto &pipeDetails = m_pDriver->m_Pipelines[id];

			size_t s = ShaderIdx(shaderDetails.type);

			curProg = m_pDriver->GetResourceManager()->GetCurrentResource(pipeDetails.stagePrograms[s]).name;
		}
	}

	auto cblock = shaderDetails.reflection.ConstantBlocks.elems[cbufSlot];
	
	FillCBufferVariables(gl, curProg, cblock.bufferBacked ? true : false, "", cblock.variables, outvars, data);
}

#pragma endregion

void GLReplay::InitPostVSBuffers(uint32_t frameID, uint32_t eventID)
{
	GLNOTIMP("GLReplay::InitPostVSBuffers");
}

vector<EventUsage> GLReplay::GetUsage(ResourceId id)
{
	GLNOTIMP("GetUsage");
	return vector<EventUsage>();
}

void GLReplay::SetContextFilter(ResourceId id, uint32_t firstDefEv, uint32_t lastDefEv)
{
	RDCUNIMPLEMENTED("SetContextFilter");
}

void GLReplay::FreeTargetResource(ResourceId id)
{
	RDCUNIMPLEMENTED("FreeTargetResource");
}

void GLReplay::FreeCustomShader(ResourceId id)
{
	RDCUNIMPLEMENTED("FreeCustomShader");
}

PostVSMeshData GLReplay::GetPostVSBuffers(uint32_t frameID, uint32_t eventID, MeshDataStage stage)
{
	PostVSMeshData ret;
	RDCEraseEl(ret);

	GLNOTIMP("GLReplay::GetPostVSBuffers");

	return ret;
}

byte *GLReplay::GetTextureData(ResourceId tex, uint32_t arrayIdx, uint32_t mip, bool resolve, bool forceRGBA8unorm, float blackPoint, float whitePoint, size_t &dataSize)
{
	RDCUNIMPLEMENTED("GetTextureData");
	return NULL;
}

void GLReplay::ReplaceResource(ResourceId from, ResourceId to)
{
	RDCUNIMPLEMENTED("ReplaceResource");
}

void GLReplay::RemoveReplacement(ResourceId id)
{
	RDCUNIMPLEMENTED("RemoveReplacement");
}

void GLReplay::TimeDrawcalls(rdctype::array<FetchDrawcall> &arr)
{
	RDCUNIMPLEMENTED("TimeDrawcalls");
}

void GLReplay::BuildTargetShader(string source, string entry, const uint32_t compileFlags, ShaderStageType type, ResourceId *id, string *errors)
{
	RDCUNIMPLEMENTED("BuildTargetShader");
}

void GLReplay::BuildCustomShader(string source, string entry, const uint32_t compileFlags, ShaderStageType type, ResourceId *id, string *errors)
{
	RDCUNIMPLEMENTED("BuildCustomShader");
}

vector<PixelModification> GLReplay::PixelHistory(uint32_t frameID, vector<EventUsage> events, ResourceId target, uint32_t x, uint32_t y, uint32_t sampleIdx)
{
	RDCUNIMPLEMENTED("GLReplay::PixelHistory");
	return vector<PixelModification>();
}

ShaderDebugTrace GLReplay::DebugVertex(uint32_t frameID, uint32_t eventID, uint32_t vertid, uint32_t instid, uint32_t idx, uint32_t instOffset, uint32_t vertOffset)
{
	RDCUNIMPLEMENTED("DebugVertex");
	return ShaderDebugTrace();
}

ShaderDebugTrace GLReplay::DebugPixel(uint32_t frameID, uint32_t eventID, uint32_t x, uint32_t y, uint32_t sample, uint32_t primitive)
{
	RDCUNIMPLEMENTED("DebugPixel");
	return ShaderDebugTrace();
}

ShaderDebugTrace GLReplay::DebugThread(uint32_t frameID, uint32_t eventID, uint32_t groupid[3], uint32_t threadid[3])
{
	RDCUNIMPLEMENTED("DebugThread");
	return ShaderDebugTrace();
}

ResourceId GLReplay::ApplyCustomShader(ResourceId shader, ResourceId texid, uint32_t mip)
{
	RDCUNIMPLEMENTED("ApplyCustomShader");
	return ResourceId();
}

ResourceId GLReplay::CreateProxyTexture( FetchTexture templateTex )
{
	RDCUNIMPLEMENTED("CreateProxyTexture");
	return ResourceId();
}

void GLReplay::SetProxyTextureData(ResourceId texid, uint32_t arrayIdx, uint32_t mip, byte *data, size_t dataSize)
{
	RDCUNIMPLEMENTED("SetProxyTextureData");
}

const GLHookSet &GetRealFunctions();

// defined in gl_replay_<platform>.cpp
ReplayCreateStatus GL_CreateReplayDevice(const char *logfile, IReplayDriver **driver);

static DriverRegistration GLDriverRegistration(RDC_OpenGL, "OpenGL", &GL_CreateReplayDevice);
