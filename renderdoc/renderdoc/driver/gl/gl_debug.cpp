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
#include "maths/matrix.h"
#include "maths/camera.h"
#include "maths/formatpacking.h"

#include "data/glsl/debuguniforms.h"

#include "serialise/string_utils.h"

#include <algorithm>

GLuint GLReplay::CreateCShaderProgram(const char *csSrc)
{
	if(m_pDriver == NULL) return 0;
	
	MakeCurrentReplayContext(m_DebugCtx);
	
	WrappedOpenGL &gl = *m_pDriver;

	GLuint cs = gl.glCreateShader(eGL_COMPUTE_SHADER);

	gl.glShaderSource(cs, 1, &csSrc, NULL);

	gl.glCompileShader(cs);

	char buffer[1024];
	GLint status = 0;

	gl.glGetShaderiv(cs, eGL_COMPILE_STATUS, &status);
	if(status == 0)
	{
		gl.glGetShaderInfoLog(cs, 1024, NULL, buffer);
		RDCERR("Shader error: %s", buffer);
	}

	GLuint ret = gl.glCreateProgram();

	gl.glAttachShader(ret, cs);

	gl.glLinkProgram(ret);
	
	gl.glGetProgramiv(ret, eGL_LINK_STATUS, &status);
	if(status == 0)
	{
		gl.glGetProgramInfoLog(ret, 1024, NULL, buffer);
		RDCERR("Link error: %s", buffer);
	}

	gl.glDetachShader(ret, cs);

	gl.glDeleteShader(cs);

	return ret;
}

GLuint GLReplay::CreateShaderProgram(const char *vsSrc, const char *psSrc, const char *gsSrc)
{
	if(m_pDriver == NULL) return 0;
	
	MakeCurrentReplayContext(m_DebugCtx);
	
	WrappedOpenGL &gl = *m_pDriver;

	GLuint vs = gl.glCreateShader(eGL_VERTEX_SHADER);
	GLuint fs = gl.glCreateShader(eGL_FRAGMENT_SHADER);
	GLuint gs = 0;

	const char *src = vsSrc;
	gl.glShaderSource(vs, 1, &src, NULL);
	src = psSrc;
	gl.glShaderSource(fs, 1, &src, NULL);

	if(gsSrc)
	{
		gs = gl.glCreateShader(eGL_GEOMETRY_SHADER);
		src = gsSrc;
		gl.glShaderSource(gs, 1, &src, NULL);
	}

	gl.glCompileShader(vs);
	gl.glCompileShader(fs);
	if(gs) gl.glCompileShader(gs);

	char buffer[1024];
	GLint status = 0;

	gl.glGetShaderiv(vs, eGL_COMPILE_STATUS, &status);
	if(status == 0)
	{
		gl.glGetShaderInfoLog(vs, 1024, NULL, buffer);
		RDCERR("Shader error: %s", buffer);
	}

	gl.glGetShaderiv(fs, eGL_COMPILE_STATUS, &status);
	if(status == 0)
	{
		gl.glGetShaderInfoLog(fs, 1024, NULL, buffer);
		RDCERR("Shader error: %s", buffer);
	}

	if(gs)
	{
		gl.glGetShaderiv(gs, eGL_COMPILE_STATUS, &status);
		if(status == 0)
		{
			gl.glGetShaderInfoLog(gs, 1024, NULL, buffer);
			RDCERR("Shader error: %s", buffer);
		}
	}

	GLuint ret = gl.glCreateProgram();

	gl.glAttachShader(ret, vs);
	gl.glAttachShader(ret, fs);
	if(gs) gl.glAttachShader(ret, gs);

	gl.glLinkProgram(ret);

	gl.glDetachShader(ret, vs);
	gl.glDetachShader(ret, fs);
	if(gs) gl.glDetachShader(ret, gs);

	gl.glDeleteShader(vs);
	gl.glDeleteShader(fs);
	if(gs) gl.glDeleteShader(gs);

	return ret;
}

void GLReplay::InitDebugData()
{
	if(m_pDriver == NULL) return;
	
	{
		uint64_t id = MakeOutputWindow(NULL, true);

		m_DebugID = id;
		m_DebugCtx = &m_OutputWindows[id];

		MakeCurrentReplayContext(m_DebugCtx);
	}

	DebugData.outWidth = 0.0f; DebugData.outHeight = 0.0f;
	
	DebugData.blitvsSource = GetEmbeddedResource(blit_vert);
	DebugData.blitfsSource = GetEmbeddedResource(blit_frag);

	DebugData.blitProg = CreateShaderProgram(DebugData.blitvsSource.c_str(), DebugData.blitfsSource.c_str());
	
	string glslheader = GetEmbeddedResource(debuguniforms_h);

	string texfs = GetEmbeddedResource(texsample_h);
	texfs += GetEmbeddedResource(texdisplay_frag);

	for(int i=0; i < 3; i++)
	{
		string glsl = glslheader;
		glsl += string("#define UINT_TEX ") + (i == 1 ? "1" : "0") + "\n";
		glsl += string("#define SINT_TEX ") + (i == 2 ? "1" : "0") + "\n";
		glsl += texfs;

		DebugData.texDisplayProg[i] = CreateShaderProgram(DebugData.blitvsSource.c_str(), glsl.c_str());
	}

	string checkerfs = GetEmbeddedResource(checkerboard_frag);
	
	DebugData.checkerProg = CreateShaderProgram(DebugData.blitvsSource.c_str(), checkerfs.c_str());

	DebugData.genericvsSource = GetEmbeddedResource(generic_vert);
	DebugData.genericfsSource = GetEmbeddedResource(generic_frag);

	DebugData.genericProg = CreateShaderProgram(DebugData.genericvsSource.c_str(), DebugData.genericfsSource.c_str());
	
	string meshvs = GetEmbeddedResource(mesh_vert);
	string meshgs = GetEmbeddedResource(mesh_geom);
	string meshfs = GetEmbeddedResource(mesh_frag);
	meshfs = glslheader + meshfs;
	
	DebugData.meshProg = CreateShaderProgram(meshvs.c_str(), meshfs.c_str());
	DebugData.meshgsProg = CreateShaderProgram(meshvs.c_str(), meshfs.c_str(), meshgs.c_str());
	
	WrappedOpenGL &gl = *m_pDriver;

	{
		float data[] = {
			0.0f, -1.0f, 0.0f, 1.0f,
			1.0f, -1.0f, 0.0f, 1.0f,
			1.0f,  0.0f, 0.0f, 1.0f,
			0.0f,  0.0f, 0.0f, 1.0f,
		};

		gl.glGenBuffers(1, &DebugData.outlineStripVB);
		gl.glBindBuffer(eGL_ARRAY_BUFFER, DebugData.outlineStripVB);
		gl.glNamedBufferDataEXT(DebugData.outlineStripVB, sizeof(data), data, eGL_STATIC_DRAW);
		
    gl.glGenVertexArrays(1, &DebugData.outlineStripVAO);
    gl.glBindVertexArray(DebugData.outlineStripVAO);
		
		gl.glVertexAttribPointer(0, 4, eGL_FLOAT, false, 0, (const void *)0);
		gl.glEnableVertexAttribArray(0);
	}

	gl.glGenSamplers(1, &DebugData.linearSampler);
	gl.glSamplerParameteri(DebugData.linearSampler, eGL_TEXTURE_MIN_FILTER, eGL_LINEAR);
	gl.glSamplerParameteri(DebugData.linearSampler, eGL_TEXTURE_MAG_FILTER, eGL_LINEAR);
	gl.glSamplerParameteri(DebugData.linearSampler, eGL_TEXTURE_WRAP_S, eGL_CLAMP_TO_EDGE);
	gl.glSamplerParameteri(DebugData.linearSampler, eGL_TEXTURE_WRAP_T, eGL_CLAMP_TO_EDGE);
	
	gl.glGenSamplers(1, &DebugData.pointSampler);
	gl.glSamplerParameteri(DebugData.pointSampler, eGL_TEXTURE_MIN_FILTER, eGL_NEAREST_MIPMAP_NEAREST);
	gl.glSamplerParameteri(DebugData.pointSampler, eGL_TEXTURE_MAG_FILTER, eGL_NEAREST);
	gl.glSamplerParameteri(DebugData.pointSampler, eGL_TEXTURE_WRAP_S, eGL_CLAMP_TO_EDGE);
	gl.glSamplerParameteri(DebugData.pointSampler, eGL_TEXTURE_WRAP_T, eGL_CLAMP_TO_EDGE);
	
	gl.glGenSamplers(1, &DebugData.pointNoMipSampler);
	gl.glSamplerParameteri(DebugData.pointNoMipSampler, eGL_TEXTURE_MIN_FILTER, eGL_NEAREST);
	gl.glSamplerParameteri(DebugData.pointNoMipSampler, eGL_TEXTURE_MAG_FILTER, eGL_NEAREST);
	gl.glSamplerParameteri(DebugData.pointNoMipSampler, eGL_TEXTURE_WRAP_S, eGL_CLAMP_TO_EDGE);
	gl.glSamplerParameteri(DebugData.pointNoMipSampler, eGL_TEXTURE_WRAP_T, eGL_CLAMP_TO_EDGE);
	
	gl.glGenBuffers(ARRAY_COUNT(DebugData.UBOs), DebugData.UBOs);
	for(size_t i=0; i < ARRAY_COUNT(DebugData.UBOs); i++)
	{
		gl.glBindBuffer(eGL_UNIFORM_BUFFER, DebugData.UBOs[i]);
		gl.glNamedBufferDataEXT(DebugData.UBOs[i], 512, NULL, eGL_DYNAMIC_DRAW);
		RDCCOMPILE_ASSERT(sizeof(texdisplay) < 512, "texdisplay UBO too large");
		RDCCOMPILE_ASSERT(sizeof(FontUniforms) < 512, "texdisplay UBO too large");
		RDCCOMPILE_ASSERT(sizeof(HistogramCBufferData) < 512, "texdisplay UBO too large");
	}

	DebugData.overlayTexWidth = DebugData.overlayTexHeight = 0;
	DebugData.overlayTex = DebugData.overlayFBO = 0;
	
	gl.glGenFramebuffers(1, &DebugData.pickPixelFBO);
	gl.glBindFramebuffer(eGL_FRAMEBUFFER, DebugData.pickPixelFBO);

	gl.glGenTextures(1, &DebugData.pickPixelTex);
	gl.glBindTexture(eGL_TEXTURE_2D, DebugData.pickPixelTex);
	
	gl.glTexStorage2D(eGL_TEXTURE_2D, 1, eGL_RGBA32F, 1, 1); 
	gl.glTexParameteri(eGL_TEXTURE_2D, eGL_TEXTURE_MIN_FILTER, eGL_NEAREST);
	gl.glTexParameteri(eGL_TEXTURE_2D, eGL_TEXTURE_MAG_FILTER, eGL_NEAREST);
	gl.glTexParameteri(eGL_TEXTURE_2D, eGL_TEXTURE_WRAP_S, eGL_CLAMP_TO_EDGE);
	gl.glTexParameteri(eGL_TEXTURE_2D, eGL_TEXTURE_WRAP_T, eGL_CLAMP_TO_EDGE);
	gl.glFramebufferTexture(eGL_FRAMEBUFFER, eGL_COLOR_ATTACHMENT0, DebugData.pickPixelTex, 0);

	gl.glGenVertexArrays(1, &DebugData.emptyVAO);
	gl.glBindVertexArray(DebugData.emptyVAO);
	
	// histogram/minmax data
	{
		string histogramglsl = GetEmbeddedResource(texsample_h);
		histogramglsl += GetEmbeddedResource(histogram_comp);

		RDCEraseEl(DebugData.minmaxTileProgram);
		RDCEraseEl(DebugData.histogramProgram);
		RDCEraseEl(DebugData.minmaxResultProgram);

		RDCCOMPILE_ASSERT(ARRAY_COUNT(DebugData.minmaxTileProgram) >= (TEXDISPLAY_SINT_TEX|TEXDISPLAY_TYPEMASK)+1, "not enough programs");

		for(int t=1; t <= RESTYPE_TEXTYPEMAX; t++)
		{
			// float, uint, sint
			for(int i=0; i < 3; i++)
			{
				int idx = t;
				if(i == 1) idx |= TEXDISPLAY_UINT_TEX;
				if(i == 2) idx |= TEXDISPLAY_SINT_TEX;

				{
					string glsl = glslheader;
					glsl += string("#define SHADER_RESTYPE ") + ToStr::Get(t) + "\n";
					glsl += string("#define UINT_TEX ") + (i == 1 ? "1" : "0") + "\n";
					glsl += string("#define SINT_TEX ") + (i == 2 ? "1" : "0") + "\n";
					glsl += string("#define RENDERDOC_TileMinMaxCS 1\n");
					glsl += histogramglsl;

					DebugData.minmaxTileProgram[idx] = CreateCShaderProgram(glsl.c_str());
				}

				{
					string glsl = glslheader;
					glsl += string("#define SHADER_RESTYPE ") + ToStr::Get(t) + "\n";
					glsl += string("#define UINT_TEX ") + (i == 1 ? "1" : "0") + "\n";
					glsl += string("#define SINT_TEX ") + (i == 2 ? "1" : "0") + "\n";
					glsl += string("#define RENDERDOC_HistogramCS 1\n");
					glsl += histogramglsl;

					DebugData.histogramProgram[idx] = CreateCShaderProgram(glsl.c_str());
				}

				if(t == 1)
				{
					string glsl = glslheader;
					glsl += string("#define SHADER_RESTYPE ") + ToStr::Get(t) + "\n";
					glsl += string("#define UINT_TEX ") + (i == 1 ? "1" : "0") + "\n";
					glsl += string("#define SINT_TEX ") + (i == 2 ? "1" : "0") + "\n";
					glsl += string("#define RENDERDOC_ResultMinMaxCS 1\n");
					glsl += histogramglsl;

					DebugData.minmaxResultProgram[i] = CreateCShaderProgram(glsl.c_str());
				}
			}
		}

		gl.glGenBuffers(1, &DebugData.minmaxTileResult);
		gl.glGenBuffers(1, &DebugData.minmaxResult);
		gl.glGenBuffers(1, &DebugData.histogramBuf);
		
		const uint32_t maxTexDim = 16384;
		const uint32_t blockPixSize = HGRAM_PIXELS_PER_TILE*HGRAM_TILES_PER_BLOCK;
		const uint32_t maxBlocksNeeded = (maxTexDim*maxTexDim)/(blockPixSize*blockPixSize);

		const size_t byteSize = 2*sizeof(Vec4f)*HGRAM_TILES_PER_BLOCK*HGRAM_TILES_PER_BLOCK*maxBlocksNeeded;

		gl.glNamedBufferStorageEXT(DebugData.minmaxTileResult, byteSize, NULL, 0);
		gl.glNamedBufferStorageEXT(DebugData.minmaxResult, sizeof(Vec4f)*2, NULL, GL_MAP_READ_BIT);
		gl.glNamedBufferStorageEXT(DebugData.histogramBuf, sizeof(uint32_t)*HGRAM_NUM_BUCKETS, NULL, GL_MAP_READ_BIT);
	}

	gl.glGenVertexArrays(1, &DebugData.meshVAO);
	gl.glBindVertexArray(DebugData.meshVAO);
	
	gl.glGenBuffers(1, &DebugData.axisFrustumBuffer);
	gl.glBindBuffer(eGL_ARRAY_BUFFER, DebugData.axisFrustumBuffer);
	
	Vec3f TLN = Vec3f(-1.0f,  1.0f, 0.0f); // TopLeftNear, etc...
	Vec3f TRN = Vec3f( 1.0f,  1.0f, 0.0f);
	Vec3f BLN = Vec3f(-1.0f, -1.0f, 0.0f);
	Vec3f BRN = Vec3f( 1.0f, -1.0f, 0.0f);

	Vec3f TLF = Vec3f(-1.0f,  1.0f, 1.0f);
	Vec3f TRF = Vec3f( 1.0f,  1.0f, 1.0f);
	Vec3f BLF = Vec3f(-1.0f, -1.0f, 1.0f);
	Vec3f BRF = Vec3f( 1.0f, -1.0f, 1.0f);

	Vec3f axisFrustum[] = {
		// axis marker vertices
		Vec3f(0.0f, 0.0f, 0.0f),
		Vec3f(1.0f, 0.0f, 0.0f),
		Vec3f(0.0f, 0.0f, 0.0f),
		Vec3f(0.0f, 1.0f, 0.0f),
		Vec3f(0.0f, 0.0f, 0.0f),
		Vec3f(0.0f, 0.0f, 1.0f),

		// frustum vertices
		TLN, TRN,
		TRN, BRN,
		BRN, BLN,
		BLN, TLN,

		TLN, TLF,
		TRN, TRF,
		BLN, BLF,
		BRN, BRF,

		TLF, TRF,
		TRF, BRF,
		BRF, BLF,
		BLF, TLF,
	};

	gl.glNamedBufferStorageEXT(DebugData.axisFrustumBuffer, sizeof(axisFrustum), axisFrustum, 0);
	
	gl.glGenVertexArrays(1, &DebugData.axisVAO);
	gl.glBindVertexArray(DebugData.axisVAO);
	gl.glVertexAttribPointer(0, 3, eGL_FLOAT, GL_FALSE, sizeof(Vec3f), NULL);
	gl.glEnableVertexAttribArray(0);
	
	gl.glGenVertexArrays(1, &DebugData.frustumVAO);
	gl.glBindVertexArray(DebugData.frustumVAO);
	gl.glVertexAttribPointer(0, 3, eGL_FLOAT, GL_FALSE, sizeof(Vec3f), (const void *)( sizeof(Vec3f) * 6 ));
	gl.glEnableVertexAttribArray(0);
	
	gl.glGenVertexArrays(1, &DebugData.triHighlightVAO);
	gl.glBindVertexArray(DebugData.triHighlightVAO);
	
	gl.glGenBuffers(1, &DebugData.triHighlightBuffer);
	gl.glBindBuffer(eGL_ARRAY_BUFFER, DebugData.triHighlightBuffer);
	
	gl.glNamedBufferStorageEXT(DebugData.triHighlightBuffer, sizeof(Vec4f)*16, NULL, GL_DYNAMIC_STORAGE_BIT);
	
	gl.glVertexAttribPointer(0, 4, eGL_FLOAT, GL_FALSE, sizeof(Vec4f), NULL);
	gl.glEnableVertexAttribArray(0);
	
	DebugData.replayQuadProg = CreateShaderProgram(DebugData.blitvsSource.c_str(), DebugData.genericfsSource.c_str());

	MakeCurrentReplayContext(&m_ReplayCtx);

	gl.glGenTransformFeedbacks(1, &DebugData.feedbackObj);
	gl.glGenBuffers(1, &DebugData.feedbackBuffer);
	gl.glGenQueries(1, &DebugData.feedbackQuery);

	gl.glBindTransformFeedback(eGL_TRANSFORM_FEEDBACK, DebugData.feedbackObj);
	gl.glBindBuffer(eGL_TRANSFORM_FEEDBACK_BUFFER, DebugData.feedbackBuffer);
	gl.glNamedBufferStorageEXT(DebugData.feedbackBuffer, 32*1024*1024, NULL, GL_MAP_READ_BIT);
	gl.glBindBufferBase(eGL_TRANSFORM_FEEDBACK_BUFFER, 0, DebugData.feedbackBuffer);
	gl.glBindTransformFeedback(eGL_TRANSFORM_FEEDBACK, 0);
}

void GLReplay::DeleteDebugData()
{
	MakeCurrentReplayContext(m_DebugCtx);

	WrappedOpenGL &gl = *m_pDriver;

	for(auto it=m_PostVSData.begin(); it != m_PostVSData.end(); ++it)
	{
		gl.glDeleteBuffers(1, &it->second.vsout.buf);
		gl.glDeleteBuffers(1, &it->second.vsout.idxBuf);
		gl.glDeleteBuffers(1, &it->second.gsout.buf);
		gl.glDeleteBuffers(1, &it->second.gsout.idxBuf);
	}

	m_PostVSData.clear();

	gl.glDeleteProgram(DebugData.blitProg);

	for(int i=0; i < 3; i++)
		gl.glDeleteProgram(DebugData.texDisplayProg[i]);

	gl.glDeleteProgram(DebugData.checkerProg);
	gl.glDeleteProgram(DebugData.genericProg);
	gl.glDeleteProgram(DebugData.meshProg);
	gl.glDeleteProgram(DebugData.meshgsProg);

	gl.glDeleteBuffers(1, &DebugData.outlineStripVB);
	gl.glDeleteVertexArrays(1, &DebugData.outlineStripVAO);

	gl.glDeleteSamplers(1, &DebugData.linearSampler);
	gl.glDeleteSamplers(1, &DebugData.pointSampler);
	gl.glDeleteSamplers(1, &DebugData.pointNoMipSampler);
	gl.glDeleteBuffers(ARRAY_COUNT(DebugData.UBOs), DebugData.UBOs);
	gl.glDeleteFramebuffers(1, &DebugData.pickPixelFBO);
	gl.glDeleteTextures(1, &DebugData.pickPixelTex);

	gl.glDeleteVertexArrays(1, &DebugData.emptyVAO);

	for(int t=1; t <= RESTYPE_TEXTYPEMAX; t++)
	{
		// float, uint, sint
		for(int i=0; i < 3; i++)
		{
			int idx = t;
			if(i == 1) idx |= TEXDISPLAY_UINT_TEX;
			if(i == 2) idx |= TEXDISPLAY_SINT_TEX;

			gl.glDeleteProgram(DebugData.minmaxTileProgram[idx]);
			gl.glDeleteProgram(DebugData.histogramProgram[idx]);

			if(t == 1)
				gl.glDeleteProgram(DebugData.minmaxResultProgram[i]);
		}
	}

	gl.glDeleteBuffers(1, &DebugData.minmaxTileResult);
	gl.glDeleteBuffers(1, &DebugData.minmaxResult);
	gl.glDeleteBuffers(1, &DebugData.histogramBuf);

	gl.glDeleteTransformFeedbacks(1, &DebugData.feedbackObj);
	gl.glDeleteBuffers(1, &DebugData.feedbackBuffer);
	gl.glDeleteQueries(1, &DebugData.feedbackQuery);

	gl.glDeleteVertexArrays(1, &DebugData.meshVAO);
	gl.glDeleteVertexArrays(1, &DebugData.axisVAO);
	gl.glDeleteVertexArrays(1, &DebugData.frustumVAO);
	gl.glDeleteVertexArrays(1, &DebugData.triHighlightVAO);

	gl.glDeleteBuffers(1, &DebugData.axisFrustumBuffer);
	gl.glDeleteBuffers(1, &DebugData.triHighlightBuffer);

	gl.glDeleteProgram(DebugData.replayQuadProg);
}

bool GLReplay::GetMinMax(ResourceId texid, uint32_t sliceFace, uint32_t mip, uint32_t sample, float *minval, float *maxval)
{
	if(m_pDriver->m_Textures.find(texid) == m_pDriver->m_Textures.end())
		return false;
	
	auto &texDetails = m_pDriver->m_Textures[texid];

	FetchTexture details = GetTexture(texid);

	const GLHookSet &gl = m_pDriver->GetHookset();
	
	int texSlot = 0;
	int intIdx = 0;

	bool renderbuffer = false;
	
	switch (texDetails.curType)
	{
		case eGL_RENDERBUFFER:
			texSlot = RESTYPE_TEX2D;
			renderbuffer = true;
			break;
		case eGL_TEXTURE_1D:
			texSlot = RESTYPE_TEX1D;
			break;
		default:
			RDCWARN("Unexpected texture type");
		case eGL_TEXTURE_2D:
			texSlot = RESTYPE_TEX2D;
			break;
		case eGL_TEXTURE_2D_MULTISAMPLE:
			texSlot = RESTYPE_TEX2DMS;
			break;
		case eGL_TEXTURE_RECTANGLE:
			texSlot = RESTYPE_TEXRECT;
			break;
		case eGL_TEXTURE_BUFFER:
			texSlot = RESTYPE_TEXBUFFER;
			break;
		case eGL_TEXTURE_3D:
			texSlot = RESTYPE_TEX3D;
			break;
		case eGL_TEXTURE_CUBE_MAP:
			texSlot = RESTYPE_TEXCUBE;
			break;
		case eGL_TEXTURE_1D_ARRAY:
			texSlot = RESTYPE_TEX1DARRAY;
			break;
		case eGL_TEXTURE_2D_ARRAY:
			texSlot = RESTYPE_TEX2DARRAY;
			break;
		case eGL_TEXTURE_CUBE_MAP_ARRAY:
			texSlot = RESTYPE_TEXCUBEARRAY;
			break;
	}

	GLenum target = texDetails.curType;
	GLuint texname = texDetails.resource.name;

	// do blit from renderbuffer to texture, then sample from texture
	if(renderbuffer)
	{
		// need replay context active to do blit (as FBOs aren't shared)
		MakeCurrentReplayContext(&m_ReplayCtx);
	
		GLuint curDrawFBO = 0;
		GLuint curReadFBO = 0;
		gl.glGetIntegerv(eGL_DRAW_FRAMEBUFFER_BINDING, (GLint*)&curDrawFBO);
		gl.glGetIntegerv(eGL_READ_FRAMEBUFFER_BINDING, (GLint*)&curReadFBO);
		
		gl.glBindFramebuffer(eGL_DRAW_FRAMEBUFFER, texDetails.renderbufferFBOs[1]);
		gl.glBindFramebuffer(eGL_READ_FRAMEBUFFER, texDetails.renderbufferFBOs[0]);

		gl.glBlitFramebuffer(0, 0, texDetails.width, texDetails.height,
		                     0, 0, texDetails.width, texDetails.height,
												 GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT,
												 eGL_NEAREST);

		gl.glBindFramebuffer(eGL_DRAW_FRAMEBUFFER, curDrawFBO);
		gl.glBindFramebuffer(eGL_READ_FRAMEBUFFER, curReadFBO);

		texname = texDetails.renderbufferReadTex;
		target = eGL_TEXTURE_2D;
	}
	
	MakeCurrentReplayContext(m_DebugCtx);

	gl.glBindBufferBase(eGL_UNIFORM_BUFFER, 0, DebugData.UBOs[0]);
	HistogramCBufferData *cdata = (HistogramCBufferData *)gl.glMapBufferRange(eGL_UNIFORM_BUFFER, 0, sizeof(HistogramCBufferData),
	                                                                          GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	cdata->HistogramTextureResolution.x = (float)RDCMAX(details.width>>mip, 1U);
	cdata->HistogramTextureResolution.y = (float)RDCMAX(details.height>>mip, 1U);
	cdata->HistogramTextureResolution.z = (float)RDCMAX(details.depth>>mip, 1U);
	cdata->HistogramSlice = (float)sliceFace;
	cdata->HistogramMip = (int)mip;
	cdata->HistogramNumSamples = texDetails.samples;
	cdata->HistogramSample = (int)RDCCLAMP(sample, 0U, details.msSamp-1);
	if(sample == ~0U) cdata->HistogramSample = -int(details.msSamp);
	cdata->HistogramMin = 0.0f;
	cdata->HistogramMax = 1.0f;
	cdata->HistogramChannels = 0xf;
	
	int progIdx = texSlot;

	if(details.format.compType == eCompType_UInt)
	{
		progIdx |= TEXDISPLAY_UINT_TEX;
		intIdx = 1;
	}
	if(details.format.compType == eCompType_SInt)
	{
		progIdx |= TEXDISPLAY_SINT_TEX;
		intIdx = 2;
	}
	
	if(details.dimension == 3)
		cdata->HistogramSlice = float(sliceFace)/float(details.depth);
	
	int blocksX = (int)ceil(cdata->HistogramTextureResolution.x/float(HGRAM_PIXELS_PER_TILE*HGRAM_TILES_PER_BLOCK));
	int blocksY = (int)ceil(cdata->HistogramTextureResolution.y/float(HGRAM_PIXELS_PER_TILE*HGRAM_TILES_PER_BLOCK));

	gl.glUnmapBuffer(eGL_UNIFORM_BUFFER);

	gl.glActiveTexture((RDCGLenum)(eGL_TEXTURE0 + texSlot));
	gl.glBindTexture(target, texname);
	if(texSlot == RESTYPE_TEXRECT || texSlot == RESTYPE_TEXBUFFER)
		gl.glBindSampler(texSlot, DebugData.pointNoMipSampler);
	else
		gl.glBindSampler(texSlot, DebugData.pointSampler);
	
	gl.glBindBufferBase(eGL_SHADER_STORAGE_BUFFER, 0, DebugData.minmaxTileResult);

	gl.glUseProgram(DebugData.minmaxTileProgram[progIdx]);
	gl.glDispatchCompute(blocksX, blocksY, 1);

	gl.glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	gl.glBindBufferBase(eGL_SHADER_STORAGE_BUFFER, 0, DebugData.minmaxResult);
	gl.glBindBufferBase(eGL_SHADER_STORAGE_BUFFER, 1, DebugData.minmaxTileResult);
	
	gl.glUseProgram(DebugData.minmaxResultProgram[intIdx]);
	gl.glDispatchCompute(1, 1, 1);

	gl.glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	Vec4f minmax[2];
	gl.glBindBuffer(eGL_COPY_READ_BUFFER, DebugData.minmaxResult);
	gl.glGetBufferSubData(eGL_COPY_READ_BUFFER, 0, sizeof(minmax), minmax);

	minval[0] = minmax[0].x;
	minval[1] = minmax[0].y;
	minval[2] = minmax[0].z;
	minval[3] = minmax[0].w;

	maxval[0] = minmax[1].x;
	maxval[1] = minmax[1].y;
	maxval[2] = minmax[1].z;
	maxval[3] = minmax[1].w;

	return true;
}

bool GLReplay::GetHistogram(ResourceId texid, uint32_t sliceFace, uint32_t mip, uint32_t sample, float minval, float maxval, bool channels[4], vector<uint32_t> &histogram)
{
	if(minval >= maxval) return false;

	if(m_pDriver->m_Textures.find(texid) == m_pDriver->m_Textures.end())
		return false;

	auto &texDetails = m_pDriver->m_Textures[texid];

	FetchTexture details = GetTexture(texid);

	const GLHookSet &gl = m_pDriver->GetHookset();

	int texSlot = 0;
	int intIdx = 0;

	bool renderbuffer = false;

	switch (texDetails.curType)
	{
		case eGL_RENDERBUFFER:
			texSlot = RESTYPE_TEX2D;
			renderbuffer = true;
			break;
		case eGL_TEXTURE_1D:
			texSlot = RESTYPE_TEX1D;
			break;
		default:
			RDCWARN("Unexpected texture type");
		case eGL_TEXTURE_2D:
			texSlot = RESTYPE_TEX2D;
			break;
		case eGL_TEXTURE_2D_MULTISAMPLE:
			texSlot = RESTYPE_TEX2DMS;
			break;
		case eGL_TEXTURE_RECTANGLE:
			texSlot = RESTYPE_TEXRECT;
			break;
		case eGL_TEXTURE_BUFFER:
			texSlot = RESTYPE_TEXBUFFER;
			break;
		case eGL_TEXTURE_3D:
			texSlot = RESTYPE_TEX3D;
			break;
		case eGL_TEXTURE_CUBE_MAP:
			texSlot = RESTYPE_TEXCUBE;
			break;
		case eGL_TEXTURE_1D_ARRAY:
			texSlot = RESTYPE_TEX1DARRAY;
			break;
		case eGL_TEXTURE_2D_ARRAY:
			texSlot = RESTYPE_TEX2DARRAY;
			break;
		case eGL_TEXTURE_CUBE_MAP_ARRAY:
			texSlot = RESTYPE_TEXCUBEARRAY;
			break;
	}

	GLenum target = texDetails.curType;
	GLuint texname = texDetails.resource.name;

	// do blit from renderbuffer to texture, then sample from texture
	if(renderbuffer)
	{
		// need replay context active to do blit (as FBOs aren't shared)
		MakeCurrentReplayContext(&m_ReplayCtx);
	
		GLuint curDrawFBO = 0;
		GLuint curReadFBO = 0;
		gl.glGetIntegerv(eGL_DRAW_FRAMEBUFFER_BINDING, (GLint*)&curDrawFBO);
		gl.glGetIntegerv(eGL_READ_FRAMEBUFFER_BINDING, (GLint*)&curReadFBO);
		
		gl.glBindFramebuffer(eGL_DRAW_FRAMEBUFFER, texDetails.renderbufferFBOs[1]);
		gl.glBindFramebuffer(eGL_READ_FRAMEBUFFER, texDetails.renderbufferFBOs[0]);

		gl.glBlitFramebuffer(0, 0, texDetails.width, texDetails.height,
		                     0, 0, texDetails.width, texDetails.height,
												 GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT,
												 eGL_NEAREST);

		gl.glBindFramebuffer(eGL_DRAW_FRAMEBUFFER, curDrawFBO);
		gl.glBindFramebuffer(eGL_READ_FRAMEBUFFER, curReadFBO);

		texname = texDetails.renderbufferReadTex;
		target = eGL_TEXTURE_2D;
	}
	
	MakeCurrentReplayContext(m_DebugCtx);

	gl.glBindBufferBase(eGL_UNIFORM_BUFFER, 0, DebugData.UBOs[0]);
	HistogramCBufferData *cdata = (HistogramCBufferData *)gl.glMapBufferRange(eGL_UNIFORM_BUFFER, 0, sizeof(HistogramCBufferData),
	                                                                          GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	cdata->HistogramTextureResolution.x = (float)RDCMAX(details.width>>mip, 1U);
	cdata->HistogramTextureResolution.y = (float)RDCMAX(details.height>>mip, 1U);
	cdata->HistogramTextureResolution.z = (float)RDCMAX(details.depth>>mip, 1U);
	cdata->HistogramSlice = (float)sliceFace;
	cdata->HistogramMip = mip;
	cdata->HistogramNumSamples = texDetails.samples;
	cdata->HistogramSample = (int)RDCCLAMP(sample, 0U, details.msSamp-1);
	if(sample == ~0U) cdata->HistogramSample = -int(details.msSamp);
	cdata->HistogramMin = minval;
	cdata->HistogramMax = maxval;
	cdata->HistogramChannels = 0;
	if(channels[0]) cdata->HistogramChannels |= 0x1;
	if(channels[1]) cdata->HistogramChannels |= 0x2;
	if(channels[2]) cdata->HistogramChannels |= 0x4;
	if(channels[3]) cdata->HistogramChannels |= 0x8;
	cdata->HistogramFlags = 0;

	int progIdx = texSlot;

	if(details.format.compType == eCompType_UInt)
	{
		progIdx |= TEXDISPLAY_UINT_TEX;
		intIdx = 1;
	}
	if(details.format.compType == eCompType_SInt)
	{
		progIdx |= TEXDISPLAY_SINT_TEX;
		intIdx = 2;
	}

	if(details.dimension == 3)
		cdata->HistogramSlice = float(sliceFace)/float(details.depth);

	int blocksX = (int)ceil(cdata->HistogramTextureResolution.x/float(HGRAM_PIXELS_PER_TILE*HGRAM_TILES_PER_BLOCK));
	int blocksY = (int)ceil(cdata->HistogramTextureResolution.y/float(HGRAM_PIXELS_PER_TILE*HGRAM_TILES_PER_BLOCK));

	gl.glUnmapBuffer(eGL_UNIFORM_BUFFER);

	gl.glActiveTexture((RDCGLenum)(eGL_TEXTURE0 + texSlot));
	gl.glBindTexture(target, texname);
	if(texSlot == RESTYPE_TEXRECT || texSlot == RESTYPE_TEXBUFFER)
		gl.glBindSampler(texSlot, DebugData.pointNoMipSampler);
	else
		gl.glBindSampler(texSlot, DebugData.pointSampler);

	gl.glBindBufferBase(eGL_SHADER_STORAGE_BUFFER, 0, DebugData.histogramBuf);

	GLuint zero = 0;
	gl.glClearBufferData(eGL_SHADER_STORAGE_BUFFER, eGL_R32UI, eGL_RED, eGL_UNSIGNED_INT, &zero);

	gl.glUseProgram(DebugData.histogramProgram[progIdx]);
	gl.glDispatchCompute(blocksX, blocksY, 1);

	gl.glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

	histogram.clear();
	histogram.resize(HGRAM_NUM_BUCKETS);

	gl.glBindBuffer(eGL_COPY_READ_BUFFER, DebugData.histogramBuf);
	gl.glGetBufferSubData(eGL_COPY_READ_BUFFER, 0, sizeof(uint32_t)*HGRAM_NUM_BUCKETS, &histogram[0]);

	return true;
}

void GLReplay::PickPixel(ResourceId texture, uint32_t x, uint32_t y, uint32_t sliceFace, uint32_t mip, uint32_t sample, float pixel[4])
{
	WrappedOpenGL &gl = *m_pDriver;
	
	MakeCurrentReplayContext(m_DebugCtx);
	
	gl.glBindFramebuffer(eGL_FRAMEBUFFER, DebugData.pickPixelFBO);
	gl.glBindFramebuffer(eGL_READ_FRAMEBUFFER, DebugData.pickPixelFBO);
	
	pixel[0] = pixel[1] = pixel[2] = pixel[3] = 0.0f;
	gl.glClearBufferfv(eGL_COLOR, 0, pixel);

	DebugData.outWidth = DebugData.outHeight = 1.0f;
	gl.glViewport(0, 0, 1, 1);

	{
		TextureDisplay texDisplay;

		texDisplay.Red = texDisplay.Green = texDisplay.Blue = texDisplay.Alpha = true;
		texDisplay.FlipY = false;
		texDisplay.HDRMul = -1.0f;
		texDisplay.linearDisplayAsGamma = true;
		texDisplay.mip = mip;
		texDisplay.sampleIdx = sample;
		texDisplay.CustomShader = ResourceId();
		texDisplay.sliceFace = sliceFace;
		texDisplay.rangemin = 0.0f;
		texDisplay.rangemax = 1.0f;
		texDisplay.scale = 1.0f;
		texDisplay.texid = texture;
		texDisplay.rawoutput = true;
		texDisplay.offx = -float(x);
		texDisplay.offy = -float(y);

		RenderTexture(texDisplay);
	}

	gl.glReadPixels(0, 0, 1, 1, eGL_RGBA, eGL_FLOAT, (void *)pixel);
}

bool GLReplay::RenderTexture(TextureDisplay cfg)
{
	WrappedOpenGL &gl = *m_pDriver;
	
	auto &texDetails = m_pDriver->m_Textures[cfg.texid];

	bool renderbuffer = false;

	int intIdx = 0;

	int resType;
	switch (texDetails.curType)
	{
		case eGL_RENDERBUFFER:
			resType = RESTYPE_TEX2D;
			renderbuffer = true;
			break;
		case eGL_TEXTURE_1D:
			resType = RESTYPE_TEX1D;
			break;
		default:
			RDCWARN("Unexpected texture type");
		case eGL_TEXTURE_2D:
			resType = RESTYPE_TEX2D;
			break;
		case eGL_TEXTURE_2D_MULTISAMPLE:
			resType = RESTYPE_TEX2DMS;
			break;
		case eGL_TEXTURE_RECTANGLE:
			resType = RESTYPE_TEXRECT;
			break;
		case eGL_TEXTURE_BUFFER:
			resType = RESTYPE_TEXBUFFER;
			break;
		case eGL_TEXTURE_3D:
			resType = RESTYPE_TEX3D;
			break;
		case eGL_TEXTURE_CUBE_MAP:
			resType = RESTYPE_TEXCUBE;
			break;
		case eGL_TEXTURE_1D_ARRAY:
			resType = RESTYPE_TEX1DARRAY;
			break;
		case eGL_TEXTURE_2D_ARRAY:
			resType = RESTYPE_TEX2DARRAY;
			break;
		case eGL_TEXTURE_CUBE_MAP_ARRAY:
			resType = RESTYPE_TEXCUBEARRAY;
			break;
	}

	GLuint texname = texDetails.resource.name;
	GLenum target = texDetails.curType;

	// do blit from renderbuffer to texture, then sample from texture
	if(renderbuffer)
	{
		// need replay context active to do blit (as FBOs aren't shared)
		MakeCurrentReplayContext(&m_ReplayCtx);
	
		GLuint curDrawFBO = 0;
		GLuint curReadFBO = 0;
		gl.glGetIntegerv(eGL_DRAW_FRAMEBUFFER_BINDING, (GLint*)&curDrawFBO);
		gl.glGetIntegerv(eGL_READ_FRAMEBUFFER_BINDING, (GLint*)&curReadFBO);
		
		gl.glBindFramebuffer(eGL_DRAW_FRAMEBUFFER, texDetails.renderbufferFBOs[1]);
		gl.glBindFramebuffer(eGL_READ_FRAMEBUFFER, texDetails.renderbufferFBOs[0]);

		gl.glBlitFramebuffer(0, 0, texDetails.width, texDetails.height,
		                     0, 0, texDetails.width, texDetails.height,
												 GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT,
												 eGL_NEAREST);

		gl.glBindFramebuffer(eGL_DRAW_FRAMEBUFFER, curDrawFBO);
		gl.glBindFramebuffer(eGL_READ_FRAMEBUFFER, curReadFBO);

		texname = texDetails.renderbufferReadTex;
		target = eGL_TEXTURE_2D;
	}
	
	MakeCurrentReplayContext(m_DebugCtx);
	
	RDCGLenum dsTexMode = eGL_NONE;
	if(IsDepthStencilFormat(texDetails.internalFormat))
	{
		if (!cfg.Red && cfg.Green)
		{
			dsTexMode = eGL_STENCIL_INDEX;

			// Stencil texture sampling is not normalized in OpenGL
			intIdx = 1;
			float rangeScale;
			switch (texDetails.internalFormat)
			{
				case eGL_STENCIL_INDEX1:
					rangeScale = 1.0f;
					break;
				case eGL_STENCIL_INDEX4:
					rangeScale = 16.0f;
					break;
				default:
					RDCWARN("Unexpected raw format for stencil visualization");
				case eGL_DEPTH24_STENCIL8:
				case eGL_DEPTH32F_STENCIL8:
				case eGL_STENCIL_INDEX8:
					rangeScale = 256.0f;
					break;
				case eGL_STENCIL_INDEX16:
					rangeScale = 65536.0f;
					break;
			}
			cfg.rangemin *= rangeScale;
			cfg.rangemax *= rangeScale;
		}
		else
			dsTexMode = eGL_DEPTH_COMPONENT;
	}
	else
	{
		if(IsUIntFormat(texDetails.internalFormat))
				intIdx = 1;
		if(IsSIntFormat(texDetails.internalFormat))
				intIdx = 2;
	}
	
	gl.glUseProgram(DebugData.texDisplayProg[intIdx]);

	gl.glActiveTexture((RDCGLenum)(eGL_TEXTURE0 + resType));
	gl.glBindTexture(target, texname);

	GLint origDSTexMode = eGL_DEPTH_COMPONENT;
	if (dsTexMode != eGL_NONE)
	{
		gl.glGetTexParameteriv(target, eGL_DEPTH_STENCIL_TEXTURE_MODE, &origDSTexMode);
		gl.glTexParameteri(target, eGL_DEPTH_STENCIL_TEXTURE_MODE, dsTexMode);
	}

	int maxlevel = -1;

	int clampmaxlevel = m_CachedTextures[cfg.texid].mips - 1;
	
	gl.glGetTextureParameterivEXT(texname, target, eGL_TEXTURE_MAX_LEVEL, (GLint *)&maxlevel);
	
	// need to ensure texture is mipmap complete by clamping TEXTURE_MAX_LEVEL.
	if(clampmaxlevel != maxlevel)
	{
		gl.glTextureParameterivEXT(texname, target, eGL_TEXTURE_MAX_LEVEL, (GLint *)&clampmaxlevel);
	}
	else
	{
		maxlevel = -1;
	}

	if(cfg.mip == 0 && cfg.scale < 1.0f && dsTexMode == eGL_NONE && resType != RESTYPE_TEXBUFFER && resType != RESTYPE_TEXRECT)
	{
		gl.glBindSampler(resType, DebugData.linearSampler);
	}
	else
	{
		if(resType == RESTYPE_TEXRECT || resType == RESTYPE_TEX2DMS || resType == RESTYPE_TEXBUFFER)
			gl.glBindSampler(resType, DebugData.pointNoMipSampler);
		else
			gl.glBindSampler(resType, DebugData.pointSampler);
	}
	
	GLint tex_x = texDetails.width, tex_y = texDetails.height, tex_z = texDetails.depth;

	gl.glBindBufferBase(eGL_UNIFORM_BUFFER, 0, DebugData.UBOs[0]);

	texdisplay *ubo = (texdisplay *)gl.glMapBufferRange(eGL_UNIFORM_BUFFER, 0, sizeof(texdisplay), GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);
	
	float x = cfg.offx;
	float y = cfg.offy;
	
	ubo->Position.x = x;
	ubo->Position.y = y;
	ubo->Scale = cfg.scale;
	
	if(cfg.scale <= 0.0f)
	{
		float xscale = DebugData.outWidth/float(tex_x);
		float yscale = DebugData.outHeight/float(tex_y);

		ubo->Scale = RDCMIN(xscale, yscale);

		if(yscale > xscale)
		{
			ubo->Position.x = 0;
			ubo->Position.y = (DebugData.outHeight-(tex_y*ubo->Scale) )*0.5f;
		}
		else
		{
			ubo->Position.y = 0;
			ubo->Position.x = (DebugData.outWidth-(tex_x*ubo->Scale) )*0.5f;
		}
	}

	ubo->HDRMul = cfg.HDRMul;

	ubo->FlipY = cfg.FlipY ? 1 : 0;
	
	if(cfg.rangemax <= cfg.rangemin) cfg.rangemax += 0.00001f;

	if (dsTexMode == eGL_NONE)
	{
		ubo->Channels.x = cfg.Red ? 1.0f : 0.0f;
		ubo->Channels.y = cfg.Green ? 1.0f : 0.0f;
		ubo->Channels.z = cfg.Blue ? 1.0f : 0.0f;
		ubo->Channels.w = cfg.Alpha ? 1.0f : 0.0f;
	}
	else
	{
		// Both depth and stencil texture mode use the red channel
		ubo->Channels.x = 1.0f;
		ubo->Channels.y = 0.0f;
		ubo->Channels.z = 0.0f;
		ubo->Channels.w = 0.0f;
	}

	ubo->RangeMinimum = cfg.rangemin;
	ubo->InverseRangeSize = 1.0f/(cfg.rangemax-cfg.rangemin);
	
	ubo->MipLevel = (float)cfg.mip;
	ubo->Slice = (float)cfg.sliceFace;

	ubo->OutputDisplayFormat = resType;
	
	if(cfg.overlay == eTexOverlay_NaN)
		ubo->OutputDisplayFormat |= TEXDISPLAY_NANS;

	if(cfg.overlay == eTexOverlay_Clipping)
		ubo->OutputDisplayFormat |= TEXDISPLAY_CLIPPING;
	
	if(!IsSRGBFormat(texDetails.internalFormat) && cfg.linearDisplayAsGamma)
		ubo->OutputDisplayFormat |= TEXDISPLAY_GAMMA_CURVE;

	ubo->RawOutput = cfg.rawoutput ? 1 : 0;

	ubo->TextureResolutionPS.x = float(tex_x);
	ubo->TextureResolutionPS.y = float(tex_y);
	ubo->TextureResolutionPS.z = float(tex_z);

	float mipScale = float(1<<cfg.mip);

	ubo->Scale *= mipScale;
	ubo->TextureResolutionPS.x /= mipScale;
	ubo->TextureResolutionPS.y /= mipScale;
	ubo->TextureResolutionPS.z /= mipScale;

	ubo->OutputRes.x = DebugData.outWidth;
	ubo->OutputRes.y = DebugData.outHeight;

	ubo->NumSamples = texDetails.samples;
	ubo->SampleIdx = (int)RDCCLAMP(cfg.sampleIdx, 0U, (uint32_t)texDetails.samples-1);

	// hacky resolve
	if(cfg.sampleIdx == ~0U) ubo->SampleIdx = -1;

	gl.glUnmapBuffer(eGL_UNIFORM_BUFFER);

	if(cfg.rawoutput)
	{
		gl.glDisable(eGL_BLEND);
	}
	else
	{
		gl.glEnable(eGL_BLEND);
		gl.glBlendFunc(eGL_SRC_ALPHA, eGL_ONE_MINUS_SRC_ALPHA);
	}

	gl.glDisable(eGL_DEPTH_TEST);

	gl.glEnable(eGL_FRAMEBUFFER_SRGB);

	gl.glBindVertexArray(DebugData.emptyVAO);
	gl.glDrawArrays(eGL_TRIANGLE_STRIP, 0, 4);
	
	if(maxlevel >= 0)
		gl.glTextureParameterivEXT(texname, target, eGL_TEXTURE_MAX_LEVEL, (GLint *)&maxlevel);

	gl.glBindSampler(0, 0);

	if (dsTexMode != eGL_NONE)
		gl.glTexParameteri(target, eGL_DEPTH_STENCIL_TEXTURE_MODE, origDSTexMode);

	return true;
}

void GLReplay::RenderCheckerboard(Vec3f light, Vec3f dark)
{
	MakeCurrentReplayContext(m_DebugCtx);
	
	WrappedOpenGL &gl = *m_pDriver;
	
	gl.glUseProgram(DebugData.checkerProg);

	gl.glDisable(eGL_DEPTH_TEST);

	gl.glBindBufferBase(eGL_UNIFORM_BUFFER, 0, DebugData.UBOs[0]);
	
	Vec4f *ubo = (Vec4f *)gl.glMapBufferRange(eGL_UNIFORM_BUFFER, 0, sizeof(Vec4f)*2, GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT);

	ubo[0] = Vec4f(light.x, light.y, light.z, 1.0f);
	ubo[1] = Vec4f(dark.x, dark.y, dark.z, 1.0f);
	
	gl.glUnmapBuffer(eGL_UNIFORM_BUFFER);
	
	gl.glBindVertexArray(DebugData.emptyVAO);
	gl.glDrawArrays(eGL_TRIANGLE_STRIP, 0, 4);
}

void GLReplay::RenderHighlightBox(float w, float h, float scale)
{
	MakeCurrentReplayContext(m_DebugCtx);
	
	const float xpixdim = 2.0f/w;
	const float ypixdim = 2.0f/h;
	
	const float xdim = scale*xpixdim;
	const float ydim = scale*ypixdim;

	WrappedOpenGL &gl = *m_pDriver;
	
	gl.glUseProgram(DebugData.genericProg);

	GLint offsetLoc = gl.glGetUniformLocation(DebugData.genericProg, "RENDERDOC_GenericVS_Offset");
	GLint scaleLoc = gl.glGetUniformLocation(DebugData.genericProg, "RENDERDOC_GenericVS_Scale");
	GLint colLoc = gl.glGetUniformLocation(DebugData.genericProg, "RENDERDOC_GenericFS_Color");
	
	Vec4f offsetVal(0.0f, 0.0f, 0.0f, 0.0f);
	Vec4f scaleVal(xdim, ydim, 1.0f, 1.0f);
	Vec4f colVal(1.0f, 1.0f, 1.0f, 1.0f);

	gl.glUniform4fv(offsetLoc, 1, &offsetVal.x);
	gl.glUniform4fv(scaleLoc, 1, &scaleVal.x);
	gl.glUniform4fv(colLoc, 1, &colVal.x);

	gl.glDisable(eGL_DEPTH_TEST);
	
	gl.glBindVertexArray(DebugData.outlineStripVAO);
	gl.glDrawArrays(eGL_LINE_LOOP, 0, 4);

	offsetVal = Vec4f(-xpixdim, ypixdim, 0.0f, 0.0f);
	scaleVal = Vec4f(xdim+xpixdim*2, ydim+ypixdim*2, 1.0f, 1.0f);
	colVal = Vec4f(0.0f, 0.0f, 0.0f, 1.0f);
	
	gl.glUniform4fv(offsetLoc, 1, &offsetVal.x);
	gl.glUniform4fv(scaleLoc, 1, &scaleVal.x);
	gl.glUniform4fv(colLoc, 1, &colVal.x);

	gl.glBindVertexArray(DebugData.outlineStripVAO);
	gl.glDrawArrays(eGL_LINE_LOOP, 0, 4);
}

ResourceId GLReplay::RenderOverlay(ResourceId texid, TextureDisplayOverlay overlay, uint32_t frameID, uint32_t eventID, const vector<uint32_t> &passEvents)
{
	WrappedOpenGL &gl = *m_pDriver;
	
	MakeCurrentReplayContext(&m_ReplayCtx);

	GLuint curProg = 0;
	gl.glGetIntegerv(eGL_CURRENT_PROGRAM, (GLint*)&curProg);

	GLuint curDrawFBO = 0;
	GLuint curReadFBO = 0;
	gl.glGetIntegerv(eGL_DRAW_FRAMEBUFFER_BINDING, (GLint*)&curDrawFBO);
	gl.glGetIntegerv(eGL_READ_FRAMEBUFFER_BINDING, (GLint*)&curReadFBO);

	void *ctx = m_ReplayCtx.ctx;
	
	auto &progDetails = m_pDriver->m_Programs[m_pDriver->GetResourceManager()->GetID(ProgramRes(ctx, curProg))];

	if(progDetails.colOutProg == 0)
	{
		progDetails.colOutProg = gl.glCreateProgram();
		GLuint shad = gl.glCreateShader(eGL_FRAGMENT_SHADER);

		const char *src = DebugData.genericfsSource.c_str();
		gl.glShaderSource(shad, 1, &src, NULL);
		gl.glCompileShader(shad);
		gl.glAttachShader(progDetails.colOutProg, shad);
		gl.glDeleteShader(shad);

		for(size_t i=0; i < progDetails.shaders.size(); i++)
		{
			const auto &shadDetails = m_pDriver->m_Shaders[progDetails.shaders[i]];

			if(shadDetails.type != eGL_FRAGMENT_SHADER)
			{
				shad = gl.glCreateShader(shadDetails.type);

				char **srcs = new char *[shadDetails.sources.size()];
				for(size_t s=0; s < shadDetails.sources.size(); s++)
					srcs[s] = (char *)shadDetails.sources[s].c_str();
				gl.glShaderSource(shad, (GLsizei)shadDetails.sources.size(), srcs, NULL);
				SAFE_DELETE_ARRAY(srcs);

				gl.glCompileShader(shad);
				gl.glAttachShader(progDetails.colOutProg, shad);
				gl.glDeleteShader(shad);
			}
		}

		gl.glLinkProgram(progDetails.colOutProg);
	}
	
	auto &texDetails = m_pDriver->m_Textures[texid];
	
	if(DebugData.overlayTexWidth != texDetails.width || DebugData.overlayTexHeight != texDetails.height)
	{
		if(DebugData.overlayFBO)
		{
			gl.glDeleteFramebuffers(1, &DebugData.overlayFBO);
			gl.glDeleteTextures(1, &DebugData.overlayTex);
		}

		gl.glGenFramebuffers(1, &DebugData.overlayFBO);
		gl.glBindFramebuffer(eGL_FRAMEBUFFER, DebugData.overlayFBO);

		GLuint curTex = 0;
		gl.glGetIntegerv(eGL_TEXTURE_BINDING_2D, (GLint*)&curTex);

		gl.glGenTextures(1, &DebugData.overlayTex);
		gl.glBindTexture(eGL_TEXTURE_2D, DebugData.overlayTex);

		DebugData.overlayTexWidth = texDetails.width;
		DebugData.overlayTexHeight = texDetails.height;

		gl.glTexStorage2D(eGL_TEXTURE_2D, 1, eGL_SRGB8_ALPHA8, texDetails.width, texDetails.height); 
		gl.glTexParameteri(eGL_TEXTURE_2D, eGL_TEXTURE_MIN_FILTER, eGL_NEAREST);
		gl.glTexParameteri(eGL_TEXTURE_2D, eGL_TEXTURE_MAG_FILTER, eGL_NEAREST);
		gl.glTexParameteri(eGL_TEXTURE_2D, eGL_TEXTURE_WRAP_S, eGL_CLAMP_TO_EDGE);
		gl.glTexParameteri(eGL_TEXTURE_2D, eGL_TEXTURE_WRAP_T, eGL_CLAMP_TO_EDGE);
		gl.glFramebufferTexture(eGL_FRAMEBUFFER, eGL_COLOR_ATTACHMENT0, DebugData.overlayTex, 0);
		
		gl.glBindTexture(eGL_TEXTURE_2D, curTex);
	}
	
	gl.glBindFramebuffer(eGL_FRAMEBUFFER, DebugData.overlayFBO);
	
	if(overlay == eTexOverlay_NaN || overlay == eTexOverlay_Clipping)
	{
		// just need the basic texture
		float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		gl.glClearBufferfv(eGL_COLOR, 0, black);
	}
	else if(overlay == eTexOverlay_Drawcall)
	{
		gl.glUseProgram(progDetails.colOutProg);
		
		CopyProgramUniforms(gl.m_Real, curProg, progDetails.colOutProg);
		
		float black[] = { 0.0f, 0.0f, 0.0f, 0.5f };
		gl.glClearBufferfv(eGL_COLOR, 0, black);

		GLint colLoc = gl.glGetUniformLocation(progDetails.colOutProg, "RENDERDOC_GenericFS_Color");
		float colVal[] = { 0.8f, 0.1f, 0.8f, 1.0f };
		gl.glUniform4fv(colLoc, 1, colVal);

		ReplayLog(frameID, 0, eventID, eReplay_OnlyDraw);
		
		gl.glUseProgram(curProg);
	}
	else if(overlay == eTexOverlay_DepthBoth || overlay == eTexOverlay_StencilBoth)
	{
		gl.glUseProgram(progDetails.colOutProg);
		
		CopyProgramUniforms(gl.m_Real, curProg, progDetails.colOutProg);
		
		float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		gl.glClearBufferfv(eGL_COLOR, 0, black);

		GLint depthTest = GL_FALSE;
		gl.glGetIntegerv(eGL_DEPTH_TEST, (GLint*)&depthTest);
		GLint depthMask = GL_FALSE;
		gl.glGetIntegerv(eGL_DEPTH_WRITEMASK, (GLint*)&depthMask);
		
		GLint stencilTest = GL_FALSE;
		gl.glGetIntegerv(eGL_STENCIL_TEST, (GLint*)&stencilTest);
		GLuint stencilMaskFront = 0xff;
		gl.glGetIntegerv(eGL_STENCIL_WRITEMASK, (GLint*)&stencilMaskFront);
		GLuint stencilMaskBack = 0xff;
		gl.glGetIntegerv(eGL_STENCIL_BACK_WRITEMASK, (GLint*)&stencilMaskBack);
		
		gl.glDisable(eGL_DEPTH_TEST);
		gl.glDepthMask(GL_FALSE);
		gl.glDisable(eGL_STENCIL_TEST);
		gl.glStencilMask(0);

		GLint colLoc = gl.glGetUniformLocation(progDetails.colOutProg, "RENDERDOC_GenericFS_Color");
		float red[] = { 1.0f, 0.0f, 0.0f, 1.0f };
		gl.glUniform4fv(colLoc, 1, red);

		ReplayLog(frameID, 0, eventID, eReplay_OnlyDraw);

		GLuint curDepth = 0, curStencil = 0;

		gl.glGetNamedFramebufferAttachmentParameterivEXT(curDrawFBO, eGL_DEPTH_ATTACHMENT, eGL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, (GLint*)&curDepth);
		gl.glGetNamedFramebufferAttachmentParameterivEXT(curDrawFBO, eGL_STENCIL_ATTACHMENT, eGL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, (GLint*)&curStencil);

		GLuint depthCopy = 0, stencilCopy = 0;

		// TODO fetch mip in use
		// TODO handle non-2D and fetch slice
		GLint mip = 0;

		// create matching depth for existing FBO
		if(curDepth != 0)
		{
			GLuint curTex = 0;
			gl.glGetIntegerv(eGL_TEXTURE_BINDING_2D, (GLint*)&curTex);

			GLenum fmt;
			gl.glGetTextureLevelParameterivEXT(curDepth, eGL_TEXTURE_2D, mip, eGL_TEXTURE_INTERNAL_FORMAT, (GLint *)&fmt);

			gl.glGenTextures(1, &depthCopy);
			gl.glBindTexture(eGL_TEXTURE_2D, depthCopy);
			gl.glTexStorage2D(eGL_TEXTURE_2D, 1, fmt, DebugData.overlayTexWidth, DebugData.overlayTexHeight);
			gl.glTexParameteri(eGL_TEXTURE_2D, eGL_TEXTURE_MIN_FILTER, eGL_NEAREST);
			gl.glTexParameteri(eGL_TEXTURE_2D, eGL_TEXTURE_MAG_FILTER, eGL_NEAREST);
			gl.glTexParameteri(eGL_TEXTURE_2D, eGL_TEXTURE_WRAP_S, eGL_CLAMP_TO_EDGE);
			gl.glTexParameteri(eGL_TEXTURE_2D, eGL_TEXTURE_WRAP_T, eGL_CLAMP_TO_EDGE);

			gl.glBindTexture(eGL_TEXTURE_2D, curTex);
		}

		// create matching separate stencil if relevant
		if(curStencil != curDepth && curStencil != 0)
		{
			GLuint curTex = 0;
			gl.glGetIntegerv(eGL_TEXTURE_BINDING_2D, (GLint*)&curTex);

			GLenum fmt;
			gl.glGetTextureLevelParameterivEXT(curStencil, eGL_TEXTURE_2D, mip, eGL_TEXTURE_INTERNAL_FORMAT, (GLint *)&fmt);

			gl.glGenTextures(1, &stencilCopy);
			gl.glBindTexture(eGL_TEXTURE_2D, stencilCopy);
			gl.glTexStorage2D(eGL_TEXTURE_2D, 1, fmt, DebugData.overlayTexWidth, DebugData.overlayTexHeight);
			gl.glTexParameteri(eGL_TEXTURE_2D, eGL_TEXTURE_MIN_FILTER, eGL_NEAREST);
			gl.glTexParameteri(eGL_TEXTURE_2D, eGL_TEXTURE_MAG_FILTER, eGL_NEAREST);
			gl.glTexParameteri(eGL_TEXTURE_2D, eGL_TEXTURE_WRAP_S, eGL_CLAMP_TO_EDGE);
			gl.glTexParameteri(eGL_TEXTURE_2D, eGL_TEXTURE_WRAP_T, eGL_CLAMP_TO_EDGE);

			gl.glBindTexture(eGL_TEXTURE_2D, curTex);
		}

		// bind depth/stencil to overlay FBO
		if(curDepth != 0 && curDepth == curStencil)
			gl.glFramebufferTexture(eGL_DRAW_FRAMEBUFFER, eGL_DEPTH_STENCIL_ATTACHMENT, depthCopy, mip);
		else if(curDepth != 0)
			gl.glFramebufferTexture(eGL_DRAW_FRAMEBUFFER, eGL_DEPTH_ATTACHMENT, depthCopy, mip);
		else if(curStencil != 0)
			gl.glFramebufferTexture(eGL_DRAW_FRAMEBUFFER, eGL_STENCIL_ATTACHMENT, stencilCopy, mip);

		gl.glBindFramebuffer(eGL_READ_FRAMEBUFFER, curDrawFBO);

		float green[] = { 0.0f, 1.0f, 0.0f, 1.0f };
		gl.glUniform4fv(colLoc, 1, green);

		if(overlay == eTexOverlay_DepthBoth)
		{
			if(depthTest)
				gl.glEnable(eGL_DEPTH_TEST);
			else
				gl.glDisable(eGL_DEPTH_TEST);

			if(depthMask)
				gl.glDepthMask(GL_TRUE);
			else
				gl.glDepthMask(GL_FALSE);
		}
		else
		{
			if(stencilTest)
				gl.glEnable(eGL_STENCIL_TEST);
			else
				gl.glDisable(eGL_STENCIL_TEST);

			gl.glStencilMaskSeparate(eGL_FRONT, stencilMaskFront);
			gl.glStencilMaskSeparate(eGL_BACK, stencilMaskBack);
		}

		// get latest depth/stencil from read FBO (existing FBO) into draw FBO (overlay FBO)
		gl.glBlitFramebuffer(0, 0, DebugData.overlayTexWidth, DebugData.overlayTexHeight,
		                     0, 0, DebugData.overlayTexWidth, DebugData.overlayTexHeight,
												 GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT, eGL_NEAREST);

		ReplayLog(frameID, 0, eventID, eReplay_OnlyDraw);

		// unset and delete temp depth/stencil
		gl.glFramebufferTexture(eGL_DRAW_FRAMEBUFFER, eGL_DEPTH_STENCIL_ATTACHMENT, 0, 0);
		if(depthCopy != 0)   gl.glDeleteTextures(1, &depthCopy);
		if(stencilCopy != 0) gl.glDeleteTextures(1, &stencilCopy);

		if(depthTest)
			gl.glEnable(eGL_DEPTH_TEST);
		else
			gl.glDisable(eGL_DEPTH_TEST);

		gl.glDepthMask(depthMask ? GL_TRUE : GL_FALSE);
		
		if(stencilTest)
			gl.glEnable(eGL_STENCIL_TEST);
		else
			gl.glDisable(eGL_STENCIL_TEST);

		gl.glStencilMaskSeparate(eGL_FRONT, stencilMaskFront);
		gl.glStencilMaskSeparate(eGL_BACK, stencilMaskBack);

		gl.glUseProgram(curProg);
	}
	else if(overlay == eTexOverlay_Wireframe)
	{
		gl.glUseProgram(progDetails.colOutProg);
		
		CopyProgramUniforms(gl.m_Real, curProg, progDetails.colOutProg);
		
		float wireCol[] = { 200.0f/255.0f, 255.0f/255.0f, 0.0f/255.0f, 0.0f };
		gl.glClearBufferfv(eGL_COLOR, 0, wireCol);

		GLint colLoc = gl.glGetUniformLocation(progDetails.colOutProg, "RENDERDOC_GenericFS_Color");
		wireCol[3] = 1.0f;
		gl.glUniform4fv(colLoc, 1, wireCol);

		GLint depthTest = GL_FALSE;
		gl.glGetIntegerv(eGL_DEPTH_TEST, (GLint*)&depthTest);
		GLenum polyMode = eGL_FILL;
		if(!VendorCheck[VendorCheck_AMD_polygon_mode_query])
			gl.glGetIntegerv(eGL_POLYGON_MODE, (GLint*)&polyMode);

		gl.glDisable(eGL_DEPTH_TEST);
		gl.glPolygonMode(eGL_FRONT_AND_BACK, eGL_LINE);

		ReplayLog(frameID, 0, eventID, eReplay_OnlyDraw);

		if(depthTest)
			gl.glEnable(eGL_DEPTH_TEST);
		if(polyMode != eGL_LINE)
			gl.glPolygonMode(eGL_FRONT_AND_BACK, polyMode);
		
		gl.glUseProgram(curProg);
	}
	else if(overlay == eTexOverlay_ViewportScissor)
	{
		float col[] = { 0.0f, 0.0f, 0.0f, 0.0f };
		gl.glClearBufferfv(eGL_COLOR, 0, col);

		GLint depthTest = GL_FALSE;
		gl.glGetIntegerv(eGL_DEPTH_TEST, (GLint*)&depthTest);
		GLint depthMask = GL_FALSE;
		gl.glGetIntegerv(eGL_DEPTH_WRITEMASK, (GLint*)&depthMask);
		
		GLint stencilTest = GL_FALSE;
		gl.glGetIntegerv(eGL_STENCIL_TEST, (GLint*)&stencilTest);
		GLuint stencilMaskFront = 0;
		gl.glGetIntegerv(eGL_STENCIL_WRITEMASK, (GLint*)&stencilMaskFront);
		GLuint stencilMaskBack = 0;
		gl.glGetIntegerv(eGL_STENCIL_BACK_WRITEMASK, (GLint*)&stencilMaskBack);
		
		GLint cullMask = GL_FALSE;
		gl.glGetIntegerv(eGL_CULL_FACE, (GLint*)&cullMask);
		GLint scissorTest = GL_FALSE;
		gl.glGetIntegeri_v(eGL_SCISSOR_TEST, 0, (GLint*)&scissorTest);
		
		gl.glDisable(eGL_DEPTH_TEST);
		gl.glDepthMask(GL_FALSE);
		gl.glDisable(eGL_STENCIL_TEST);
		gl.glStencilMaskSeparate(eGL_FRONT, 0);
		gl.glStencilMaskSeparate(eGL_BACK, 0);
		gl.glDisable(eGL_CULL_FACE);
		gl.glDisablei(eGL_SCISSOR_TEST, 0);
		
		gl.glUseProgram(DebugData.replayQuadProg);
		
		GLint colLoc = gl.glGetUniformLocation(DebugData.replayQuadProg, "RENDERDOC_GenericFS_Color");
		float viewportConsts[] = { 0.15f, 0.3f, 0.6f, 0.3f };
		gl.glUniform4fv(colLoc, 1, viewportConsts);

		gl.glDrawArrays(eGL_TRIANGLE_STRIP, 0, 4);
		
		gl.glEnablei(eGL_SCISSOR_TEST, 0);
		
		float scissorConsts[] = { 0.5f, 0.6f, 0.8f, 0.3f };
		gl.glUniform4fv(colLoc, 1, scissorConsts);

		gl.glDrawArrays(eGL_TRIANGLE_STRIP, 0, 4);
		
		if(depthTest)
			gl.glEnable(eGL_DEPTH_TEST);
		else
			gl.glDisable(eGL_DEPTH_TEST);
		gl.glDepthMask(depthMask ? GL_TRUE : GL_FALSE);
		
		if(stencilTest)
			gl.glEnable(eGL_STENCIL_TEST);
		else
			gl.glDisable(eGL_STENCIL_TEST);
		gl.glStencilMaskSeparate(eGL_FRONT, stencilMaskFront);
		gl.glStencilMaskSeparate(eGL_BACK, stencilMaskBack);

		if(cullMask)
			gl.glEnable(eGL_CULL_FACE);
		else
			gl.glDisable(eGL_CULL_FACE);
		if(scissorTest)
			gl.glEnablei(eGL_SCISSOR_TEST, 0);
		else
			gl.glDisablei(eGL_SCISSOR_TEST, 0);

		gl.glUseProgram(curProg);
	}
	
	gl.glBindFramebuffer(eGL_DRAW_FRAMEBUFFER, curDrawFBO);
	gl.glBindFramebuffer(eGL_READ_FRAMEBUFFER, curReadFBO);

	return m_pDriver->GetResourceManager()->GetID(TextureRes(ctx, DebugData.overlayTex));
}

void GLReplay::InitPostVSBuffers(uint32_t frameID, uint32_t eventID)
{
	auto idx = std::make_pair(frameID, eventID);
	if(m_PostVSData.find(idx) != m_PostVSData.end())
		return;
	
	MakeCurrentReplayContext(&m_ReplayCtx);
	
	void *ctx = m_ReplayCtx.ctx;
	
	WrappedOpenGL &gl = *m_pDriver;
	GLResourceManager *rm = m_pDriver->GetResourceManager();
	
	GLRenderState rs(&gl.GetHookset(), NULL, READING);
	rs.FetchState(ctx, &gl);
	GLuint elArrayBuffer = 0;
	if(rs.VAO)
		gl.glGetIntegerv(eGL_ELEMENT_ARRAY_BUFFER_BINDING, (GLint *)&elArrayBuffer);

	// reflection structures
	ShaderReflection *vsRefl = NULL;
	ShaderReflection *tesRefl = NULL;
	ShaderReflection *gsRefl = NULL;

	// non-program used separable programs of each shader.
	// we'll add our feedback varings to these programs, relink,
	// and combine into a pipeline for use.
	GLuint vsProg = 0;
	GLuint tcsProg = 0;
	GLuint tesProg = 0;
	GLuint gsProg = 0;

	// these are the 'real' programs with uniform values that we need
	// to copy over to our separable programs.
	GLuint vsProgSrc = 0;
	GLuint tcsProgSrc = 0;
	GLuint tesProgSrc = 0;
	GLuint gsProgSrc = 0;

	if(rs.Program == 0)
	{
		if(rs.Pipeline == 0)
		{
			return;
		}
		else
		{
			ResourceId id = rm->GetID(ProgramPipeRes(ctx, rs.Pipeline));
			auto &pipeDetails = m_pDriver->m_Pipelines[id];

			if(pipeDetails.stageShaders[0] != ResourceId())
			{
				vsRefl = GetShader(pipeDetails.stageShaders[0]);
				vsProg = m_pDriver->m_Shaders[pipeDetails.stageShaders[0]].prog;
				vsProgSrc = rm->GetCurrentResource(pipeDetails.stagePrograms[0]).name;
			}
			if(pipeDetails.stageShaders[1] != ResourceId())
			{
				tcsProg = m_pDriver->m_Shaders[pipeDetails.stageShaders[1]].prog;
				tcsProgSrc = rm->GetCurrentResource(pipeDetails.stagePrograms[1]).name;
			}
			if(pipeDetails.stageShaders[2] != ResourceId())
			{
				tesRefl = GetShader(pipeDetails.stageShaders[2]);
				tesProg = m_pDriver->m_Shaders[pipeDetails.stageShaders[2]].prog;
				tesProgSrc = rm->GetCurrentResource(pipeDetails.stagePrograms[2]).name;
			}
			if(pipeDetails.stageShaders[3] != ResourceId())
			{
				gsRefl = GetShader(pipeDetails.stageShaders[3]);
				gsProg = m_pDriver->m_Shaders[pipeDetails.stageShaders[3]].prog;
				gsProgSrc = rm->GetCurrentResource(pipeDetails.stagePrograms[3]).name;
			}
		}
	}
	else
	{
		auto &progDetails = m_pDriver->m_Programs[rm->GetID(ProgramRes(ctx, rs.Program))];

		if(progDetails.stageShaders[0] != ResourceId())
		{
			vsRefl = GetShader(progDetails.stageShaders[0]);
			vsProg = m_pDriver->m_Shaders[progDetails.stageShaders[0]].prog;
		}
		if(progDetails.stageShaders[1] != ResourceId())
		{
			tcsProg = m_pDriver->m_Shaders[progDetails.stageShaders[1]].prog;
		}
		if(progDetails.stageShaders[2] != ResourceId())
		{
			tesRefl = GetShader(progDetails.stageShaders[2]);
			tesProg = m_pDriver->m_Shaders[progDetails.stageShaders[2]].prog;
		}
		if(progDetails.stageShaders[3] != ResourceId())
		{
			gsRefl = GetShader(progDetails.stageShaders[3]);
			gsProg = m_pDriver->m_Shaders[progDetails.stageShaders[3]].prog;
		}

		vsProgSrc = tcsProgSrc = tesProgSrc = gsProgSrc = rs.Program;
	}

	if(vsRefl == NULL)
	{
		// no vertex shader bound (no vertex processing - compute only program
		// or no program bound, for a clear etc)
		m_PostVSData[idx] = GLPostVSData();
		return;
	}

	const FetchDrawcall *drawcall = m_pDriver->GetDrawcall(frameID, eventID);

	if(drawcall->numIndices == 0)
	{
		// draw is 0 length, nothing to do
		m_PostVSData[idx] = GLPostVSData();
		return;
	}
	
	GLenum gsOutputType = eGL_NONE;

	if(gsProg)
		gl.glGetProgramiv(gsProg, eGL_GEOMETRY_OUTPUT_TYPE, (GLint *)&gsOutputType);
	
	vector<const char *> varyings;

	// we don't want to do any work, so just discard before rasterizing
	gl.glEnable(eGL_RASTERIZER_DISCARD);

	varyings.clear();

	uint32_t stride = 0;
	uint32_t posoffset = ~0U;

	for(int32_t i=0; i < vsRefl->OutputSig.count; i++)
	{
		varyings.push_back(vsRefl->OutputSig[i].varName.elems);

		if(!strcmp(vsRefl->OutputSig[i].varName.elems, "gl_Position"))
			posoffset = stride;

		stride += sizeof(float)*vsRefl->OutputSig[i].compCount;
	}

	gl.glTransformFeedbackVaryings(vsProg, (GLsizei)varyings.size(), &varyings[0], eGL_INTERLEAVED_ATTRIBS);

	// relink separable program with varyings
	gl.glLinkProgram(vsProg);

	GLint status = 0;
	gl.glGetProgramiv(vsProg, eGL_LINK_STATUS, &status);
	if(status == 0)
	{
		char buffer[1025] = {0};
		gl.glGetProgramInfoLog(vsProg, 1024, NULL, buffer);
		RDCERR("Link error making xfb vs program: %s", buffer);
		m_PostVSData[idx] = GLPostVSData();
		return;
	}

	// make a pipeline to contain just the vertex shader
	GLuint vsFeedbackPipe = 0;
	gl.glGenProgramPipelines(1, &vsFeedbackPipe);

	// bind the separable vertex program to it
	gl.glUseProgramStages(vsFeedbackPipe, eGL_VERTEX_SHADER_BIT, vsProg);

	// copy across any uniform values, bindings etc from the real program containing
	// the vertex stage
	CopyProgramUniforms(gl.GetHookset(), vsProgSrc, vsProg);

	// bind our program and do the feedback draw
	gl.glUseProgram(0);
	gl.glBindProgramPipeline(vsFeedbackPipe);

	gl.glBindTransformFeedback(eGL_TRANSFORM_FEEDBACK, DebugData.feedbackObj);

	// need to rebind this here because of an AMD bug that seems to ignore the buffer
	// bindings in the feedback object - or at least it errors if the default feedback
	// object has no buffers bound. Fortunately the state is still object-local so
	// we don't have to restore the buffer binding on the default feedback object.
	gl.glBindBufferBase(eGL_TRANSFORM_FEEDBACK_BUFFER, 0, DebugData.feedbackBuffer);

	GLuint idxBuf = 0;
	
	gl.glBeginQuery(eGL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN, DebugData.feedbackQuery);
	gl.glBeginTransformFeedback(eGL_POINTS);

	if((drawcall->flags & eDraw_UseIBuffer) == 0)
	{
		gl.glDrawArrays(eGL_POINTS, drawcall->vertexOffset, drawcall->numIndices);
	}
	else // drawcall is indexed
	{
		ResourceId idxId = rm->GetID(BufferRes(NULL, elArrayBuffer));

		vector<byte> idxdata = GetBufferData(idxId, drawcall->indexOffset*drawcall->indexByteWidth, drawcall->numIndices*drawcall->indexByteWidth);
		
		vector<uint32_t> indices;
		
		uint8_t  *idx8 =  (uint8_t *) &idxdata[0];
		uint16_t *idx16 = (uint16_t *)&idxdata[0];
		uint32_t *idx32 = (uint32_t *)&idxdata[0];

		// only read as many indices as were available in the buffer
		uint32_t numIndices = RDCMIN(uint32_t(idxdata.size()/drawcall->indexByteWidth), drawcall->numIndices);

		// grab all unique vertex indices referenced
		for(uint32_t i=0; i < numIndices; i++)
		{
			uint32_t i32 = 0;
			     if(drawcall->indexByteWidth == 1) i32 = uint32_t(idx8 [i]);
			else if(drawcall->indexByteWidth == 2) i32 = uint32_t(idx16[i]);
			else if(drawcall->indexByteWidth == 4) i32 =          idx32[i];

			auto it = std::lower_bound(indices.begin(), indices.end(), i32);

			if(it != indices.end() && *it == i32)
				continue;

			indices.insert(it, i32);
		}

		// An index buffer could be something like: 500, 501, 502, 501, 503, 502
		// in which case we can't use the existing index buffer without filling 499 slots of vertex
		// data with padding. Instead we rebase the indices based on the smallest vertex so it becomes
		// 0, 1, 2, 1, 3, 2 and then that matches our stream-out'd buffer.
		//
		// Since we want the indices to be preserved in order to easily match up inputs to outputs,
		// but shifted, fill in gaps in our streamout vertex buffer with the lowest index value.
		// (use the lowest index value so that even the gaps are a 'valid' vertex, rather than
		// potentially garbage data).
		uint32_t minindex = indices.empty() ? 0 : indices[0];

		// indices[] contains ascending unique vertex indices referenced. Fill gaps with minindex
		for(size_t i=1; i < indices.size(); i++)
		{
			if(indices[i]-1 > indices[i-1])
			{
				size_t gapsize = size_t( (indices[i]-1) - indices[i-1] );

				indices.insert(indices.begin()+i, gapsize, minindex);

				i += gapsize;
			}
		}
		
		// generate a temporary index buffer with our 'unique index set' indices,
		// so we can transform feedback each referenced vertex once
		GLuint indexSetBuffer = 0;
		gl.glGenBuffers(1, &indexSetBuffer);
		gl.glBindBuffer(eGL_ELEMENT_ARRAY_BUFFER, indexSetBuffer);
		gl.glNamedBufferStorageEXT(indexSetBuffer, sizeof(uint32_t)*indices.size(), &indices[0], 0);
		
		gl.glDrawElementsBaseVertex(eGL_POINTS, (GLsizei)indices.size(), eGL_UNSIGNED_INT, NULL, drawcall->vertexOffset);
		
		// delete the buffer, we don't need it anymore
		gl.glBindBuffer(eGL_ELEMENT_ARRAY_BUFFER, elArrayBuffer);
		gl.glDeleteBuffers(1, &indexSetBuffer);
		
		// rebase existing index buffer to point from 0 onwards (which will index into our
		// stream-out'd vertex buffer)
		if(drawcall->indexByteWidth == 1)
		{
			for(uint32_t i=0; i < numIndices; i++)
				idx8[i] -= uint8_t(minindex&0xff);
		}
		else if(drawcall->indexByteWidth == 2)
		{
			for(uint32_t i=0; i < numIndices; i++)
				idx16[i] -= uint16_t(minindex&0xffff);
		}
		else
		{
			for(uint32_t i=0; i < numIndices; i++)
				idx32[i] -= minindex;
		}
		
		// make the index buffer that can be used to render this postvs data - the original
		// indices, rebased with minindex being 0 (since we transform feedback to the start
		// of our feedback buffer).
		gl.glGenBuffers(1, &idxBuf);
		gl.glBindBuffer(eGL_ELEMENT_ARRAY_BUFFER, idxBuf);
		gl.glNamedBufferStorageEXT(idxBuf, (GLsizeiptr)idxdata.size(), &idxdata[0], 0);
		
		// restore previous element array buffer binding
		gl.glBindBuffer(eGL_ELEMENT_ARRAY_BUFFER, elArrayBuffer);
	}
	
	gl.glEndTransformFeedback();
	gl.glEndQuery(eGL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);

	// this should be the same as the draw size
	GLuint primsWritten = 0;
	gl.glGetQueryObjectuiv(DebugData.feedbackQuery, eGL_QUERY_RESULT, &primsWritten);

	// get buffer data from buffer attached to feedback object
	float *data = (float *)gl.glMapNamedBufferEXT(DebugData.feedbackBuffer, eGL_READ_ONLY);

	// create a buffer with this data, for future use (typed to ARRAY_BUFFER so we
	// can render from it to display previews).
	GLuint vsoutBuffer = 0;
	gl.glGenBuffers(1, &vsoutBuffer);
	gl.glBindBuffer(eGL_ARRAY_BUFFER, vsoutBuffer);
	gl.glNamedBufferStorageEXT(vsoutBuffer, stride*primsWritten, data, 0);

	byte *byteData = (byte *)data;

	float nearp = 0.0f;
	float farp = 0.0f;

	Vec4f *pos0 = (Vec4f *)(byteData + posoffset);

	for(GLuint i=1; posoffset != ~0U && i < primsWritten; i++)
	{
		//////////////////////////////////////////////////////////////////////////////////
		// derive near/far, assuming a standard perspective matrix
		//
		// the transformation from from pre-projection {Z,W} to post-projection {Z,W}
		// is linear. So we can say Zpost = Zpre*m + c . Here we assume Wpre = 1
		// and we know Wpost = Zpre from the perspective matrix.
		// we can then see from the perspective matrix that
		// m = F/(F-N)
		// c = -(F*N)/(F-N)
		//
		// with re-arranging and substitution, we then get:
		// N = -c/m
		// F = c/(1-m)
		//
		// so if we can derive m and c then we can determine N and F. We can do this with
		// two points, and we pick them reasonably distinct on z to reduce floating-point
		// error

		Vec4f *pos = (Vec4f *)(byteData + posoffset + i*stride);

		if(fabs(pos->w - pos0->w) > 0.01f)
		{
			Vec2f A(pos0->w, pos0->z);
			Vec2f B(pos->w, pos->z);

			float m = (B.y-A.y)/(B.x-A.x);
			float c = B.y - B.x*m;

			if(m == 1.0f) continue;

			nearp = -c/m;
			farp = c/(1-m);

			break;
		}
	}

	gl.glUnmapNamedBufferEXT(DebugData.feedbackBuffer);

	// store everything out to the PostVS data cache
	m_PostVSData[idx].vsin.topo = drawcall->topology;
	m_PostVSData[idx].vsout.buf = vsoutBuffer;
	m_PostVSData[idx].vsout.posOffset = posoffset;
	m_PostVSData[idx].vsout.vertStride = stride;
	m_PostVSData[idx].vsout.nearPlane = nearp;
	m_PostVSData[idx].vsout.farPlane = farp;

	m_PostVSData[idx].vsout.useIndices = (drawcall->flags & eDraw_UseIBuffer) > 0;
	m_PostVSData[idx].vsout.numVerts = drawcall->numIndices;

	m_PostVSData[idx].vsout.idxBuf = 0;
	m_PostVSData[idx].vsout.idxByteWidth = drawcall->indexByteWidth;
	if(m_PostVSData[idx].vsout.useIndices && idxBuf)
	{
		m_PostVSData[idx].vsout.idxBuf = idxBuf;
	}

	m_PostVSData[idx].vsout.topo = drawcall->topology;

	// set vsProg back to no varyings, for future use
	gl.glTransformFeedbackVaryings(vsProg, 0, NULL, eGL_INTERLEAVED_ATTRIBS);
	gl.glLinkProgram(vsProg);

	GLuint lastFeedbackPipe = 0;
	
	// delete temporary pipelines we made
	gl.glDeleteProgramPipelines(1, &vsFeedbackPipe);
	if(lastFeedbackPipe) gl.glDeleteProgramPipelines(1, &lastFeedbackPipe);

	// restore replay state we trashed
	gl.glUseProgram(rs.Program);
	gl.glBindProgramPipeline(rs.Pipeline);
	
	gl.glBindBuffer(eGL_ARRAY_BUFFER, rs.BufferBindings[GLRenderState::eBufIdx_Array]);
	gl.glBindBuffer(eGL_ELEMENT_ARRAY_BUFFER, elArrayBuffer);

	gl.glBindTransformFeedback(eGL_TRANSFORM_FEEDBACK, rs.FeedbackObj);
	
	if(!rs.Enabled[GLRenderState::eEnabled_RasterizerDiscard])
		gl.glDisable(eGL_RASTERIZER_DISCARD);
	else
		gl.glEnable(eGL_RASTERIZER_DISCARD);
}

MeshFormat GLReplay::GetPostVSBuffers(uint32_t frameID, uint32_t eventID, MeshDataStage stage)
{
	GLPostVSData postvs;
	RDCEraseEl(postvs);

	auto idx = std::make_pair(frameID, eventID);
	if(m_PostVSData.find(idx) != m_PostVSData.end())
		postvs = m_PostVSData[idx];

	GLPostVSData::StageData s = postvs.GetStage(stage);
	
	MeshFormat ret;
	
	if(s.useIndices && s.idxBuf)
		ret.idxbuf = m_pDriver->GetResourceManager()->GetID(BufferRes(NULL, s.idxBuf));
	else
		ret.idxbuf = ResourceId();
	ret.idxoffs = 0;
	ret.idxByteWidth = s.idxByteWidth;

	if(s.buf)
		ret.buf = m_pDriver->GetResourceManager()->GetID(BufferRes(NULL, s.buf));
	else
		ret.buf = ResourceId();

	ret.offset = s.posOffset;
	ret.stride = s.vertStride;

	ret.compCount = 4;
	ret.compByteWidth = 4;
	ret.compType = eCompType_Float;
	ret.specialFormat = eSpecial_Unknown;

	ret.showAlpha = false;

	ret.topo = s.topo;
	ret.numVerts = s.numVerts;

	ret.unproject = true;
	ret.nearPlane = s.nearPlane;
	ret.farPlane = s.farPlane;

	return ret;
}

FloatVector GLReplay::InterpretVertex(byte *data, uint32_t vert, MeshDisplay cfg, byte *end, bool &valid)
{
	FloatVector ret(0.0f, 0.0f, 0.0f, 1.0f);

	if(m_HighlightCache.useidx)
	{
		if(vert >= (uint32_t)m_HighlightCache.indices.size())
		{
			valid = false;
			return ret;
		}

		vert = m_HighlightCache.indices[vert];
	}

	data += vert*cfg.position.stride;

	float *out = &ret.x;

	ResourceFormat fmt;
	fmt.compByteWidth = cfg.position.compByteWidth;
	fmt.compCount = cfg.position.compCount;
	fmt.compType = cfg.position.compType;

	if(cfg.position.specialFormat == eSpecial_R10G10B10A2)
	{
		if(data+4 >= end)
		{
			valid = false;
			return ret;
		}

		Vec4f v = ConvertFromR10G10B10A2(*(uint32_t *)data);
		ret.x = v.x;
		ret.y = v.y;
		ret.z = v.z;
		ret.w = v.w;
		return ret;
	}
	else if(cfg.position.specialFormat == eSpecial_R11G11B10)
	{
		if(data+4 >= end)
		{
			valid = false;
			return ret;
		}

		Vec3f v = ConvertFromR11G11B10(*(uint32_t *)data);
		ret.x = v.x;
		ret.y = v.y;
		ret.z = v.z;
		return ret;
	}
	else if(cfg.position.specialFormat == eSpecial_B8G8R8A8)
	{
		if(data+4 >= end)
		{
			valid = false;
			return ret;
		}

		fmt.compByteWidth = 1;
		fmt.compCount = 4;
		fmt.compType = eCompType_UNorm;
	}
	
	if(data + cfg.position.compCount*cfg.position.compByteWidth > end)
	{
		valid = false;
		return ret;
	}

	for(uint32_t i=0; i < cfg.position.compCount; i++)
	{
		*out = ConvertComponent(fmt, data);

		data += cfg.position.compByteWidth;
		out++;
	}

	if(cfg.position.specialFormat == eSpecial_B8G8R8A8)
	{
		FloatVector reversed;
		reversed.x = ret.x;
		reversed.y = ret.y;
		reversed.z = ret.z;
		reversed.w = ret.w;
		return reversed;
	}

	return ret;
}

void GLReplay::RenderMesh(uint32_t frameID, uint32_t eventID, const vector<MeshFormat> &secondaryDraws, MeshDisplay cfg)
{
	WrappedOpenGL &gl = *m_pDriver;

	if(cfg.position.buf == ResourceId())
		return;
	
	MakeCurrentReplayContext(m_DebugCtx);
	
	Matrix4f projMat = Matrix4f::Perspective(90.0f, 0.1f, 100000.0f, DebugData.outWidth/DebugData.outHeight);

	Camera cam;
	if(cfg.arcballCamera)
		cam.Arcball(cfg.cameraPos.x, Vec3f(cfg.cameraRot.x, cfg.cameraRot.y, cfg.cameraRot.z));
	else
		cam.fpsLook(Vec3f(cfg.cameraPos.x, cfg.cameraPos.y, cfg.cameraPos.z), Vec3f(cfg.cameraRot.x, cfg.cameraRot.y, cfg.cameraRot.z));

	Matrix4f camMat = cam.GetMatrix();

	Matrix4f ModelViewProj = projMat.Mul(camMat);
	Matrix4f guessProjInv;
	
	gl.glBindVertexArray(DebugData.meshVAO);

	const MeshFormat *fmts[2] = { &cfg.position, &cfg.second };
	
	GLenum topo = MakeGLPrimitiveTopology(cfg.position.topo);

	GLuint prog = DebugData.meshProg;
	
	if(cfg.solidShadeMode == eShade_Lit)
	{
		// pick program with GS for per-face lighting
		prog = DebugData.meshgsProg;
	}
	
	GLint colLoc = gl.glGetUniformLocation(prog, "RENDERDOC_GenericFS_Color");
	GLint mvpLoc = gl.glGetUniformLocation(prog, "ModelViewProj");
	GLint fmtLoc = gl.glGetUniformLocation(prog, "Mesh_DisplayFormat");
	GLint sizeLoc = gl.glGetUniformLocation(prog, "PointSpriteSize");
	GLint homogLoc = gl.glGetUniformLocation(prog, "HomogenousInput");
	
	gl.glUseProgram(prog);
	

	gl.glEnable(eGL_FRAMEBUFFER_SRGB);

	if(cfg.position.unproject)
	{
		// the derivation of the projection matrix might not be right (hell, it could be an
		// orthographic projection). But it'll be close enough likely.
		Matrix4f guessProj = Matrix4f::Perspective(cfg.fov, cfg.position.nearPlane, cfg.position.farPlane, cfg.aspect);

		if(cfg.ortho)
		{
			guessProj = Matrix4f::Orthographic(cfg.position.nearPlane, cfg.position.farPlane);
		}
		
		guessProjInv = guessProj.Inverse();

		ModelViewProj = projMat.Mul(camMat.Mul(guessProjInv));
	}
	
	gl.glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, ModelViewProj.Data());
	gl.glUniform1ui(homogLoc, cfg.position.unproject);
	gl.glUniform2f(sizeLoc, 0.0f, 0.0f);
	
	if(!secondaryDraws.empty())
	{
		gl.glUniform4fv(colLoc, 1, &cfg.prevMeshColour.x);

		gl.glUniform1ui(fmtLoc, MESHDISPLAY_SOLID);
		
		gl.glPolygonMode(eGL_FRONT_AND_BACK, eGL_LINE);

		// secondary draws have to come from gl_Position which is float4
		gl.glVertexAttribFormat(0, 4, eGL_FLOAT, GL_FALSE, 0);
		gl.glEnableVertexAttribArray(0);
		gl.glDisableVertexAttribArray(1);

		for(size_t i=0; i < secondaryDraws.size(); i++)
		{
			const MeshFormat &fmt = secondaryDraws[i];

			if(fmt.buf != ResourceId())
			{
				GLuint vb = m_pDriver->GetResourceManager()->GetCurrentResource(fmt.buf).name;
				gl.glBindVertexBuffer(0, vb, fmt.offset, fmt.stride);

				GLenum topo = MakeGLPrimitiveTopology(fmt.topo);
				
				if(fmt.idxbuf != ResourceId())
				{
					GLuint ib = m_pDriver->GetResourceManager()->GetCurrentResource(fmt.idxbuf).name;
					gl.glBindBuffer(eGL_ELEMENT_ARRAY_BUFFER, ib);

					GLenum idxtype = eGL_UNSIGNED_BYTE;
					if(fmt.idxByteWidth == 2)
						idxtype = eGL_UNSIGNED_SHORT;
					else if(fmt.idxByteWidth == 4)
						idxtype = eGL_UNSIGNED_INT;

					gl.glDrawElements(topo, fmt.numVerts, idxtype, (const void *)(fmt.idxoffs));
				}
				else
				{
					gl.glDrawArrays(topo, 0, fmt.numVerts);
				}
			}
		}
	}

	for(uint32_t i=0; i < 2; i++)
	{
		if(fmts[i]->buf == ResourceId()) continue;

		if(fmts[i]->specialFormat != eSpecial_Unknown)
		{
			if(fmts[i]->specialFormat == eSpecial_R10G10B10A2)
			{
				if(fmts[i]->compType == eCompType_UInt)
					gl.glVertexAttribIFormat(i, 4, eGL_UNSIGNED_INT_2_10_10_10_REV, 0);
				if(fmts[i]->compType == eCompType_SInt)
					gl.glVertexAttribIFormat(i, 4, eGL_INT_2_10_10_10_REV, 0);
			}
			else if(fmts[i]->specialFormat == eSpecial_R11G11B10)
			{
				gl.glVertexAttribFormat(i, 4, eGL_UNSIGNED_INT_10F_11F_11F_REV, GL_FALSE, 0);
			}
			else
			{
				RDCWARN("Unsupported special vertex attribute format: %x", fmts[i]->specialFormat);
			}
		}
		else if(fmts[i]->compType == eCompType_Float ||
			fmts[i]->compType == eCompType_UNorm ||
			fmts[i]->compType == eCompType_SNorm)
		{
			GLenum fmttype = eGL_UNSIGNED_INT;

			if(fmts[i]->compByteWidth == 4)
			{
				if(fmts[i]->compType == eCompType_Float) fmttype = eGL_FLOAT;
				else if(fmts[i]->compType == eCompType_UNorm) fmttype = eGL_UNSIGNED_INT;
				else if(fmts[i]->compType == eCompType_SNorm) fmttype = eGL_INT;
			}
			else if(fmts[i]->compByteWidth == 2)
			{
				if(fmts[i]->compType == eCompType_Float) fmttype = eGL_HALF_FLOAT;
				else if(fmts[i]->compType == eCompType_UNorm) fmttype = eGL_UNSIGNED_SHORT;
				else if(fmts[i]->compType == eCompType_SNorm) fmttype = eGL_SHORT;
			}
			else if(fmts[i]->compByteWidth == 1)
			{
				if(fmts[i]->compType == eCompType_UNorm) fmttype = eGL_UNSIGNED_BYTE;
				else if(fmts[i]->compType == eCompType_SNorm) fmttype = eGL_BYTE;
			}

			gl.glVertexAttribFormat(i, fmts[i]->compCount, fmttype, fmts[i]->compType != eCompType_Float, 0);
		}
		else if(fmts[i]->compType == eCompType_UInt ||
			fmts[i]->compType == eCompType_SInt)
		{
			GLenum fmttype = eGL_UNSIGNED_INT;

			if(fmts[i]->compByteWidth == 4)
			{
				if(fmts[i]->compType == eCompType_UInt)  fmttype = eGL_UNSIGNED_INT;
				else if(fmts[i]->compType == eCompType_SInt)  fmttype = eGL_INT;
			}
			else if(fmts[i]->compByteWidth == 2)
			{
				if(fmts[i]->compType == eCompType_UInt)  fmttype = eGL_UNSIGNED_SHORT;
				else if(fmts[i]->compType == eCompType_SInt)  fmttype = eGL_SHORT;
			}
			else if(fmts[i]->compByteWidth == 1)
			{
				if(fmts[i]->compType == eCompType_UInt)  fmttype = eGL_UNSIGNED_BYTE;
				else if(fmts[i]->compType == eCompType_SInt)  fmttype = eGL_BYTE;
			}

			gl.glVertexAttribIFormat(i, fmts[i]->compCount, fmttype, 0);
		}
		else if(fmts[i]->compType == eCompType_Double)
		{
			gl.glVertexAttribLFormat(i, fmts[i]->compCount, eGL_DOUBLE, 0);
		}

		GLuint vb = m_pDriver->GetResourceManager()->GetCurrentResource(fmts[i]->buf).name;
		gl.glBindVertexBuffer(i, vb, fmts[i]->offset, fmts[i]->stride);
	}

	// enable position attribute
	gl.glEnableVertexAttribArray(0);
	gl.glDisableVertexAttribArray(1);

	// solid render
	if(cfg.solidShadeMode != eShade_None && topo != eGL_PATCHES)
	{
		gl.glEnable(eGL_DEPTH_TEST);
		gl.glDepthFunc(eGL_LESS);

		if(cfg.solidShadeMode == eShade_Lit)
		{
			GLint invProjLoc = gl.glGetUniformLocation(prog, "InvProj");

			Matrix4f InvProj = projMat.Inverse();

			gl.glUniformMatrix4fv(invProjLoc, 1, GL_FALSE, InvProj.Data());
		}

		if(cfg.second.buf != ResourceId())
			gl.glEnableVertexAttribArray(1);

		float wireCol[] = { 0.8f, 0.8f, 0.0f, 1.0f };
		gl.glUniform4fv(colLoc, 1, wireCol);
		
		GLint OutputDisplayFormat = (int)cfg.solidShadeMode;
		if(cfg.solidShadeMode == eShade_Secondary && cfg.second.showAlpha)
			OutputDisplayFormat = MESHDISPLAY_SECONDARY_ALPHA;
		gl.glUniform1ui(fmtLoc, OutputDisplayFormat);
		
		gl.glPolygonMode(eGL_FRONT_AND_BACK, eGL_FILL);

		if(cfg.position.idxbuf != ResourceId())
		{
			GLenum idxtype = eGL_UNSIGNED_BYTE;
			if(cfg.position.idxByteWidth == 2)
				idxtype = eGL_UNSIGNED_SHORT;
			else if(cfg.position.idxByteWidth == 4)
				idxtype = eGL_UNSIGNED_INT;

			GLuint ib = m_pDriver->GetResourceManager()->GetCurrentResource(cfg.position.idxbuf).name;
			gl.glBindBuffer(eGL_ELEMENT_ARRAY_BUFFER, ib);
			gl.glDrawElements(topo, cfg.position.numVerts, idxtype, (const void *)(cfg.position.idxoffs));
		}
		else
		{
			gl.glDrawArrays(topo, 0, cfg.position.numVerts);
		}

		gl.glDisableVertexAttribArray(1);
		
		if(cfg.solidShadeMode == eShade_Lit)
		{
			prog = DebugData.meshProg;

			colLoc = gl.glGetUniformLocation(prog, "RENDERDOC_GenericFS_Color");
			mvpLoc = gl.glGetUniformLocation(prog, "ModelViewProj");
			fmtLoc = gl.glGetUniformLocation(prog, "Mesh_DisplayFormat");

			gl.glUseProgram(prog);

			gl.glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, ModelViewProj.Data());
			gl.glUniform1ui(homogLoc, cfg.position.unproject);
			gl.glUniform2f(sizeLoc, 0.0f, 0.0f);
		}
	}
	
	gl.glDisable(eGL_DEPTH_TEST);

	// wireframe render
	if(cfg.solidShadeMode == eShade_None || cfg.wireframeDraw || topo == eGL_PATCHES)
	{
		float wireCol[] = { 0.0f, 0.0f, 0.0f, 1.0f };
		if(!secondaryDraws.empty())
		{
			wireCol[0] = cfg.currentMeshColour.x;
			wireCol[1] = cfg.currentMeshColour.y;
			wireCol[2] = cfg.currentMeshColour.z;
		}
		gl.glUniform4fv(colLoc, 1, wireCol);

		gl.glUniform1ui(fmtLoc, MESHDISPLAY_SOLID);

		gl.glPolygonMode(eGL_FRONT_AND_BACK, eGL_LINE);

		if(cfg.position.idxbuf != ResourceId())
		{
			GLenum idxtype = eGL_UNSIGNED_BYTE;
			if(cfg.position.idxByteWidth == 2)
				idxtype = eGL_UNSIGNED_SHORT;
			else if(cfg.position.idxByteWidth == 4)
				idxtype = eGL_UNSIGNED_INT;

			GLuint ib = m_pDriver->GetResourceManager()->GetCurrentResource(cfg.position.idxbuf).name;
			gl.glBindBuffer(eGL_ELEMENT_ARRAY_BUFFER, ib);
			gl.glDrawElements(topo != eGL_PATCHES ? topo : eGL_POINTS, cfg.position.numVerts, idxtype, (const void *)(cfg.position.idxoffs));
		}
		else
		{
			gl.glDrawArrays(topo != eGL_PATCHES ? topo : eGL_POINTS, 0, cfg.position.numVerts);
		}
	}
	
	// draw axis helpers
	if(!cfg.position.unproject)
	{
		gl.glBindVertexArray(DebugData.axisVAO);

		Vec4f wireCol(1.0f, 0.0f, 0.0f, 1.0f);
		gl.glUniform4fv(colLoc, 1, &wireCol.x);
		gl.glDrawArrays(eGL_LINES, 0, 2);

		wireCol = Vec4f(0.0f, 1.0f, 0.0f, 1.0f);
		gl.glUniform4fv(colLoc, 1, &wireCol.x);
		gl.glDrawArrays(eGL_LINES, 2, 2);

		wireCol = Vec4f(0.0f, 0.0f, 1.0f, 1.0f);
		gl.glUniform4fv(colLoc, 1, &wireCol.x);
		gl.glDrawArrays(eGL_LINES, 4, 2);
	}
	
	// 'fake' helper frustum
	if(cfg.position.unproject)
	{
		gl.glBindVertexArray(DebugData.frustumVAO);
		
		float wireCol[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		gl.glUniform4fv(colLoc, 1, wireCol);

		gl.glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, ModelViewProj.Data());
		
		gl.glDrawArrays(eGL_LINES, 0, 24);
	}

	gl.glPolygonMode(eGL_FRONT_AND_BACK, eGL_FILL);
	
	// show highlighted vertex
	if(cfg.highlightVert != ~0U)
	{
		MeshDataStage stage = cfg.type;
		
		if(m_HighlightCache.EID != eventID || stage != m_HighlightCache.stage)
		{
			m_HighlightCache.EID = eventID;
			m_HighlightCache.stage = stage;
			
			UINT bytesize = cfg.position.idxByteWidth; 

			m_HighlightCache.data = GetBufferData(cfg.position.buf, 0, 0);

			if(cfg.position.idxbuf == ResourceId() || stage == eMeshDataStage_GSOut)
			{
				m_HighlightCache.indices.clear();
				m_HighlightCache.useidx = false;
			}
			else
			{
				m_HighlightCache.useidx = true;

				vector<byte> idxdata = GetBufferData(cfg.position.idxbuf, cfg.position.idxoffs, cfg.position.numVerts*bytesize);

				uint8_t *idx8 = (uint8_t *)&idxdata[0];
				uint16_t *idx16 = (uint16_t *)&idxdata[0];
				uint32_t *idx32 = (uint32_t *)&idxdata[0];

				uint32_t numIndices = RDCMIN(cfg.position.numVerts, uint32_t(idxdata.size()/bytesize));

				m_HighlightCache.indices.resize(numIndices);

				if(bytesize == 1)
				{
					for(uint32_t i=0; i < numIndices; i++)
						m_HighlightCache.indices[i] = uint32_t(idx8[i]);
				}
				else if(bytesize == 2)
				{
					for(uint32_t i=0; i < numIndices; i++)
						m_HighlightCache.indices[i] = uint32_t(idx16[i]);
				}
				else if(bytesize == 4)
				{
					for(uint32_t i=0; i < numIndices; i++)
						m_HighlightCache.indices[i] = idx32[i];
				}
			}
		}

		GLenum meshtopo = topo;

		uint32_t idx = cfg.highlightVert;

		byte *data = &m_HighlightCache.data[0]; // buffer start
		byte *dataEnd = data + m_HighlightCache.data.size();

		data += cfg.position.offset; // to start of position data
		
		///////////////////////////////////////////////////////////////
		// vectors to be set from buffers, depending on topology

		bool valid = true;

		// this vert (blue dot, required)
		FloatVector activeVertex;
		 
		// primitive this vert is a part of (red prim, optional)
		vector<FloatVector> activePrim;

		// for patch lists, to show other verts in patch (green dots, optional)
		// for non-patch lists, we use the activePrim and adjacentPrimVertices
		// to show what other verts are related
		vector<FloatVector> inactiveVertices;

		// adjacency (line or tri, strips or lists) (green prims, optional)
		// will be N*M long, N adjacent prims of M verts each. M = primSize below
		vector<FloatVector> adjacentPrimVertices; 

		GLenum primTopo = eGL_TRIANGLES;
		uint32_t primSize = 3; // number of verts per primitive
		
		if(meshtopo == eGL_LINES ||
		   meshtopo == eGL_LINES_ADJACENCY ||
		   meshtopo == eGL_LINE_STRIP ||
		   meshtopo == eGL_LINE_STRIP_ADJACENCY)
		{
			primSize = 2;
			primTopo = eGL_LINES;
		}
		
		activeVertex = InterpretVertex(data, idx, cfg, dataEnd, valid);

		// see Section 10.1 of the OpenGL 4.5 spec for
		// how primitive topologies are laid out
		if(meshtopo == eGL_LINES)
		{
			uint32_t v = uint32_t(idx/2) * 2; // find first vert in primitive

			activePrim.push_back(InterpretVertex(data, v+0, cfg, dataEnd, valid));
			activePrim.push_back(InterpretVertex(data, v+1, cfg, dataEnd, valid));
		}
		else if(meshtopo == eGL_TRIANGLES)
		{
			uint32_t v = uint32_t(idx/3) * 3; // find first vert in primitive

			activePrim.push_back(InterpretVertex(data, v+0, cfg, dataEnd, valid));
			activePrim.push_back(InterpretVertex(data, v+1, cfg, dataEnd, valid));
			activePrim.push_back(InterpretVertex(data, v+2, cfg, dataEnd, valid));
		}
		else if(meshtopo == eGL_LINES_ADJACENCY)
		{
			uint32_t v = uint32_t(idx/4) * 4; // find first vert in primitive
			
			FloatVector vs[] = {
				InterpretVertex(data, v+0, cfg, dataEnd, valid),
				InterpretVertex(data, v+1, cfg, dataEnd, valid),
				InterpretVertex(data, v+2, cfg, dataEnd, valid),
				InterpretVertex(data, v+3, cfg, dataEnd, valid),
			};

			adjacentPrimVertices.push_back(vs[0]);
			adjacentPrimVertices.push_back(vs[1]);

			adjacentPrimVertices.push_back(vs[2]);
			adjacentPrimVertices.push_back(vs[3]);

			activePrim.push_back(vs[1]);
			activePrim.push_back(vs[2]);
		}
		else if(meshtopo == eGL_TRIANGLES_ADJACENCY)
		{
			uint32_t v = uint32_t(idx/6) * 6; // find first vert in primitive
			
			FloatVector vs[] = {
				InterpretVertex(data, v+0, cfg, dataEnd, valid),
				InterpretVertex(data, v+1, cfg, dataEnd, valid),
				InterpretVertex(data, v+2, cfg, dataEnd, valid),
				InterpretVertex(data, v+3, cfg, dataEnd, valid),
				InterpretVertex(data, v+4, cfg, dataEnd, valid),
				InterpretVertex(data, v+5, cfg, dataEnd, valid),
			};

			adjacentPrimVertices.push_back(vs[0]);
			adjacentPrimVertices.push_back(vs[1]);
			adjacentPrimVertices.push_back(vs[2]);
			
			adjacentPrimVertices.push_back(vs[2]);
			adjacentPrimVertices.push_back(vs[3]);
			adjacentPrimVertices.push_back(vs[4]);
			
			adjacentPrimVertices.push_back(vs[4]);
			adjacentPrimVertices.push_back(vs[5]);
			adjacentPrimVertices.push_back(vs[0]);

			activePrim.push_back(vs[0]);
			activePrim.push_back(vs[2]);
			activePrim.push_back(vs[4]);
		}
		else if(meshtopo == eGL_LINE_STRIP)
		{
			// find first vert in primitive. In strips a vert isn't
			// in only one primitive, so we pick the first primitive
			// it's in. This means the first N points are in the first
			// primitive, and thereafter each point is in the next primitive
			uint32_t v = RDCMAX(idx, 1U) - 1;
			
			activePrim.push_back(InterpretVertex(data, v+0, cfg, dataEnd, valid));
			activePrim.push_back(InterpretVertex(data, v+1, cfg, dataEnd, valid));
		}
		else if(meshtopo == eGL_TRIANGLE_STRIP)
		{
			// find first vert in primitive. In strips a vert isn't
			// in only one primitive, so we pick the first primitive
			// it's in. This means the first N points are in the first
			// primitive, and thereafter each point is in the next primitive
			uint32_t v = RDCMAX(idx, 2U) - 2;
			
			activePrim.push_back(InterpretVertex(data, v+0, cfg, dataEnd, valid));
			activePrim.push_back(InterpretVertex(data, v+1, cfg, dataEnd, valid));
			activePrim.push_back(InterpretVertex(data, v+2, cfg, dataEnd, valid));
		}
		else if(meshtopo == eGL_LINE_STRIP_ADJACENCY)
		{
			// find first vert in primitive. In strips a vert isn't
			// in only one primitive, so we pick the first primitive
			// it's in. This means the first N points are in the first
			// primitive, and thereafter each point is in the next primitive
			uint32_t v = RDCMAX(idx, 3U) - 3;
			
			FloatVector vs[] = {
				InterpretVertex(data, v+0, cfg, dataEnd, valid),
				InterpretVertex(data, v+1, cfg, dataEnd, valid),
				InterpretVertex(data, v+2, cfg, dataEnd, valid),
				InterpretVertex(data, v+3, cfg, dataEnd, valid),
			};

			adjacentPrimVertices.push_back(vs[0]);
			adjacentPrimVertices.push_back(vs[1]);

			adjacentPrimVertices.push_back(vs[2]);
			adjacentPrimVertices.push_back(vs[3]);

			activePrim.push_back(vs[1]);
			activePrim.push_back(vs[2]);
		}
		else if(meshtopo == eGL_TRIANGLE_STRIP_ADJACENCY)
		{
			// Triangle strip with adjacency is the most complex topology, as
			// we need to handle the ends separately where the pattern breaks.

			uint32_t numidx = cfg.position.numVerts;

			if(numidx < 6)
			{
				// not enough indices provided, bail to make sure logic below doesn't
				// need to have tons of edge case detection
				valid = false;
			}
			else if(idx <= 4 || numidx <= 7)
			{
				FloatVector vs[] = {
					InterpretVertex(data, 0, cfg, dataEnd, valid),
					InterpretVertex(data, 1, cfg, dataEnd, valid),
					InterpretVertex(data, 2, cfg, dataEnd, valid),
					InterpretVertex(data, 3, cfg, dataEnd, valid),
					InterpretVertex(data, 4, cfg, dataEnd, valid),

					// note this one isn't used as it's adjacency for the next triangle
					InterpretVertex(data, 5, cfg, dataEnd, valid),

					// min() with number of indices in case this is a tiny strip
					// that is basically just a list
					InterpretVertex(data, RDCMIN(6U, numidx-1), cfg, dataEnd, valid),
				};

				// these are the triangles on the far left of the MSDN diagram above
				adjacentPrimVertices.push_back(vs[0]);
				adjacentPrimVertices.push_back(vs[1]);
				adjacentPrimVertices.push_back(vs[2]);

				adjacentPrimVertices.push_back(vs[4]);
				adjacentPrimVertices.push_back(vs[3]);
				adjacentPrimVertices.push_back(vs[0]);

				adjacentPrimVertices.push_back(vs[4]);
				adjacentPrimVertices.push_back(vs[2]);
				adjacentPrimVertices.push_back(vs[6]);

				activePrim.push_back(vs[0]);
				activePrim.push_back(vs[2]);
				activePrim.push_back(vs[4]);
			}
			else if(idx > numidx-4)
			{
				// in diagram, numidx == 14

				FloatVector vs[] = {
					/*[0]=*/ InterpretVertex(data, numidx-8, cfg, dataEnd, valid), // 6 in diagram

					// as above, unused since this is adjacency for 2-previous triangle
					/*[1]=*/ InterpretVertex(data, numidx-7, cfg, dataEnd, valid), // 7 in diagram
					/*[2]=*/ InterpretVertex(data, numidx-6, cfg, dataEnd, valid), // 8 in diagram
					
					// as above, unused since this is adjacency for previous triangle
					/*[3]=*/ InterpretVertex(data, numidx-5, cfg, dataEnd, valid), // 9 in diagram
					/*[4]=*/ InterpretVertex(data, numidx-4, cfg, dataEnd, valid), // 10 in diagram
					/*[5]=*/ InterpretVertex(data, numidx-3, cfg, dataEnd, valid), // 11 in diagram
					/*[6]=*/ InterpretVertex(data, numidx-2, cfg, dataEnd, valid), // 12 in diagram
					/*[7]=*/ InterpretVertex(data, numidx-1, cfg, dataEnd, valid), // 13 in diagram
				};

				// these are the triangles on the far right of the MSDN diagram above
				adjacentPrimVertices.push_back(vs[2]); // 8 in diagram
				adjacentPrimVertices.push_back(vs[0]); // 6 in diagram
				adjacentPrimVertices.push_back(vs[4]); // 10 in diagram

				adjacentPrimVertices.push_back(vs[4]); // 10 in diagram
				adjacentPrimVertices.push_back(vs[7]); // 13 in diagram
				adjacentPrimVertices.push_back(vs[6]); // 12 in diagram

				adjacentPrimVertices.push_back(vs[6]); // 12 in diagram
				adjacentPrimVertices.push_back(vs[5]); // 11 in diagram
				adjacentPrimVertices.push_back(vs[2]); // 8 in diagram

				activePrim.push_back(vs[2]); // 8 in diagram
				activePrim.push_back(vs[4]); // 10 in diagram
				activePrim.push_back(vs[6]); // 12 in diagram
			}
			else
			{
				// we're in the middle somewhere. Each primitive has two vertices for it
				// so our step rate is 2. The first 'middle' primitive starts at indices 5&6
				// and uses indices all the way back to 0
				uint32_t v = RDCMAX( ( (idx+1) / 2) * 2, 6U) - 6;

				// these correspond to the indices in the MSDN diagram, with {2,4,6} as the
				// main triangle
				FloatVector vs[] = {
					InterpretVertex(data, v+0, cfg, dataEnd, valid),

					// this one is adjacency for 2-previous triangle
					InterpretVertex(data, v+1, cfg, dataEnd, valid),
					InterpretVertex(data, v+2, cfg, dataEnd, valid),

					// this one is adjacency for previous triangle
					InterpretVertex(data, v+3, cfg, dataEnd, valid),
					InterpretVertex(data, v+4, cfg, dataEnd, valid),
					InterpretVertex(data, v+5, cfg, dataEnd, valid),
					InterpretVertex(data, v+6, cfg, dataEnd, valid),
					InterpretVertex(data, v+7, cfg, dataEnd, valid),
					InterpretVertex(data, v+8, cfg, dataEnd, valid),
				};

				// these are the triangles around {2,4,6} in the MSDN diagram above
				adjacentPrimVertices.push_back(vs[0]);
				adjacentPrimVertices.push_back(vs[2]);
				adjacentPrimVertices.push_back(vs[4]);

				adjacentPrimVertices.push_back(vs[2]);
				adjacentPrimVertices.push_back(vs[5]);
				adjacentPrimVertices.push_back(vs[6]);

				adjacentPrimVertices.push_back(vs[6]);
				adjacentPrimVertices.push_back(vs[8]);
				adjacentPrimVertices.push_back(vs[4]);

				activePrim.push_back(vs[2]);
				activePrim.push_back(vs[4]);
				activePrim.push_back(vs[6]);
			}
		}
		else if(meshtopo == eGL_PATCHES)
		{
			uint32_t dim = (cfg.position.topo - eTopology_PatchList_1CPs + 1);

			uint32_t v0 = uint32_t(idx/dim) * dim;

			for(uint32_t v = v0; v < v0+dim; v++)
			{
				if(v != idx && valid)
					inactiveVertices.push_back(InterpretVertex(data, v, cfg, dataEnd, valid));
			}
		}
		else // if(meshtopo == eGL_POINTS) point list, or unknown/unhandled type
		{
			// no adjacency, inactive verts or active primitive
		}

		if(valid)
		{
			////////////////////////////////////////////////////////////////
			// prepare rendering (for both vertices & primitives)

			prog = DebugData.meshProg;

			gl.glUseProgram(prog);
			
			colLoc = gl.glGetUniformLocation(prog, "RENDERDOC_GenericFS_Color");
			mvpLoc = gl.glGetUniformLocation(prog, "ModelViewProj");
			sizeLoc = gl.glGetUniformLocation(prog, "PointSpriteSize");
			homogLoc = gl.glGetUniformLocation(prog, "HomogenousInput");
			
			// if data is from post transform, it will be in clipspace
			if(cfg.position.unproject)
				ModelViewProj = projMat.Mul(camMat.Mul(guessProjInv));
			else
				ModelViewProj = projMat.Mul(camMat);
			
			gl.glUniform1ui(homogLoc, cfg.position.unproject);
			
			gl.glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, ModelViewProj.Data());
			
			gl.glBindVertexArray(DebugData.triHighlightVAO);

			////////////////////////////////////////////////////////////////
			// render primitives
			
			// Draw active primitive (red)
			Vec4f WireframeColour(1.0f, 0.0f, 0.0f, 1.0f);
			gl.glUniform4fv(colLoc, 1, &WireframeColour.x);

			if(activePrim.size() >= primSize)
			{
				gl.glBindBuffer(eGL_ARRAY_BUFFER, DebugData.triHighlightBuffer);
				gl.glBufferSubData(eGL_ARRAY_BUFFER, 0, sizeof(Vec4f)*primSize, &activePrim[0]);

				gl.glDrawArrays(primTopo, 0, primSize);
			}

			// Draw adjacent primitives (green)
			WireframeColour = Vec4f(0.0f, 1.0f, 0.0f, 1.0f);
			gl.glUniform4fv(colLoc, 1, &WireframeColour.x);

			if(adjacentPrimVertices.size() >= primSize && (adjacentPrimVertices.size() % primSize) == 0)
			{
				gl.glBindBuffer(eGL_ARRAY_BUFFER, DebugData.triHighlightBuffer);
				gl.glBufferSubData(eGL_ARRAY_BUFFER, 0, sizeof(Vec4f)*adjacentPrimVertices.size(), &adjacentPrimVertices[0]);
				
				gl.glDrawArrays(primTopo, 0, (GLsizei)adjacentPrimVertices.size());
			}

			////////////////////////////////////////////////////////////////
			// prepare to render dots
			float scale = 800.0f/float(DebugData.outHeight);
			float asp = float(DebugData.outWidth)/float(DebugData.outHeight);

			Vec2f SpriteSize = Vec2f(scale/asp, scale);
			gl.glUniform2fv(sizeLoc, 1, &SpriteSize.x);

			// Draw active vertex (blue)
			WireframeColour = Vec4f(0.0f, 0.0f, 1.0f, 1.0f);
			gl.glUniform4fv(colLoc, 1, &WireframeColour.x);

			FloatVector vertSprite[4] = {
				activeVertex,
				activeVertex,
				activeVertex,
				activeVertex,
			};
			
			gl.glBindBuffer(eGL_ARRAY_BUFFER, DebugData.triHighlightBuffer);
			gl.glBufferSubData(eGL_ARRAY_BUFFER, 0, sizeof(vertSprite), &vertSprite[0]);

			gl.glDrawArrays(eGL_TRIANGLE_STRIP, 0, 4);

			// Draw inactive vertices (green)
			WireframeColour = Vec4f(0.0f, 1.0f, 0.0f, 1.0f);
			gl.glUniform4fv(colLoc, 1, &WireframeColour.x);

			for(size_t i=0; i < inactiveVertices.size(); i++)
			{
				vertSprite[0] = vertSprite[1] = vertSprite[2] = vertSprite[3] = inactiveVertices[i];
				
				gl.glBufferSubData(eGL_ARRAY_BUFFER, 0, sizeof(vertSprite), &vertSprite[0]);
				
				gl.glDrawArrays(eGL_TRIANGLE_STRIP, 0, 4);
			}
		}
	}
}
