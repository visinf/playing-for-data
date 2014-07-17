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

#pragma once

struct FloatVector
{
	FloatVector() {}
	FloatVector(float X, float Y, float Z, float W)
		: x(X), y(Y), z(Z), w(W) {}
	float x, y, z, w;
};

struct ResourceFormat
{
	ResourceFormat()
	{
		rawType = 0;
		special = true;
		specialFormat = eSpecial_Unknown;

		compCount = compByteWidth = 0;
		compType = eCompType_Float;

		srgbCorrected = false;
	}
	
	uint32_t rawType;

	// indicates it's not a type represented with the members below
	// usually this means non-uniform across components or block compressed
	bool32 special;
	SpecialFormat specialFormat;

	rdctype::wstr strname;

	uint32_t compCount;
	uint32_t compByteWidth;
	FormatComponentType compType;

	bool32 srgbCorrected;
};

struct FetchBuffer
{
	ResourceId ID;
	rdctype::wstr name;
	bool32 customName;
	uint32_t length;
	uint32_t structureSize;
	uint32_t creationFlags;
	uint64_t byteSize;
};

struct FetchTexture
{
	rdctype::wstr name;
	bool32 customName;
	ResourceFormat format;
	uint32_t dimension;
	uint32_t width, height, depth;
	ResourceId ID;
	bool32 cubemap;
	uint32_t mips;
	uint32_t arraysize;
	uint32_t numSubresources;
	uint32_t creationFlags;
	uint32_t msQual, msSamp;
	uint64_t byteSize;
};

struct FetchFrameInfo
{
	uint32_t frameNumber;
	uint32_t firstEvent;
	uint64_t fileOffset;
	ResourceId immContextId;
};

struct FetchAPIEvent
{
	uint32_t eventID;

	ResourceId context;

	rdctype::array<uint64_t> callstack;

	rdctype::wstr eventDesc;

	uint64_t fileOffset;
};

struct DebugMessage
{
	DebugMessageCategory category;
	DebugMessageSeverity severity;
	uint32_t messageID;
	rdctype::str description;
};

struct EventUsage
{
	EventUsage()
		: eventID(0), usage(eUsage_None) {}
	EventUsage(uint32_t e, ResourceUsage u)
		: eventID(e), usage(u) {}

	uint32_t eventID;
	ResourceUsage usage;
};

struct FetchDrawcall
{
	FetchDrawcall()
	{
		Reset();
	}

	void Reset()
	{
		eventID = 0;
		drawcallID = 0;
		numIndices = 0;
		numInstances = 0;
		indexOffset = 0;
		vertexOffset = 0;
		instanceOffset = 0;
		flags = 0;
		context = ResourceId();
		duration = -1.0f;
		parent = 0;
		previous = 0;
		next = 0;
		for(int i=0; i < 10; i++)
			outputs[i] = ResourceId();
	}

	uint32_t eventID, drawcallID;

	rdctype::wstr name;

	uint32_t flags;

	uint32_t numIndices;
	uint32_t numInstances;
	uint32_t indexOffset;
	uint32_t vertexOffset;
	uint32_t instanceOffset;

	ResourceId context;

	double duration;

	int64_t parent;

	int64_t previous;
	int64_t next;

	ResourceId outputs[8];
	ResourceId depthOut;

	rdctype::array<FetchAPIEvent> events;
	rdctype::array<FetchDrawcall> children;

	rdctype::array<DebugMessage> debugMessages;
};

struct APIProperties
{
	APIPipelineStateType pipelineType;
};

struct PixelValue
{
	union
	{
		float value_f[4];
		uint32_t value_u[4];
		int32_t value_i[4];
		uint16_t value_u16[4];
	};
};

struct ModificationValue
{
	PixelValue col;
	float depth;
	int32_t stencil;
};

struct PixelModification
{
	uint32_t eventID;

	ModificationValue preMod;
	ModificationValue shaderOut;
	ModificationValue postMod;

	bool32 backfaceCulled;
	bool32 depthClipped;
	bool32 viewClipped;
	bool32 scissorClipped;
	bool32 shaderDiscarded;
	bool32 depthTestFailed;
	bool32 stencilTestFailed;
};

struct PostVSMeshData
{
	rdctype::array<byte> buf;
	uint32_t numVerts;
	PrimitiveTopology topo;
};
