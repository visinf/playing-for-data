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

#include "gl_renderstate.h"

GLRenderState::GLRenderState(const GLHookSet *funcs, Serialiser *ser)
	: m_Real(funcs)
	, m_pSerialiser(ser)
{
	Clear();
}

GLRenderState::~GLRenderState()
{
}

void GLRenderState::FetchState()
{
	// TODO check GL_MAX_*

	m_Real->glGetIntegerv(eGL_ACTIVE_TEXTURE, (GLint *)&ActiveTexture);
	
	// TODO fetch bindings for other types than 2D
	for(int i=0; i < ARRAY_COUNT(Tex2D); i++)
	{
		m_Real->glActiveTexture(GLenum(eGL_TEXTURE0 + i));
		m_Real->glGetIntegerv(eGL_TEXTURE_BINDING_2D, (GLint*)&Tex2D[i]);
	}
	
	m_Real->glActiveTexture(ActiveTexture);

	m_Real->glGetIntegerv(eGL_ARRAY_BUFFER_BINDING,              (GLint*)&BufferBindings[0]);
	m_Real->glGetIntegerv(eGL_COPY_READ_BUFFER_BINDING,          (GLint*)&BufferBindings[1]);
	m_Real->glGetIntegerv(eGL_COPY_WRITE_BUFFER_BINDING,         (GLint*)&BufferBindings[2]);
	m_Real->glGetIntegerv(eGL_DRAW_INDIRECT_BUFFER_BINDING,      (GLint*)&BufferBindings[3]);
	m_Real->glGetIntegerv(eGL_DISPATCH_INDIRECT_BUFFER_BINDING,  (GLint*)&BufferBindings[4]);
	m_Real->glGetIntegerv(eGL_ELEMENT_ARRAY_BUFFER_BINDING,      (GLint*)&BufferBindings[5]);
	m_Real->glGetIntegerv(eGL_PIXEL_PACK_BUFFER_BINDING,         (GLint*)&BufferBindings[6]);
	m_Real->glGetIntegerv(eGL_PIXEL_UNPACK_BUFFER_BINDING,       (GLint*)&BufferBindings[7]);
	m_Real->glGetIntegerv(eGL_QUERY_BUFFER_BINDING,              (GLint*)&BufferBindings[8]);
	m_Real->glGetIntegerv(eGL_TEXTURE_BUFFER_BINDING,            (GLint*)&BufferBindings[9]);

	struct { IdxRangeBuffer *bufs; int count; GLenum binding; GLenum start; GLenum size; } idxBufs[] =
	{
		{ AtomicCounter, ARRAY_COUNT(AtomicCounter), eGL_ATOMIC_COUNTER_BUFFER_BINDING, },
		{ ShaderStorage, ARRAY_COUNT(ShaderStorage), eGL_SHADER_STORAGE_BUFFER_BINDING, },
		{ TransformFeedback, ARRAY_COUNT(TransformFeedback), eGL_TRANSFORM_FEEDBACK_BUFFER_BINDING, },
		{ UniformBinding, ARRAY_COUNT(UniformBinding), eGL_UNIFORM_BUFFER_BINDING, },
	};

	for(int b=0; b < ARRAY_COUNT(idxBufs); b++)
	{
		for(int i=0; i < idxBufs[b].count; i++)
		{
			m_Real->glGetIntegeri_v(idxBufs[b].binding, i, (GLint*)&idxBufs[b].bufs[i].name);
			m_Real->glGetInteger64i_v(idxBufs[b].start, i, (GLint64*)&idxBufs[b].bufs[i].start);
			m_Real->glGetInteger64i_v(idxBufs[b].size,  i, (GLint64*)&idxBufs[b].bufs[i].size);
		}
	}
	
	for(int i=0; i < ARRAY_COUNT(Blends); i++)
	{
		m_Real->glGetIntegeri_v(eGL_BLEND_EQUATION_RGB, i, (GLint*)Blends[i].EquationRGB);
		m_Real->glGetIntegeri_v(eGL_BLEND_EQUATION_ALPHA, i, (GLint*)Blends[i].EquationAlpha);

		m_Real->glGetIntegeri_v(eGL_BLEND_SRC_RGB, i, (GLint*)Blends[i].SourceRGB);
		m_Real->glGetIntegeri_v(eGL_BLEND_SRC_ALPHA, i, (GLint*)Blends[i].SourceAlpha);

		m_Real->glGetIntegeri_v(eGL_BLEND_DST_RGB, i, (GLint*)Blends[i].DestinationRGB);
		m_Real->glGetIntegeri_v(eGL_BLEND_DST_ALPHA, i, (GLint*)Blends[i].DestinationAlpha);
	}

	m_Real->glGetFloatv(eGL_BLEND_COLOR, &BlendColor[0]);
}

void GLRenderState::ApplyState()
{
	for(int i=0; i < ARRAY_COUNT(Tex2D); i++)
	{
		m_Real->glActiveTexture(GLenum(eGL_TEXTURE0 + i));
		m_Real->glBindTexture(eGL_TEXTURE_2D, Tex2D[i]);
	}
	
	m_Real->glActiveTexture(ActiveTexture);

	m_Real->glBindBuffer(eGL_ARRAY_BUFFER,              BufferBindings[0]);
	m_Real->glBindBuffer(eGL_COPY_READ_BUFFER,          BufferBindings[1]);
	m_Real->glBindBuffer(eGL_COPY_WRITE_BUFFER,         BufferBindings[2]);
	m_Real->glBindBuffer(eGL_DRAW_INDIRECT_BUFFER,      BufferBindings[3]);
	m_Real->glBindBuffer(eGL_DISPATCH_INDIRECT_BUFFER,  BufferBindings[4]);
	m_Real->glBindBuffer(eGL_ELEMENT_ARRAY_BUFFER,      BufferBindings[5]);
	m_Real->glBindBuffer(eGL_PIXEL_PACK_BUFFER,         BufferBindings[6]);
	m_Real->glBindBuffer(eGL_PIXEL_UNPACK_BUFFER,       BufferBindings[7]);
	m_Real->glBindBuffer(eGL_QUERY_BUFFER,              BufferBindings[8]);
	m_Real->glBindBuffer(eGL_TEXTURE_BUFFER,            BufferBindings[9]);

	struct { IdxRangeBuffer *bufs; int count; GLenum binding; } idxBufs[] =
	{
		{ AtomicCounter, ARRAY_COUNT(AtomicCounter), eGL_ATOMIC_COUNTER_BUFFER, },
		{ ShaderStorage, ARRAY_COUNT(ShaderStorage), eGL_SHADER_STORAGE_BUFFER, },
		{ TransformFeedback, ARRAY_COUNT(TransformFeedback), eGL_TRANSFORM_FEEDBACK_BUFFER, },
		{ UniformBinding, ARRAY_COUNT(UniformBinding), eGL_UNIFORM_BUFFER, },
	};

	for(int b=0; b < ARRAY_COUNT(idxBufs); b++)
		for(int i=0; i < idxBufs[b].count; i++)
			m_Real->glBindBufferRange(idxBufs[b].binding, i, idxBufs[b].bufs[i].name, (GLintptr)idxBufs[b].bufs[i].start, (GLsizeiptr)idxBufs[b].bufs[i].size);
	
	for(int i=0; i < ARRAY_COUNT(Blends); i++)
	{
		m_Real->glBlendFuncSeparatei(i, Blends[i].SourceRGB, Blends[i].DestinationRGB, Blends[i].DestinationRGB, Blends[i].DestinationAlpha);
		m_Real->glBlendEquationSeparatei(i, Blends[i].EquationRGB, Blends[i].EquationAlpha);
	}

	m_Real->glBlendColor(BlendColor[0], BlendColor[1], BlendColor[2], BlendColor[3]);
}

void GLRenderState::Clear()
{
	RDCEraseEl(Tex2D);
	RDCEraseEl(ActiveTexture);
	RDCEraseEl(BufferBindings);
	RDCEraseEl(AtomicCounter);
	RDCEraseEl(ShaderStorage);
	RDCEraseEl(TransformFeedback);
	RDCEraseEl(UniformBinding);
	RDCEraseEl(Blends);
	RDCEraseEl(BlendColor);
}

void GLRenderState::Serialise(LogState state, GLResourceManager *rm)
{
	// TODO check GL_MAX_*

	{
		ResourceId IDs[128];
		m_pSerialiser->Serialise<128>("GL_TEXTURE_BINDING_2D", IDs);
	}

	m_pSerialiser->Serialise("GL_ACTIVE_TEXTURE", ActiveTexture);
	
	for(int i=0; i < ARRAY_COUNT(BufferBindings); i++)
	{
		ResourceId ID = ResourceId();
		if(state >= WRITING) ID = rm->GetID(BufferRes(BufferBindings[i]));
		m_pSerialiser->Serialise("GL_BUFFER_BINDING", ID);
		if(state < WRITING) BufferBindings[i] = rm->GetLiveResource(ID).name;
	}
	
	struct { IdxRangeBuffer *bufs; int count; } idxBufs[] =
	{
		{ AtomicCounter, ARRAY_COUNT(AtomicCounter), },
		{ ShaderStorage, ARRAY_COUNT(ShaderStorage), },
		{ TransformFeedback, ARRAY_COUNT(TransformFeedback), },
		{ UniformBinding, ARRAY_COUNT(UniformBinding), },
	};

	for(int b=0; b < ARRAY_COUNT(idxBufs); b++)
	{
		for(int i=0; i < idxBufs[b].count; i++)
		{
			ResourceId ID = ResourceId();
			if(state >= WRITING) ID = rm->GetID(BufferRes(idxBufs[b].bufs[i].name));
			m_pSerialiser->Serialise("BUFFER_BINDING", ID);
			if(state < WRITING) idxBufs[b].bufs[i].name = rm->GetLiveResource(ID).name;

			m_pSerialiser->Serialise("BUFFER_START", idxBufs[b].bufs[i].start);
			m_pSerialiser->Serialise("BUFFER_SIZE", idxBufs[b].bufs[i].size);
		}
	}
	
	for(int i=0; i < ARRAY_COUNT(Blends); i++)
	{
		m_pSerialiser->Serialise("GL_BLEND_EQUATION_RGB", Blends[i].EquationRGB);
		m_pSerialiser->Serialise("GL_BLEND_EQUATION_ALPHA", Blends[i].EquationAlpha);

		m_pSerialiser->Serialise("GL_BLEND_SRC_RGB", Blends[i].SourceRGB);
		m_pSerialiser->Serialise("GL_BLEND_SRC_ALPHA", Blends[i].SourceAlpha);

		m_pSerialiser->Serialise("GL_BLEND_DST_RGB", Blends[i].DestinationRGB);
		m_pSerialiser->Serialise("GL_BLEND_DST_ALPHA", Blends[i].DestinationAlpha);
	}
	
	m_pSerialiser->Serialise<4>("GL_BLEND_COLOR", BlendColor);
}
