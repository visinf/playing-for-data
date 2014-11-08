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

#include "common/common.h"
#include "common/string_utils.h"
#include "../gl_driver.h"

bool WrappedOpenGL::Serialise_glDispatchCompute(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z)
{
	SERIALISE_ELEMENT(uint32_t, X, num_groups_x);
	SERIALISE_ELEMENT(uint32_t, Y, num_groups_y);
	SERIALISE_ELEMENT(uint32_t, Z, num_groups_z);
	
	if(m_State <= EXECUTING)
	{
		m_Real.glDispatchCompute(X, Y, Z);
	}
	
	const string desc = m_pSerialiser->GetDebugStr();
	
	vector<DebugMessage> debugMessages = Serialise_DebugMessages();

	if(m_State == READING)
	{
		AddEvent(DISPATCH_COMPUTE, desc);
		string name = "glDispatchCompute(" +
						ToStr::Get(X) + ", " +
						ToStr::Get(Y) + ", " +
						ToStr::Get(Z) + ")";

		FetchDrawcall draw;
		draw.name = widen(name);
		draw.flags |= eDraw_Dispatch;

		draw.debugMessages = debugMessages;

		AddDrawcall(draw, true);
	}

	return true;
}

void WrappedOpenGL::glDispatchCompute(GLuint num_groups_x, GLuint num_groups_y, GLuint num_groups_z)
{
	m_Real.glDispatchCompute(num_groups_x, num_groups_y, num_groups_z);

	if(m_State == WRITING_CAPFRAME)
	{
		SCOPED_SERIALISE_CONTEXT(DISPATCH_COMPUTE);
		Serialise_glDispatchCompute(num_groups_x, num_groups_y, num_groups_z);

		m_ContextRecord->AddChunk(scope.Get());
	}
}

bool WrappedOpenGL::Serialise_glDispatchComputeIndirect(GLintptr indirect)
{
	SERIALISE_ELEMENT(uint64_t, offs, indirect);
	
	if(m_State <= EXECUTING)
	{
		m_Real.glDispatchComputeIndirect((GLintptr)offs);
	}
	
	const string desc = m_pSerialiser->GetDebugStr();
	
	vector<DebugMessage> debugMessages = Serialise_DebugMessages();

	if(m_State == READING)
	{
		AddEvent(DISPATCH_COMPUTE_INDIRECT, desc);
		string name = "glDispatchComputeIndirect(" +
						ToStr::Get(0) + ", " +
						ToStr::Get(0) + ", " +
						ToStr::Get(0) + ")";
		
		RDCUNIMPLEMENTED("Not fetching indirect data for glDispatchComputeIndirect() display");

		FetchDrawcall draw;
		draw.name = widen(name);
		draw.flags |= eDraw_Dispatch;

		draw.debugMessages = debugMessages;

		AddDrawcall(draw, true);
	}

	return true;
}

void WrappedOpenGL::glDispatchComputeIndirect(GLintptr indirect)
{
	m_Real.glDispatchComputeIndirect(indirect);

	if(m_State == WRITING_CAPFRAME)
	{
		SCOPED_SERIALISE_CONTEXT(DISPATCH_COMPUTE_INDIRECT);
		Serialise_glDispatchComputeIndirect(indirect);

		m_ContextRecord->AddChunk(scope.Get());
	}
}

bool WrappedOpenGL::Serialise_glMemoryBarrier(GLbitfield barriers)
{
	SERIALISE_ELEMENT(uint32_t, Barriers, barriers);
	
	if(m_State <= EXECUTING)
	{
		m_Real.glMemoryBarrier(Barriers);
	}
	
	return true;
}

void WrappedOpenGL::glMemoryBarrier(GLbitfield barriers)
{
	m_Real.glMemoryBarrier(barriers);

	if(m_State == WRITING_CAPFRAME)
	{
		SCOPED_SERIALISE_CONTEXT(MEMORY_BARRIER);
		Serialise_glMemoryBarrier(barriers);

		m_ContextRecord->AddChunk(scope.Get());
	}
}

bool WrappedOpenGL::Serialise_glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
	SERIALISE_ELEMENT(GLenum, Mode, mode);
	SERIALISE_ELEMENT(int32_t, First, first);
	SERIALISE_ELEMENT(uint32_t, Count, count);

	if(m_State <= EXECUTING)
	{
		m_Real.glDrawArrays(Mode, First, Count);
	}
	
	const string desc = m_pSerialiser->GetDebugStr();
	
	vector<DebugMessage> debugMessages = Serialise_DebugMessages();

	if(m_State == READING)
	{
		AddEvent(DRAWARRAYS, desc);
		string name = "glDrawArrays(" +
						ToStr::Get(Mode) + ", " +
						ToStr::Get(First) + ", " +
						ToStr::Get(Count) + ")";

		FetchDrawcall draw;
		draw.name = widen(name);
		draw.numIndices = Count;
		draw.numInstances = 1;
		draw.indexOffset = 0;
		draw.vertexOffset = First;
		draw.instanceOffset = 0;

		draw.flags |= eDraw_Drawcall;

		draw.debugMessages = debugMessages;

		m_LastDrawMode = Mode;

		AddDrawcall(draw, true);
	}

	return true;
}

void WrappedOpenGL::glDrawArrays(GLenum mode, GLint first, GLsizei count)
{
	m_Real.glDrawArrays(mode, first, count);

	if(m_State == WRITING_CAPFRAME)
	{
		SCOPED_SERIALISE_CONTEXT(DRAWARRAYS);
		Serialise_glDrawArrays(mode, first, count);

		m_ContextRecord->AddChunk(scope.Get());
	}
}

bool WrappedOpenGL::Serialise_glDrawArraysIndirect(GLenum mode, const void *indirect)
{
	SERIALISE_ELEMENT(GLenum, Mode, mode);
	SERIALISE_ELEMENT(uint64_t, Offset, (uint64_t)indirect);

	if(m_State <= EXECUTING)
	{
		m_Real.glDrawArraysIndirect(Mode, (const void *)Offset);
	}
	
	const string desc = m_pSerialiser->GetDebugStr();
	
	vector<DebugMessage> debugMessages = Serialise_DebugMessages();

	if(m_State == READING)
	{
		AddEvent(DRAWARRAYS_INDIRECT, desc);
		string name = "glDrawArraysIndirect(" +
						ToStr::Get(Mode) + ")";
		
		RDCUNIMPLEMENTED("Not fetching indirect data for glDrawArraysIndirect() display");

		FetchDrawcall draw;
		draw.name = widen(name);
		draw.numIndices = 1;
		draw.numInstances = 1;
		draw.indexOffset = 0;
		draw.vertexOffset = 0;
		draw.instanceOffset = 0;

		draw.flags |= eDraw_Drawcall;
		
		draw.debugMessages = debugMessages;

		m_LastDrawMode = Mode;

		AddDrawcall(draw, true);
	}

	return true;
}

void WrappedOpenGL::glDrawArraysIndirect(GLenum mode, const void *indirect)
{
	m_Real.glDrawArraysIndirect(mode, indirect);

	if(m_State == WRITING_CAPFRAME)
	{
		SCOPED_SERIALISE_CONTEXT(DRAWARRAYS_INDIRECT);
		Serialise_glDrawArraysIndirect(mode, indirect);

		m_ContextRecord->AddChunk(scope.Get());
	}
}

bool WrappedOpenGL::Serialise_glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instancecount)
{
	SERIALISE_ELEMENT(GLenum, Mode, mode);
	SERIALISE_ELEMENT(int32_t, First, first);
	SERIALISE_ELEMENT(uint32_t, Count, count);
	SERIALISE_ELEMENT(uint32_t, InstanceCount, instancecount);

	if(m_State <= EXECUTING)
	{
		m_Real.glDrawArraysInstanced(Mode, First, Count, InstanceCount);
	}
	
	const string desc = m_pSerialiser->GetDebugStr();
	
	vector<DebugMessage> debugMessages = Serialise_DebugMessages();

	if(m_State == READING)
	{
		AddEvent(DRAWARRAYS_INSTANCED, desc);
		string name = "glDrawArraysInstanced(" +
						ToStr::Get(Mode) + ", " +
						ToStr::Get(First) + ", " +
						ToStr::Get(Count) + ", " +
						ToStr::Get(InstanceCount) + ")";

		FetchDrawcall draw;
		draw.name = widen(name);
		draw.numIndices = Count;
		draw.numInstances = InstanceCount;
		draw.indexOffset = 0;
		draw.vertexOffset = First;
		draw.instanceOffset = 0;

		draw.flags |= eDraw_Drawcall|eDraw_Instanced;
		
		draw.debugMessages = debugMessages;

		m_LastDrawMode = Mode;

		AddDrawcall(draw, true);
	}

	return true;
}

void WrappedOpenGL::glDrawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instancecount)
{
	m_Real.glDrawArraysInstanced(mode, first, count, instancecount);

	if(m_State == WRITING_CAPFRAME)
	{
		SCOPED_SERIALISE_CONTEXT(DRAWARRAYS_INSTANCED);
		Serialise_glDrawArraysInstanced(mode, first, count, instancecount);

		m_ContextRecord->AddChunk(scope.Get());
	}
}

bool WrappedOpenGL::Serialise_glDrawArraysInstancedBaseInstance(GLenum mode, GLint first, GLsizei count, GLsizei instancecount, GLuint baseinstance)
{
	SERIALISE_ELEMENT(GLenum, Mode, mode);
	SERIALISE_ELEMENT(int32_t, First, first);
	SERIALISE_ELEMENT(uint32_t, Count, count);
	SERIALISE_ELEMENT(uint32_t, InstanceCount, instancecount);
	SERIALISE_ELEMENT(uint32_t, BaseInstance, baseinstance);

	if(m_State <= EXECUTING)
	{
		m_Real.glDrawArraysInstancedBaseInstance(Mode, First, Count, InstanceCount, BaseInstance);
	}
	
	const string desc = m_pSerialiser->GetDebugStr();
	
	vector<DebugMessage> debugMessages = Serialise_DebugMessages();

	if(m_State == READING)
	{
		AddEvent(DRAWARRAYS_INSTANCEDBASEINSTANCE, desc);
		string name = "glDrawArraysInstancedBaseInstance(" +
						ToStr::Get(Mode) + ", " +
						ToStr::Get(First) + ", " +
						ToStr::Get(Count) + ", " +
						ToStr::Get(InstanceCount) + ", " +
						ToStr::Get(BaseInstance) + ")";

		FetchDrawcall draw;
		draw.name = widen(name);
		draw.numIndices = Count;
		draw.numInstances = InstanceCount;
		draw.indexOffset = 0;
		draw.vertexOffset = First;
		draw.instanceOffset = BaseInstance;

		draw.flags |= eDraw_Drawcall|eDraw_Instanced;
		
		draw.debugMessages = debugMessages;

		m_LastDrawMode = Mode;

		AddDrawcall(draw, true);
	}

	return true;
}

void WrappedOpenGL::glDrawArraysInstancedBaseInstance(GLenum mode, GLint first, GLsizei count, GLsizei instancecount, GLuint baseinstance)
{
	m_Real.glDrawArraysInstancedBaseInstance(mode, first, count, instancecount, baseinstance);

	if(m_State == WRITING_CAPFRAME)
	{
		SCOPED_SERIALISE_CONTEXT(DRAWARRAYS_INSTANCEDBASEINSTANCE);
		Serialise_glDrawArraysInstancedBaseInstance(mode, first, count, instancecount, baseinstance);

		m_ContextRecord->AddChunk(scope.Get());
	}
}

bool WrappedOpenGL::Serialise_glDrawElements(GLenum mode, GLsizei count, GLenum type, const void *indices)
{
	SERIALISE_ELEMENT(GLenum, Mode, mode);
	SERIALISE_ELEMENT(uint32_t, Count, count);
	SERIALISE_ELEMENT(GLenum, Type, type);
	SERIALISE_ELEMENT(uint64_t, IdxOffset, (uint64_t)indices);

	if(m_State <= EXECUTING)
	{
		m_Real.glDrawElements(Mode, Count, Type, (const void *)IdxOffset);
	}
	
	const string desc = m_pSerialiser->GetDebugStr();
	
	vector<DebugMessage> debugMessages = Serialise_DebugMessages();

	if(m_State == READING)
	{
		AddEvent(DRAWELEMENTS, desc);
		string name = "glDrawElements(" +
						ToStr::Get(Mode) + ", " +
						ToStr::Get(Count) + ", " +
						ToStr::Get(Type) + ", " +
						ToStr::Get(IdxOffset) + ")";

		FetchDrawcall draw;
		draw.name = widen(name);
		draw.numIndices = Count;
		draw.numInstances = 1;
		draw.indexOffset = (uint32_t)IdxOffset;
		draw.vertexOffset = 0;
		draw.instanceOffset = 0;

		draw.flags |= eDraw_Drawcall|eDraw_UseIBuffer;
		
		draw.debugMessages = debugMessages;

		m_LastDrawMode = Mode;
		m_LastIndexSize = Type;
		m_LastIndexOffset = (GLuint)IdxOffset;

		AddDrawcall(draw, true);
	}

	return true;
}

void WrappedOpenGL::glDrawElements(GLenum mode, GLsizei count, GLenum type, const void *indices)
{
	m_Real.glDrawElements(mode, count, type, indices);

	if(m_State == WRITING_CAPFRAME)
	{
		SCOPED_SERIALISE_CONTEXT(DRAWELEMENTS);
		Serialise_glDrawElements(mode, count, type, indices);

		m_ContextRecord->AddChunk(scope.Get());
	}
}

bool WrappedOpenGL::Serialise_glDrawElementsIndirect(GLenum mode, GLenum type, const void *indirect)
{
	SERIALISE_ELEMENT(GLenum, Mode, mode);
	SERIALISE_ELEMENT(GLenum, Type, type);
	SERIALISE_ELEMENT(uint64_t, Offset, (uint64_t)indirect);

	if(m_State <= EXECUTING)
	{
		m_Real.glDrawElementsIndirect(Mode, Type, (const void *)Offset);
	}
	
	const string desc = m_pSerialiser->GetDebugStr();
	
	vector<DebugMessage> debugMessages = Serialise_DebugMessages();

	if(m_State == READING)
	{
		AddEvent(DRAWELEMENTS_INDIRECT, desc);
		string name = "glDrawElementsIndirect(" +
						ToStr::Get(Mode) + ", " +
						ToStr::Get(Type) + ")";
		
		RDCUNIMPLEMENTED("Not fetching indirect data for glDrawElementsIndirect() display");

		FetchDrawcall draw;
		draw.name = widen(name);
		draw.numIndices = 1;
		draw.numInstances = 1;
		draw.indexOffset = 0;
		draw.vertexOffset = 0;
		draw.instanceOffset = 0;

		draw.flags |= eDraw_Drawcall|eDraw_UseIBuffer;
		
		draw.debugMessages = debugMessages;

		m_LastDrawMode = Mode;
		m_LastIndexSize = Type;
		m_LastIndexOffset = 0;

		AddDrawcall(draw, true);
	}

	return true;
}

void WrappedOpenGL::glDrawElementsIndirect(GLenum mode, GLenum type, const void *indirect)
{
	m_Real.glDrawElementsIndirect(mode, type, indirect);

	if(m_State == WRITING_CAPFRAME)
	{
		SCOPED_SERIALISE_CONTEXT(DRAWELEMENTS_INDIRECT);
		Serialise_glDrawElementsIndirect(mode, type, indirect);

		m_ContextRecord->AddChunk(scope.Get());
	}
}

bool WrappedOpenGL::Serialise_glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices)
{
	SERIALISE_ELEMENT(GLenum, Mode, mode);
	SERIALISE_ELEMENT(uint32_t, Start, start);
	SERIALISE_ELEMENT(uint32_t, End, end);
	SERIALISE_ELEMENT(uint32_t, Count, count);
	SERIALISE_ELEMENT(GLenum, Type, type);
	SERIALISE_ELEMENT(uint64_t, IdxOffset, (uint64_t)indices);

	if(m_State <= EXECUTING)
	{
		m_Real.glDrawRangeElements(Mode, Start, End, Count, Type, (const void *)IdxOffset);
	}
	
	const string desc = m_pSerialiser->GetDebugStr();
	
	vector<DebugMessage> debugMessages = Serialise_DebugMessages();

	if(m_State == READING)
	{
		AddEvent(DRAWELEMENTS, desc);
		string name = "glDrawRangeElements(" +
						ToStr::Get(Mode) + ", " +
						ToStr::Get(Count) + ", " +
						ToStr::Get(Type) + ", " +
						ToStr::Get(IdxOffset) + ")";

		FetchDrawcall draw;
		draw.name = widen(name);
		draw.numIndices = Count;
		draw.numInstances = 1;
		draw.indexOffset = (uint32_t)IdxOffset;
		draw.vertexOffset = 0;
		draw.instanceOffset = 0;

		draw.flags |= eDraw_Drawcall|eDraw_UseIBuffer;
		
		draw.debugMessages = debugMessages;

		m_LastDrawMode = Mode;
		m_LastIndexSize = Type;
		m_LastIndexOffset = (GLuint)IdxOffset;

		AddDrawcall(draw, true);
	}

	return true;
}

void WrappedOpenGL::glDrawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, const void *indices)
{
	m_Real.glDrawRangeElements(mode, start, end, count, type, indices);

	if(m_State == WRITING_CAPFRAME)
	{
		SCOPED_SERIALISE_CONTEXT(DRAWRANGEELEMENTS);
		Serialise_glDrawRangeElements(mode, start, end, count, type, indices);

		m_ContextRecord->AddChunk(scope.Get());
	}
}

bool WrappedOpenGL::Serialise_glDrawElementsBaseVertex(GLenum mode, GLsizei count, GLenum type, const void *indices, GLint basevertex)
{
	SERIALISE_ELEMENT(GLenum, Mode, mode);
	SERIALISE_ELEMENT(uint32_t, Count, count);
	SERIALISE_ELEMENT(GLenum, Type, type);
	SERIALISE_ELEMENT(uint64_t, IdxOffset, (uint64_t)indices);
	SERIALISE_ELEMENT(int32_t, BaseVtx, basevertex);

	if(m_State <= EXECUTING)
	{
		m_Real.glDrawElementsBaseVertex(Mode, Count, Type, (const void *)IdxOffset, BaseVtx);
	}
	
	const string desc = m_pSerialiser->GetDebugStr();
	
	vector<DebugMessage> debugMessages = Serialise_DebugMessages();

	if(m_State == READING)
	{
		AddEvent(DRAWELEMENTS_BASEVERTEX, desc);
		string name = "glDrawElementsBaseVertex(" +
						ToStr::Get(Mode) + ", " +
						ToStr::Get(Count) + ", " +
						ToStr::Get(Type) + ", " +
						ToStr::Get(IdxOffset) + ", " +
						ToStr::Get(BaseVtx) + ")";

		FetchDrawcall draw;
		draw.name = widen(name);
		draw.numIndices = Count;
		draw.numInstances = 1;
		draw.indexOffset = (uint32_t)IdxOffset;
		draw.vertexOffset = BaseVtx;
		draw.instanceOffset = 0;

		draw.flags |= eDraw_Drawcall|eDraw_UseIBuffer;
		
		draw.debugMessages = debugMessages;

		m_LastDrawMode = Mode;
		m_LastIndexSize = Type;
		m_LastIndexOffset = (GLuint)IdxOffset;

		AddDrawcall(draw, true);
	}

	return true;
}

void WrappedOpenGL::glDrawElementsBaseVertex(GLenum mode, GLsizei count, GLenum type, const void *indices, GLint basevertex)
{
	m_Real.glDrawElementsBaseVertex(mode, count, type, indices, basevertex);

	if(m_State == WRITING_CAPFRAME)
	{
		SCOPED_SERIALISE_CONTEXT(DRAWELEMENTS_BASEVERTEX);
		Serialise_glDrawElementsBaseVertex(mode, count, type, indices, basevertex);

		m_ContextRecord->AddChunk(scope.Get());
	}
}

bool WrappedOpenGL::Serialise_glDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount)
{
	SERIALISE_ELEMENT(GLenum, Mode, mode);
	SERIALISE_ELEMENT(uint32_t, Count, count);
	SERIALISE_ELEMENT(GLenum, Type, type);
	SERIALISE_ELEMENT(uint64_t, IdxOffset, (uint64_t)indices);
	SERIALISE_ELEMENT(uint32_t, InstCount, instancecount);

	if(m_State <= EXECUTING)
	{
		m_Real.glDrawElementsInstanced(Mode, Count, Type, (const void *)IdxOffset, InstCount);
	}
	
	const string desc = m_pSerialiser->GetDebugStr();
	
	vector<DebugMessage> debugMessages = Serialise_DebugMessages();

	if(m_State == READING)
	{
		AddEvent(DRAWELEMENTS_INSTANCED, desc);
		string name = "glDrawElementsInstanced(" +
						ToStr::Get(Mode) + ", " +
						ToStr::Get(Count) + ", " +
						ToStr::Get(Type) + ", " +
						ToStr::Get(IdxOffset) + ", " +
						ToStr::Get(InstCount) + ")";

		FetchDrawcall draw;
		draw.name = widen(name);
		draw.numIndices = Count;
		draw.numInstances = InstCount;
		draw.indexOffset = (uint32_t)IdxOffset;
		draw.vertexOffset = 0;
		draw.instanceOffset = 0;

		draw.flags |= eDraw_Drawcall|eDraw_UseIBuffer;
		
		draw.debugMessages = debugMessages;

		m_LastDrawMode = Mode;
		m_LastIndexSize = Type;
		m_LastIndexOffset = (GLuint)IdxOffset;

		AddDrawcall(draw, true);
	}

	return true;
}

void WrappedOpenGL::glDrawElementsInstanced(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount)
{
	m_Real.glDrawElementsInstanced(mode, count, type, indices, instancecount);

	if(m_State == WRITING_CAPFRAME)
	{
		SCOPED_SERIALISE_CONTEXT(DRAWELEMENTS_INSTANCED);
		Serialise_glDrawElementsInstanced(mode, count, type, indices, instancecount);

		m_ContextRecord->AddChunk(scope.Get());
	}
}

bool WrappedOpenGL::Serialise_glDrawElementsInstancedBaseInstance(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLuint baseinstance)
{
	SERIALISE_ELEMENT(GLenum, Mode, mode);
	SERIALISE_ELEMENT(uint32_t, Count, count);
	SERIALISE_ELEMENT(GLenum, Type, type);
	SERIALISE_ELEMENT(uint64_t, IdxOffset, (uint64_t)indices);
	SERIALISE_ELEMENT(uint32_t, InstCount, instancecount);
	SERIALISE_ELEMENT(uint32_t, BaseInstance, baseinstance);

	if(m_State <= EXECUTING)
	{
		m_Real.glDrawElementsInstancedBaseInstance(Mode, Count, Type, (const void *)IdxOffset, InstCount, BaseInstance);
	}
	
	const string desc = m_pSerialiser->GetDebugStr();
	
	vector<DebugMessage> debugMessages = Serialise_DebugMessages();

	if(m_State == READING)
	{
		AddEvent(DRAWELEMENTS_INSTANCEDBASEINSTANCE, desc);
		string name = "glDrawElementsInstancedBaseInstance(" +
						ToStr::Get(Mode) + ", " +
						ToStr::Get(Count) + ", " +
						ToStr::Get(Type) + ", " +
						ToStr::Get(IdxOffset) + ", " +
						ToStr::Get(InstCount) + ", " + 
						ToStr::Get(BaseInstance) + ")";

		FetchDrawcall draw;
		draw.name = widen(name);
		draw.numIndices = Count;
		draw.numInstances = InstCount;
		draw.indexOffset = (uint32_t)IdxOffset;
		draw.vertexOffset = 0;
		draw.instanceOffset = BaseInstance;

		draw.flags |= eDraw_Drawcall|eDraw_UseIBuffer;
		
		draw.debugMessages = debugMessages;

		m_LastDrawMode = Mode;
		m_LastIndexSize = Type;
		m_LastIndexOffset = (GLuint)IdxOffset;

		AddDrawcall(draw, true);
	}

	return true;
}

void WrappedOpenGL::glDrawElementsInstancedBaseInstance(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLuint baseinstance)
{
	m_Real.glDrawElementsInstancedBaseInstance(mode, count, type, indices, instancecount, baseinstance);

	if(m_State == WRITING_CAPFRAME)
	{
		SCOPED_SERIALISE_CONTEXT(DRAWELEMENTS_INSTANCEDBASEINSTANCE);
		Serialise_glDrawElementsInstancedBaseInstance(mode, count, type, indices, instancecount, baseinstance);

		m_ContextRecord->AddChunk(scope.Get());
	}
}

bool WrappedOpenGL::Serialise_glDrawElementsInstancedBaseVertex(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex)
{
	SERIALISE_ELEMENT(GLenum, Mode, mode);
	SERIALISE_ELEMENT(uint32_t, Count, count);
	SERIALISE_ELEMENT(GLenum, Type, type);
	SERIALISE_ELEMENT(uint64_t, IdxOffset, (uint64_t)indices);
	SERIALISE_ELEMENT(uint32_t, InstCount, instancecount);
	SERIALISE_ELEMENT(int32_t, BaseVertex, basevertex);

	if(m_State <= EXECUTING)
	{
		m_Real.glDrawElementsInstancedBaseVertex(Mode, Count, Type, (const void *)IdxOffset, InstCount, BaseVertex);
	}
	
	const string desc = m_pSerialiser->GetDebugStr();
	
	vector<DebugMessage> debugMessages = Serialise_DebugMessages();

	if(m_State == READING)
	{
		AddEvent(DRAWELEMENTS_INSTANCEDBASEVERTEX, desc);
		string name = "glDrawElementsInstancedBaseVertex(" +
						ToStr::Get(Mode) + ", " +
						ToStr::Get(Count) + ", " +
						ToStr::Get(Type) + ", " +
						ToStr::Get(IdxOffset) + ", " +
						ToStr::Get(InstCount) + ", " + 
						ToStr::Get(BaseVertex) + ")";

		FetchDrawcall draw;
		draw.name = widen(name);
		draw.numIndices = Count;
		draw.numInstances = InstCount;
		draw.indexOffset = (uint32_t)IdxOffset;
		draw.vertexOffset = BaseVertex;
		draw.instanceOffset = 0;

		draw.flags |= eDraw_Drawcall|eDraw_UseIBuffer;
		
		draw.debugMessages = debugMessages;

		m_LastDrawMode = Mode;
		m_LastIndexSize = Type;
		m_LastIndexOffset = (GLuint)IdxOffset;

		AddDrawcall(draw, true);
	}

	return true;
}

void WrappedOpenGL::glDrawElementsInstancedBaseVertex(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex)
{
	m_Real.glDrawElementsInstancedBaseVertex(mode, count, type, indices, instancecount, basevertex);

	if(m_State == WRITING_CAPFRAME)
	{
		SCOPED_SERIALISE_CONTEXT(DRAWELEMENTS_INSTANCEDBASEVERTEX);
		Serialise_glDrawElementsInstancedBaseVertex(mode, count, type, indices, instancecount, basevertex);

		m_ContextRecord->AddChunk(scope.Get());
	}
}

bool WrappedOpenGL::Serialise_glDrawElementsInstancedBaseVertexBaseInstance(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex, GLuint baseinstance)
{
	SERIALISE_ELEMENT(GLenum, Mode, mode);
	SERIALISE_ELEMENT(uint32_t, Count, count);
	SERIALISE_ELEMENT(GLenum, Type, type);
	SERIALISE_ELEMENT(uint64_t, IdxOffset, (uint64_t)indices);
	SERIALISE_ELEMENT(uint32_t, InstCount, instancecount);
	SERIALISE_ELEMENT(int32_t, BaseVertex, basevertex);
	SERIALISE_ELEMENT(uint32_t, BaseInstance, baseinstance);

	if(m_State <= EXECUTING)
	{
		m_Real.glDrawElementsInstancedBaseVertexBaseInstance(Mode, Count, Type, (const void *)IdxOffset, InstCount, BaseVertex, BaseInstance);
	}
	
	const string desc = m_pSerialiser->GetDebugStr();
	
	vector<DebugMessage> debugMessages = Serialise_DebugMessages();

	if(m_State == READING)
	{
		AddEvent(DRAWELEMENTS_INSTANCEDBASEVERTEXBASEINSTANCE, desc);
		string name = "glDrawElementsInstancedBaseVertexBaseInstance(" +
						ToStr::Get(Mode) + ", " +
						ToStr::Get(Count) + ", " +
						ToStr::Get(Type) + ", " +
						ToStr::Get(IdxOffset) + ", " +
						ToStr::Get(InstCount) + ", " + 
						ToStr::Get(BaseVertex) + ", " +
						ToStr::Get(BaseInstance) + ")";

		FetchDrawcall draw;
		draw.name = widen(name);
		draw.numIndices = Count;
		draw.numInstances = InstCount;
		draw.indexOffset = (uint32_t)IdxOffset;
		draw.vertexOffset = BaseVertex;
		draw.instanceOffset = BaseInstance;

		draw.flags |= eDraw_Drawcall|eDraw_UseIBuffer;
		
		draw.debugMessages = debugMessages;

		m_LastDrawMode = Mode;
		m_LastIndexSize = Type;
		m_LastIndexOffset = (GLuint)IdxOffset;

		AddDrawcall(draw, true);
	}

	return true;
}

void WrappedOpenGL::glDrawElementsInstancedBaseVertexBaseInstance(GLenum mode, GLsizei count, GLenum type, const void *indices, GLsizei instancecount, GLint basevertex, GLuint baseinstance)
{
	m_Real.glDrawElementsInstancedBaseVertexBaseInstance(mode, count, type, indices, instancecount, basevertex, baseinstance);

	if(m_State == WRITING_CAPFRAME)
	{
		SCOPED_SERIALISE_CONTEXT(DRAWELEMENTS_INSTANCEDBASEVERTEXBASEINSTANCE);
		Serialise_glDrawElementsInstancedBaseVertexBaseInstance(mode, count, type, indices, instancecount, basevertex, baseinstance);

		m_ContextRecord->AddChunk(scope.Get());
	}
}

bool WrappedOpenGL::Serialise_glMultiDrawArrays(GLenum mode, const GLint *first, const GLsizei *count, GLsizei drawcount)
{
	SERIALISE_ELEMENT(GLenum, Mode, mode);
	SERIALISE_ELEMENT(uint32_t, Count, drawcount);

	SERIALISE_ELEMENT_ARR(int32_t, firstArray, first, Count);
	SERIALISE_ELEMENT_ARR(int32_t, countArray, count, Count);

	if(m_State <= EXECUTING)
	{
		m_Real.glMultiDrawArrays(Mode, firstArray, countArray, Count);
	}
	
	const string desc = m_pSerialiser->GetDebugStr();
	
	vector<DebugMessage> debugMessages = Serialise_DebugMessages();

	if(m_State == READING)
	{
		AddEvent(MULTI_DRAWARRAYS, desc);
		string name = "glMultiDrawArrays(" +
						ToStr::Get(Mode) + ", " +
						ToStr::Get(Count) + ")";
				
		RDCUNIMPLEMENTED("Not processing multi-draw data for glMultiDrawArrays() display");

		FetchDrawcall draw;
		draw.name = widen(name);
		draw.numIndices = 1;
		draw.numInstances = 1;
		draw.indexOffset = 0;
		draw.vertexOffset = 0;
		draw.instanceOffset = 0;

		draw.flags |= eDraw_Drawcall;
		
		draw.debugMessages = debugMessages;

		m_LastDrawMode = Mode;

		AddDrawcall(draw, true);
	}

	SAFE_DELETE_ARRAY(firstArray);
	SAFE_DELETE_ARRAY(countArray);

	return true;
}

void WrappedOpenGL::glMultiDrawArrays(GLenum mode, const GLint *first, const GLsizei *count, GLsizei drawcount)
{
	m_Real.glMultiDrawArrays(mode, first, count, drawcount);

	if(m_State == WRITING_CAPFRAME)
	{
		SCOPED_SERIALISE_CONTEXT(MULTI_DRAWARRAYS);
		Serialise_glMultiDrawArrays(mode, first, count, drawcount);

		m_ContextRecord->AddChunk(scope.Get());
	}
}

bool WrappedOpenGL::Serialise_glMultiDrawElements(GLenum mode, const GLsizei *count, GLenum type, const void *const*indices, GLsizei drawcount)
{
	SERIALISE_ELEMENT(GLenum, Mode, mode);
	SERIALISE_ELEMENT(GLenum, Type, type);
	SERIALISE_ELEMENT(uint32_t, Count, drawcount);
	
	SERIALISE_ELEMENT_ARR(int32_t, countArray, count, Count);

	void **idxOffsArray = new void*[Count];
	size_t len = Count;

	// serialise pointer array as uint64s
	if(m_State >= WRITING)
	{
		for(uint32_t i=0; i < Count; i++)
		{
			uint64_t ptr = (uint64_t)indices[i];
			m_pSerialiser->Serialise("idxOffsArray", ptr);
		}
	}
	else
	{
		for(uint32_t i=0; i < Count; i++)
		{
			uint64_t ptr = 0;
			m_pSerialiser->Serialise("idxOffsArray", ptr);
			idxOffsArray[i] = (void *)ptr;
		}
	}

	if(m_State <= EXECUTING)
	{
		m_Real.glMultiDrawElements(Mode, countArray, Type, idxOffsArray, Count);
	}
	
	const string desc = m_pSerialiser->GetDebugStr();
	
	vector<DebugMessage> debugMessages = Serialise_DebugMessages();

	if(m_State == READING)
	{
		AddEvent(MULTI_DRAWARRAYS, desc);
		string name = "glMultiDrawElements(" +
						ToStr::Get(Mode) + ", " +
						ToStr::Get(Count) + ")";
				
		RDCUNIMPLEMENTED("Not processing multi-draw data for glMultiDrawElements() display");

		FetchDrawcall draw;
		draw.name = widen(name);
		draw.numIndices = 1;
		draw.numInstances = 1;
		draw.indexOffset = 0;
		draw.vertexOffset = 0;
		draw.instanceOffset = 0;

		draw.flags |= eDraw_Drawcall;
		
		draw.debugMessages = debugMessages;

		m_LastDrawMode = Mode;

		AddDrawcall(draw, true);
	}

	SAFE_DELETE_ARRAY(countArray);
	SAFE_DELETE_ARRAY(idxOffsArray);

	return true;
}

void WrappedOpenGL::glMultiDrawElements(GLenum mode, const GLsizei *count, GLenum type, const void *const*indices, GLsizei drawcount)
{
	m_Real.glMultiDrawElements(mode, count, type, indices, drawcount);

	if(m_State == WRITING_CAPFRAME)
	{
		SCOPED_SERIALISE_CONTEXT(MULTI_DRAWELEMENTS);
		Serialise_glMultiDrawElements(mode, count, type, indices, drawcount);

		m_ContextRecord->AddChunk(scope.Get());
	}
}

bool WrappedOpenGL::Serialise_glClearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat *value)
{
	SERIALISE_ELEMENT(GLenum, buf, buffer);
	SERIALISE_ELEMENT(int32_t, draw, drawbuffer);
	
	if(buf != eGL_DEPTH)
	{
		Vec4f v;
		if(value) v = *((Vec4f *)value);

		m_pSerialiser->Serialise<4>("value", (float *)&v.x);
		
		if(m_State <= EXECUTING)
			m_Real.glClearBufferfv(buf, draw, &v.x);
	}
	else
	{
		SERIALISE_ELEMENT(float, val, *value);

		if(m_State <= EXECUTING)
			m_Real.glClearBufferfv(buf, draw, &val);
	}
	
	const string desc = m_pSerialiser->GetDebugStr();
	
	vector<DebugMessage> debugMessages = Serialise_DebugMessages();

	if(m_State == READING)
	{
		AddEvent(CLEARBUFFERF, desc);
		string name = "glClearBufferfv(" +
						ToStr::Get(buf) + ", " +
						ToStr::Get(draw) + ")";

		FetchDrawcall draw;
		draw.name = widen(name);
		draw.flags |= eDraw_Clear;
		
		draw.debugMessages = debugMessages;

		AddDrawcall(draw, true);
	}


	return true;
}

void WrappedOpenGL::glClearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat *value)
{
	m_Real.glClearBufferfv(buffer, drawbuffer, value);

	if(m_State == WRITING_CAPFRAME)
	{
		SCOPED_SERIALISE_CONTEXT(CLEARBUFFERF);
		Serialise_glClearBufferfv(buffer, drawbuffer, value);
		
		m_ContextRecord->AddChunk(scope.Get());
	}
}

bool WrappedOpenGL::Serialise_glClearBufferiv(GLenum buffer, GLint drawbuffer, const GLint *value)
{
	SERIALISE_ELEMENT(GLenum, buf, buffer);
	SERIALISE_ELEMENT(int32_t, draw, drawbuffer);
	
	if(buf != eGL_STENCIL)
	{
		int32_t v[4];
		if(value) memcpy(v, value, sizeof(v));

		m_pSerialiser->Serialise<4>("value", v);
		
		if(m_State <= EXECUTING)
			m_Real.glClearBufferiv(buf, draw, v);
	}
	else
	{
		SERIALISE_ELEMENT(int32_t, val, *value);

		if(m_State <= EXECUTING)
			m_Real.glClearBufferiv(buf, draw, &val);
	}
	
	const string desc = m_pSerialiser->GetDebugStr();
	
	vector<DebugMessage> debugMessages = Serialise_DebugMessages();

	if(m_State == READING)
	{
		AddEvent(CLEARBUFFERI, desc);
		string name = "glClearBufferiv(" +
						ToStr::Get(buf) + ", " +
						ToStr::Get(draw) + ")";

		FetchDrawcall draw;
		draw.name = widen(name);
		draw.flags |= eDraw_Clear;
		
		draw.debugMessages = debugMessages;

		AddDrawcall(draw, true);
	}


	return true;
}

void WrappedOpenGL::glClearBufferiv(GLenum buffer, GLint drawbuffer, const GLint *value)
{
	m_Real.glClearBufferiv(buffer, drawbuffer, value);

	if(m_State == WRITING_CAPFRAME)
	{
		SCOPED_SERIALISE_CONTEXT(CLEARBUFFERI);
		Serialise_glClearBufferiv(buffer, drawbuffer, value);
		
		m_ContextRecord->AddChunk(scope.Get());
	}
}

bool WrappedOpenGL::Serialise_glClearBufferuiv(GLenum buffer, GLint drawbuffer, const GLuint *value)
{
	SERIALISE_ELEMENT(GLenum, buf, buffer);
	SERIALISE_ELEMENT(int32_t, draw, drawbuffer);
	
	{
		uint32_t v[4];
		if(value) memcpy(v, value, sizeof(v));

		m_pSerialiser->Serialise<4>("value", v);
		
		if(m_State <= EXECUTING)
			m_Real.glClearBufferuiv(buf, draw, v);
	}
	
	const string desc = m_pSerialiser->GetDebugStr();
	
	vector<DebugMessage> debugMessages = Serialise_DebugMessages();

	if(m_State == READING)
	{
		AddEvent(CLEARBUFFERUI, desc);
		string name = "glClearBufferuiv(" +
						ToStr::Get(buf) + ", " +
						ToStr::Get(draw) + ")";

		FetchDrawcall draw;
		draw.name = widen(name);
		draw.flags |= eDraw_Clear;
		
		draw.debugMessages = debugMessages;

		AddDrawcall(draw, true);
	}

	return true;
}

void WrappedOpenGL::glClearBufferuiv(GLenum buffer, GLint drawbuffer, const GLuint *value)
{
	m_Real.glClearBufferuiv(buffer, drawbuffer, value);

	if(m_State == WRITING_CAPFRAME)
	{
		SCOPED_SERIALISE_CONTEXT(CLEARBUFFERUI);
		Serialise_glClearBufferuiv(buffer, drawbuffer, value);
		
		m_ContextRecord->AddChunk(scope.Get());
	}
}
		
bool WrappedOpenGL::Serialise_glClearBufferfi(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil)
{
	SERIALISE_ELEMENT(GLenum, buf, buffer);
	SERIALISE_ELEMENT(int32_t, draw, drawbuffer);
	SERIALISE_ELEMENT(float, d, depth);
	SERIALISE_ELEMENT(int32_t, s, stencil);
	
	if(m_State <= EXECUTING)
		m_Real.glClearBufferfi(buf, draw, d, s);
	
	const string desc = m_pSerialiser->GetDebugStr();
	
	vector<DebugMessage> debugMessages = Serialise_DebugMessages();

	if(m_State == READING)
	{
		AddEvent(CLEARBUFFERFI, desc);
		string name = "glClearBufferfi(" +
						ToStr::Get(buf) + ", " +
						ToStr::Get(draw) + ")";

		FetchDrawcall draw;
		draw.name = widen(name);
		draw.flags |= eDraw_Clear;
		
		draw.debugMessages = debugMessages;

		AddDrawcall(draw, true);
	}

	return true;
}

void WrappedOpenGL::glClearBufferfi(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil)
{
	m_Real.glClearBufferfi(buffer, drawbuffer, depth, stencil);

	if(m_State == WRITING_CAPFRAME)
	{
		SCOPED_SERIALISE_CONTEXT(CLEARBUFFERFI);
		Serialise_glClearBufferfi(buffer, drawbuffer, depth, stencil);
		
		m_ContextRecord->AddChunk(scope.Get());
	}
}

bool WrappedOpenGL::Serialise_glClearBufferData(GLenum target, GLenum internalformat, GLenum format, GLenum type, const void *data)
{
	SERIALISE_ELEMENT(GLenum, Target, target);
	SERIALISE_ELEMENT(GLenum, InternalFormat, internalformat);
	SERIALISE_ELEMENT(GLenum, Format, format);
	SERIALISE_ELEMENT(GLenum, Type, type);

	uint64_t val[4] = {0};

	if(m_State >= WRITING)
	{
		size_t s = 1;
		switch(Format)
		{
			default:
				RDCWARN("Unexpected format %x, defaulting to single component", Format);
			case eGL_RED:
			case eGL_DEPTH_COMPONENT:
				s = 1; break;
			case eGL_RG:
			case eGL_DEPTH_STENCIL:
				s = 2; break;
			case eGL_RGB:
			case eGL_BGR:
				s = 3; break;
			case eGL_RGBA:
			case eGL_BGRA:
				s = 4; break;
		}
		switch(Type)
		{
			case eGL_UNSIGNED_BYTE:
			case eGL_BYTE:
				s *= 1; break;
			case eGL_UNSIGNED_SHORT:
			case eGL_SHORT:
				s *= 2; break;
			case eGL_UNSIGNED_INT:
			case eGL_INT:
			case eGL_FLOAT:
				s *= 4; break;
			default:
				RDCWARN("Unexpected type %x, defaulting to 1 byte single component type", Format);
			case eGL_UNSIGNED_BYTE_3_3_2:
			case eGL_UNSIGNED_BYTE_2_3_3_REV:
				s = 1; break;
			case eGL_UNSIGNED_SHORT_5_6_5:
			case eGL_UNSIGNED_SHORT_5_6_5_REV:
			case eGL_UNSIGNED_SHORT_4_4_4_4:
			case eGL_UNSIGNED_SHORT_4_4_4_4_REV:
			case eGL_UNSIGNED_SHORT_5_5_5_1:
			case eGL_UNSIGNED_SHORT_1_5_5_5_REV:
			case eGL_UNSIGNED_INT_8_8_8_8:
			case eGL_UNSIGNED_INT_8_8_8_8_REV:
				s = 2; break;
			case eGL_UNSIGNED_INT_10_10_10_2:
			case eGL_UNSIGNED_INT_2_10_10_10_REV:
				s = 4; break;
		}
		memcpy(val, data, s);
	}

	m_pSerialiser->Serialise<4>("data", val);
	
	if(m_State <= EXECUTING)
	{
		m_Real.glClearBufferData(Target, InternalFormat, Format, Type, (const void *)&val[0]);
	}

	return true;
}

void WrappedOpenGL::glClearBufferData(GLenum target, GLenum internalformat, GLenum format, GLenum type, const void *data)
{
	m_Real.glClearBufferData(target, internalformat, format, type, data);

	if(m_State == WRITING_CAPFRAME)
	{
		SCOPED_SERIALISE_CONTEXT(CLEARBUFFERDATA);
		Serialise_glClearBufferData(target, internalformat, format, type, data);
		
		m_ContextRecord->AddChunk(scope.Get());
	}
}

bool WrappedOpenGL::Serialise_glClearBufferSubData(GLenum target, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void *data)
{
	SERIALISE_ELEMENT(GLenum, Target, target);
	SERIALISE_ELEMENT(GLenum, InternalFormat, internalformat);
	SERIALISE_ELEMENT(uint64_t, Offset, (uint64_t)offset);
	SERIALISE_ELEMENT(uint64_t, Size, (uint64_t)size);
	SERIALISE_ELEMENT(GLenum, Format, format);
	SERIALISE_ELEMENT(GLenum, Type, type);

	uint64_t val[4] = {0};

	if(m_State >= WRITING)
	{
		size_t s = 1;
		switch(Format)
		{
			default:
				RDCWARN("Unexpected format %x, defaulting to single component", Format);
			case eGL_RED:
			case eGL_DEPTH_COMPONENT:
				s = 1; break;
			case eGL_RG:
			case eGL_DEPTH_STENCIL:
				s = 2; break;
			case eGL_RGB:
			case eGL_BGR:
				s = 3; break;
			case eGL_RGBA:
			case eGL_BGRA:
				s = 4; break;
		}
		switch(Type)
		{
			case eGL_UNSIGNED_BYTE:
			case eGL_BYTE:
				s *= 1; break;
			case eGL_UNSIGNED_SHORT:
			case eGL_SHORT:
				s *= 2; break;
			case eGL_UNSIGNED_INT:
			case eGL_INT:
			case eGL_FLOAT:
				s *= 4; break;
			default:
				RDCWARN("Unexpected type %x, defaulting to 1 byte single component type", Format);
			case eGL_UNSIGNED_BYTE_3_3_2:
			case eGL_UNSIGNED_BYTE_2_3_3_REV:
				s = 1; break;
			case eGL_UNSIGNED_SHORT_5_6_5:
			case eGL_UNSIGNED_SHORT_5_6_5_REV:
			case eGL_UNSIGNED_SHORT_4_4_4_4:
			case eGL_UNSIGNED_SHORT_4_4_4_4_REV:
			case eGL_UNSIGNED_SHORT_5_5_5_1:
			case eGL_UNSIGNED_SHORT_1_5_5_5_REV:
			case eGL_UNSIGNED_INT_8_8_8_8:
			case eGL_UNSIGNED_INT_8_8_8_8_REV:
				s = 2; break;
			case eGL_UNSIGNED_INT_10_10_10_2:
			case eGL_UNSIGNED_INT_2_10_10_10_REV:
				s = 4; break;
		}
		memcpy(val, data, s);
	}

	m_pSerialiser->Serialise<4>("data", val);

	if(m_State <= EXECUTING)
	{
		m_Real.glClearBufferSubData(Target, InternalFormat, (GLintptr)Offset, (GLsizeiptr)Size, Format, Type, (const void *)&val[0]);
	}

	return true;
}

void WrappedOpenGL::glClearBufferSubData(GLenum target, GLenum internalformat, GLintptr offset, GLsizeiptr size, GLenum format, GLenum type, const void *data)
{
	m_Real.glClearBufferSubData(target, internalformat, offset, size, format, type, data);

	if(m_State == WRITING_CAPFRAME)
	{
		SCOPED_SERIALISE_CONTEXT(CLEARBUFFERSUBDATA);
		Serialise_glClearBufferSubData(target, internalformat, offset, size, format, type, data);

		m_ContextRecord->AddChunk(scope.Get());
	}
}

bool WrappedOpenGL::Serialise_glClear(GLbitfield mask)
{
	SERIALISE_ELEMENT(uint32_t, Mask, mask);

	if(m_State <= EXECUTING)
		m_Real.glClear(Mask);
	
	const string desc = m_pSerialiser->GetDebugStr();

	vector<DebugMessage> debugMessages = Serialise_DebugMessages();

	if(m_State == READING)
	{
		AddEvent(CLEARBUFFERF, desc);
		string name = "glClear(";
		if(Mask & GL_DEPTH_BUFFER_BIT)
			name += "GL_DEPTH_BUFFER_BIT | ";
		if(Mask & GL_COLOR_BUFFER_BIT)
			name += "GL_COLOR_BUFFER_BIT | ";
		if(Mask & GL_STENCIL_BUFFER_BIT)
			name += "GL_STENCIL_BUFFER_BIT | ";

		if(Mask & (eGL_DEPTH_BUFFER_BIT|eGL_COLOR_BUFFER_BIT|eGL_STENCIL_BUFFER_BIT))
		{
			name.pop_back(); // ' '
			name.pop_back(); // '|'
			name.pop_back(); // ' '
		}

		name += ")";

		FetchDrawcall draw;
		draw.name = widen(name);
		draw.flags |= eDraw_Clear;
		
		draw.debugMessages = debugMessages;

		AddDrawcall(draw, true);
	}

	return true;
}

void WrappedOpenGL::glClear(GLbitfield mask)
{
	m_Real.glClear(mask);
	
	if(m_State == WRITING_CAPFRAME)
	{
		SCOPED_SERIALISE_CONTEXT(CLEAR);
		Serialise_glClear(mask);
		
		m_ContextRecord->AddChunk(scope.Get());
	}
}
