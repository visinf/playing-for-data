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


#include "core/core.h"

#include "common/string_utils.h"

#include "maths/formatpacking.h"

#include "driver/dxgi/dxgi_wrapped.h"
#include "driver/d3d11/d3d11_device.h"
#include "driver/d3d11/d3d11_resources.h"
#include "driver/d3d11/d3d11_renderstate.h"
#include "driver/d3d11/d3d11_context.h"

#include "3rdparty/jpeg-compressor/jpge.h"

#if defined(INCLUDE_D3D_11_1)
#include <d3d11shadertracing.h>
#endif

const char *D3D11ChunkNames[] =
{
	"ID3D11Device::Initialisation",
	"ID3D11Resource::SetDebugName",
	"ID3D11Resource::Release",
	"IDXGISwapChain::GetBuffer",
	"ID3D11Device::CreateTexture1D",
	"ID3D11Device::CreateTexture2D",
	"ID3D11Device::CreateTexture3D",
	"ID3D11Device::CreateBuffer",
	"ID3D11Device::CreateVertexShader",
	"ID3D11Device::CreateHullShader",
	"ID3D11Device::CreateDomainShader",
	"ID3D11Device::CreateGeometryShader",
	"ID3D11Device::CreateGeometryShaderWithStreamOut",
	"ID3D11Device::CreatePixelShader",
	"ID3D11Device::CreateComputeShader",
	"ID3D11ClassLinkage::GetClassInstance",
	"ID3D11ClassLinkage::CreateClassInstance",
	"ID3D11Device::CreateClassLinkage",
	"ID3D11Device::CreateShaderResourceView",
	"ID3D11Device::CreateRenderTargetView",
	"ID3D11Device::CreateDepthStencilView",
	"ID3D11Device::CreateUnorderedAccessView",
	"ID3D11Device::CreateInputLayout",
	"ID3D11Device::CreateBlendState",
	"ID3D11Device::CreateDepthStencilState",
	"ID3D11Device::CreateRasterizerState",
	"ID3D11Device::CreateSamplerState",
	"ID3D11Device::CreateQuery",
	"ID3D11Device::CreatePredicate",
	"ID3D11Device::CreateCounter",
	"ID3D11Device::CreateDeferredContext",
	"ID3D11Device::SetExceptionMode",
	"ID3D11Device::OpenSharedResource",

	"Capture",

	"ID3D11DeviceContext::IASetInputLayout",
	"ID3D11DeviceContext::IASetVertexBuffers",
	"ID3D11DeviceContext::IASetIndexBuffer",
	"ID3D11DeviceContext::IASetPrimitiveTopology",
	
	"ID3D11DeviceContext::VSSetConstantBuffers",
	"ID3D11DeviceContext::VSSetShaderResources",
	"ID3D11DeviceContext::VSSetSamplers",
	"ID3D11DeviceContext::VSSetShader",
	
	"ID3D11DeviceContext::HSSetConstantBuffers",
	"ID3D11DeviceContext::HSSetShaderResources",
	"ID3D11DeviceContext::HSSetSamplers",
	"ID3D11DeviceContext::HSSetShader",
	
	"ID3D11DeviceContext::DSSetConstantBuffers",
	"ID3D11DeviceContext::DSSetShaderResources",
	"ID3D11DeviceContext::DSSetSamplers",
	"ID3D11DeviceContext::DSSetShader",
	
	"ID3D11DeviceContext::GSSetConstantBuffers",
	"ID3D11DeviceContext::GSSetShaderResources",
	"ID3D11DeviceContext::GSSetSamplers",
	"ID3D11DeviceContext::GSSetShader",
	
	"ID3D11DeviceContext::SOSetTargets",
	
	"ID3D11DeviceContext::PSSetConstantBuffers",
	"ID3D11DeviceContext::PSSetShaderResources",
	"ID3D11DeviceContext::PSSetSamplers",
	"ID3D11DeviceContext::PSSetShader",
	
	"ID3D11DeviceContext::CSSetConstantBuffers",
	"ID3D11DeviceContext::CSSetShaderResources",
	"ID3D11DeviceContext::CSSetUnorderedAccessViews",
	"ID3D11DeviceContext::CSSetSamplers",
	"ID3D11DeviceContext::CSSetShader",

	"ID3D11DeviceContext::RSSetViewports",
	"ID3D11DeviceContext::RSSetScissors",
	"ID3D11DeviceContext::RSSetState",
	
	"ID3D11DeviceContext::OMSetRenderTargets",
	"ID3D11DeviceContext::OMSetRenderTargetsAndUnorderedAccessViews",
	"ID3D11DeviceContext::OMSetBlendState",
	"ID3D11DeviceContext::OMSetDepthStencilState",
	
	"ID3D11DeviceContext::DrawIndexedInstanced",
	"ID3D11DeviceContext::DrawInstanced",
	"ID3D11DeviceContext::DrawIndexed",
	"ID3D11DeviceContext::Draw",
	"ID3D11DeviceContext::DrawAuto",
	"ID3D11DeviceContext::DrawIndexedInstancedIndirect",
	"ID3D11DeviceContext::DrawInstancedIndirect",

	"ID3D11DeviceContext::Map",
	"ID3D11DeviceContext::Unmap",
	
	"ID3D11DeviceContext::CopySubresourceRegion",
	"ID3D11DeviceContext::CopyResource",
	"ID3D11DeviceContext::UpdateSubresource",
	"ID3D11DeviceContext::CopyStructureCount",
	"ID3D11DeviceContext::ResolveSubresource",
	"ID3D11DeviceContext::GenerateMips",

	"ID3D11DeviceContext::ClearDepthStencilView",
	"ID3D11DeviceContext::ClearRenderTargetView",
	"ID3D11DeviceContext::ClearUnorderedAccessViewInt",
	"ID3D11DeviceContext::ClearUnorderedAccessViewFloat",
	"ID3D11DeviceContext::ClearState",
	
	"ID3D11DeviceContext::ExecuteCommandList",
	"ID3D11DeviceContext::Dispatch",
	"ID3D11DeviceContext::DispatchIndirect",
	"ID3D11DeviceContext::FinishCommandlist",
	"ID3D11DeviceContext::Flush",
	
	"ID3D11DeviceContext::SetPredication",
	"ID3D11DeviceContext::SetResourceMinLOD",

	"ID3D11DeviceContext::Begin",
	"ID3D11DeviceContext::End",
	
	"ID3D11Device1::CreateRasterizerState1",
	"ID3D11Device1::CreateBlendState1",
	
	"ID3D11DeviceContext1::CopySubresourceRegion1",
	"ID3D11DeviceContext1::UpdateSubresource1",
	"ID3D11DeviceContext1::ClearView",
	
	"ID3D11DeviceContext1::VSSetConstantBuffers1",
	"ID3D11DeviceContext1::HSSetConstantBuffers1",
	"ID3D11DeviceContext1::DSSetConstantBuffers1",
	"ID3D11DeviceContext1::GSSetConstantBuffers1",
	"ID3D11DeviceContext1::PSSetConstantBuffers1",
	"ID3D11DeviceContext1::CSSetConstantBuffers1",

	"D3DPERF_PushMarker",
	"D3DPERF_SetMarker",
	"D3DPERF_PopMarker",

	"DebugMessageList",

	"ContextBegin",
	"ContextEnd",
};

WRAPPED_POOL_INST(WrappedID3D11Device);

WrappedID3D11Device *WrappedID3D11Device::m_pCurrentWrappedDevice = NULL;

D3D11InitParams::D3D11InitParams()
{
	SerialiseVersion = D3D11_SERIALISE_VERSION;
	DriverType = D3D_DRIVER_TYPE_UNKNOWN;
	Flags = 0;
	SDKVersion = D3D11_SDK_VERSION;
	NumFeatureLevels = 0;
	RDCEraseEl(FeatureLevels);
}

ReplayCreateStatus D3D11InitParams::Serialise()
{
	SERIALISE_ELEMENT(uint32_t, ver, D3D11_SERIALISE_VERSION); SerialiseVersion = ver;

	if(ver != D3D11_SERIALISE_VERSION)
	{
		RDCERR("Incompatible D3D11 serialise version, expected %d got %d", D3D11_SERIALISE_VERSION, ver);
		return eReplayCreate_APIIncompatibleVersion;
	}

	SERIALISE_ELEMENT(D3D_DRIVER_TYPE, driverType, DriverType); DriverType = driverType;
	SERIALISE_ELEMENT(uint32_t, flags, Flags); Flags = flags;
	SERIALISE_ELEMENT(uint32_t, sdk, SDKVersion); SDKVersion = sdk;
	SERIALISE_ELEMENT(uint32_t, numlevels, NumFeatureLevels); NumFeatureLevels = numlevels;
	m_pSerialiser->Serialise<ARRAY_COUNT(FeatureLevels)>("FeatureLevels", FeatureLevels);

	return eReplayCreate_Success;
}

void WrappedID3D11Device::SetLogFile(const wchar_t *logfile)
{
#if defined(RELEASE)
	const bool debugSerialiser = false;
#else
	const bool debugSerialiser = true;
#endif

	m_pSerialiser = new Serialiser(logfile, Serialiser::READING, debugSerialiser);
	m_pSerialiser->SetChunkNameLookup(&GetChunkName);
	m_pImmediateContext->SetSerialiser(m_pSerialiser);
}

WrappedID3D11Device::WrappedID3D11Device(ID3D11Device* realDevice, D3D11InitParams *params)
	: m_RefCounter(realDevice, false), m_SoftRefCounter(NULL, false), m_pDevice(realDevice)
{
	if(RenderDoc::Inst().GetCrashHandler())
		RenderDoc::Inst().GetCrashHandler()->RegisterMemoryRegion(this, sizeof(WrappedID3D11Device));

#if defined(INCLUDE_D3D_11_1)
	m_pDevice1 = NULL;
	m_pDevice->QueryInterface(__uuidof(ID3D11Device1), (void **)&m_pDevice1);
#endif

	m_Replay.SetDevice(this);

	m_DebugManager = NULL;
	m_ResourceManager = new D3D11ResourceManager(this);

	// refcounters implicitly construct with one reference, but we don't start with any soft
	// references.
	m_SoftRefCounter.Release();
	m_InternalRefcount = 0;
	m_Alive = true;

	m_DummyInfo.m_pDevice = this;

	m_FrameCounter = 0;
	m_FailedFrame = 0;
	m_FailedReason = CaptureSucceeded;
	m_Failures = 0;

	m_SwapChain = NULL;

	m_FrameTimer.Restart();

	m_TotalTime = m_AvgFrametime = m_MinFrametime = m_MaxFrametime = 0.0;

	m_CurFileSize = 0;
	
#if defined(RELEASE)
	const bool debugSerialiser = false;
#else
	const bool debugSerialiser = true;
#endif

	if(RenderDoc::Inst().IsReplayApp())
	{
		m_State = READING;
		m_pSerialiser = NULL;
		m_pDebugSerialiser = NULL;

		TrackedResource::SetReplayResourceIDs();
	}
	else
	{
		m_State = WRITING_IDLE;
		m_pSerialiser = new Serialiser(NULL, Serialiser::WRITING, debugSerialiser);

#ifdef DEBUG_TEXT_SERIALISER
		m_pDebugSerialiser = new Serialiser(L"./debuglog.txt", Serialiser::DEBUGWRITING, true);
#else
		m_pDebugSerialiser = NULL;
#endif
	}

	if(m_pSerialiser)
		m_pSerialiser->SetChunkNameLookup(&GetChunkName);
	
	// create a temporary and grab its resource ID
	m_ResourceID = TrackedResource().GetResourceID();

	m_DeviceRecord = NULL;

	if(!RenderDoc::Inst().IsReplayApp())
	{
		m_DeviceRecord = GetResourceManager()->AddResourceRecord(m_ResourceID);
		m_DeviceRecord->DataInSerialiser = false;
		m_DeviceRecord->SpecialResource = true;
		m_DeviceRecord->Length = 0;
		m_DeviceRecord->NumSubResources = 0;
		m_DeviceRecord->SubResources = NULL;
	}
	
	ID3D11DeviceContext *context = NULL;
	realDevice->GetImmediateContext(&context);

	m_pImmediateContext = new WrappedID3D11DeviceContext(this, context, m_pSerialiser, m_pDebugSerialiser);

	SAFE_RELEASE(context);

	realDevice->QueryInterface(__uuidof(ID3D11InfoQueue), (void **)&m_pInfoQueue);

	if(m_pInfoQueue)
	{
		m_pInfoQueue->SetMuteDebugOutput(true);

		UINT size = m_pInfoQueue->GetStorageFilterStackSize();

		while(size > 1)
		{
			m_pInfoQueue->ClearStorageFilter();
			size = m_pInfoQueue->GetStorageFilterStackSize();
		}

		size = m_pInfoQueue->GetRetrievalFilterStackSize();

		while(size > 1)
		{
			m_pInfoQueue->ClearRetrievalFilter();
			size = m_pInfoQueue->GetRetrievalFilterStackSize();
		}

		m_pInfoQueue->ClearStoredMessages();

		if(RenderDoc::Inst().IsReplayApp())
			m_pInfoQueue->SetMuteDebugOutput(false);
	}
	else
	{
		RDCDEBUG("Couldn't get ID3D11InfoQueue.");
	}

	m_InitParams = params;

	SetContextFilter(ResourceId(), 0, 0);

	// ATI workaround - these dlls can get unloaded and cause a crash.
	
	if(GetModuleHandleA("aticfx32.dll"))
		LoadLibraryA("aticfx32.dll");
	if(GetModuleHandleA("atiuxpag.dll"))
		LoadLibraryA("atiuxpag.dll");
	if(GetModuleHandleA("atidxx32.dll"))
		LoadLibraryA("atidxx32.dll");

	if(GetModuleHandleA("aticfx64.dll"))
		LoadLibraryA("aticfx64.dll");
	if(GetModuleHandleA("atiuxp64.dll"))
		LoadLibraryA("atiuxp64.dll");
	if(GetModuleHandleA("atidxx64.dll"))
		LoadLibraryA("atidxx64.dll");

	//////////////////////////////////////////////////////////////////////////
	// Compile time asserts

	RDCCOMPILE_ASSERT(ARRAY_COUNT(D3D11ChunkNames) == NUM_D3D11_CHUNKS-FIRST_CHUNK_ID, "Not right number of chunk names");
}

WrappedID3D11Device::~WrappedID3D11Device()
{
	SAFE_DELETE(m_InitParams);

	if(m_pCurrentWrappedDevice == this)
		m_pCurrentWrappedDevice = NULL;

	for(auto it = m_CachedStateObjects.begin(); it != m_CachedStateObjects.end(); ++it)
		if(*it)
			(*it)->Release();

	m_CachedStateObjects.clear();

#if defined(INCLUDE_D3D_11_1)
	SAFE_RELEASE(m_pDevice1);
#endif
	
	SAFE_RELEASE(m_pImmediateContext);

	for(auto it = m_SwapChains.begin(); it != m_SwapChains.end(); ++it)
		SAFE_RELEASE(it->second);
	
	SAFE_DELETE(m_DebugManager);
	
	if(m_DeviceRecord)
	{
		RDCASSERT(m_DeviceRecord->GetRefCount() == 1);
		m_DeviceRecord->Delete(GetResourceManager());
	}

	m_ResourceManager->Shutdown();

	SAFE_DELETE(m_ResourceManager);

	SAFE_RELEASE(m_pInfoQueue);
	SAFE_RELEASE(m_pDevice);

	SAFE_DELETE(m_pSerialiser);
	SAFE_DELETE(m_pDebugSerialiser);

	for(auto it=m_LayoutDXBC.begin(); it != m_LayoutDXBC.end(); ++it)
		SAFE_DELETE(it->second);
	m_LayoutDXBC.clear();
	m_LayoutDescs.clear();
	
	if(RenderDoc::Inst().GetCrashHandler())
		RenderDoc::Inst().GetCrashHandler()->UnregisterMemoryRegion(this);
}

void WrappedID3D11Device::CheckForDeath()
{
	if(!m_Alive) return;

	if(m_RefCounter.GetRefCount() == 0)
	{
		RDCASSERT(m_SoftRefCounter.GetRefCount() >= m_InternalRefcount);

		if(m_SoftRefCounter.GetRefCount() <= m_InternalRefcount || m_State < WRITING) // MEGA HACK
		{
			m_Alive = false;
			delete this;
		}
	}
}

ULONG STDMETHODCALLTYPE DummyID3D11InfoQueue::AddRef()
{
	m_pDevice->AddRef();
	return 1;
}

ULONG STDMETHODCALLTYPE DummyID3D11InfoQueue::Release()
{
	m_pDevice->Release();
	return 1;
}

HRESULT WrappedID3D11Device::QueryInterface(REFIID riid, void **ppvObject)
{
	// DEFINE_GUID(IID_IDirect3DDevice9, 0xd0223b96, 0xbf7a, 0x43fd, 0x92, 0xbd, 0xa4, 0x3b, 0xd, 0x82, 0xb9, 0xeb);
	static const GUID IDirect3DDevice9_uuid = { 0xd0223b96, 0xbf7a, 0x43fd, { 0x92, 0xbd, 0xa4, 0x3b, 0xd, 0x82, 0xb9, 0xeb } };

	//DEFINE_GUID(IID_ID3D11Device2,0x9d06dffa,0xd1e5,0x4d07,0x83,0xa8,0x1b,0xb1,0x23,0xf2,0xf8,0x41);
	static const GUID ID3D11Device2_uuid = { 0x9d06dffa, 0xd1e5, 0x4d07, { 0x83, 0xa8, 0x1b, 0xb1, 0x23, 0xf2, 0xf8, 0x41 } };

	//1fbad429-66ab-41cc-9617-667ac10e4459
	static const GUID ID3D11ShaderTraceFactory_uuid = { 0x1fbad429, 0x66ab, 0x41cc, { 0x96, 0x17, 0x66, 0x7a, 0xc1, 0x0e, 0x44, 0x59 } };

	if(riid == __uuidof(IDXGIDevice))
	{
		m_pDevice->QueryInterface(riid, ppvObject);

		IDXGIDevice *real = (IDXGIDevice *)(*ppvObject);
		*ppvObject = new WrappedIDXGIDevice(real, this);
		return S_OK;
	}
	else if(riid == __uuidof(IDXGIDevice1))
	{
		m_pDevice->QueryInterface(riid, ppvObject);

		IDXGIDevice1 *real = (IDXGIDevice1 *)(*ppvObject);
		*ppvObject = new WrappedIDXGIDevice1(real, this);
		return S_OK;
	}
	else if(riid == __uuidof(ID3D11Device))
	{
		AddRef();
		*ppvObject = (ID3D11Device *)this;
		return S_OK;
	}
	else if(riid == __uuidof(ID3D10Device))
	{
		RDCWARN("Trying to get ID3D10Device - not supported.");
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}
	else if(riid == IDirect3DDevice9_uuid)
	{
		RDCWARN("Trying to get IDirect3DDevice9 - not supported.");
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}
#if defined(INCLUDE_D3D_11_1)
	else if(riid == __uuidof(ID3D11Device1))
	{
		AddRef();
		*ppvObject = (ID3D11Device1 *)this;
		return S_OK;
	}
#endif
	else if(riid == ID3D11Device2_uuid)
	{
		RDCWARN("Trying to get ID3D11Device2. DX11.2 not supported at this time.");
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}
#if defined(INCLUDE_D3D_11_1)
	else if(riid == __uuidof(ID3D11ShaderTraceFactory))
	{
		RDCWARN("Trying to get ID3D11ShaderTraceFactory. Not supported at this time.");
		*ppvObject = NULL;
		return E_NOINTERFACE;
	}
#endif
	else if(riid == __uuidof(ID3D11InfoQueue))
	{
		RDCWARN("Returning a dumy ID3D11InfoQueue that does nothing. This ID3D11InfoQueue will not work!");
		*ppvObject = (ID3D11InfoQueue *)&m_DummyInfo;
		m_DummyInfo.AddRef();
		return S_OK;
	}
	else
	{
		string guid = ToStr::Get(riid);
		RDCWARN("Querying ID3D11Device for interface: %hs", guid.c_str());
	}

	return m_RefCounter.QueryInterface(riid, ppvObject);
}

#if defined(ENABLE_NVIDIA_PERFKIT)
#define NVPM_INITGUID
#include STRINGIZE(CONCAT(NVIDIA_PERFKIT_DIR, inc\\NvPmApi.h))

NvPmApi *nvAPI = NULL;
#endif

#if defined(ENABLE_NVIDIA_PERFKIT)
int enumFunc(NVPMCounterID id, const char *name)
{
	RDCLOG("(% 4d): %hs", id, name);

	return NVPM_OK;
}
#endif

const char *WrappedID3D11Device::GetChunkName(uint32_t idx)
{
	if(idx < FIRST_CHUNK_ID || idx >= NUM_D3D11_CHUNKS)
		return "<unknown>";
	return D3D11ChunkNames[idx-FIRST_CHUNK_ID];
}

void WrappedID3D11Device::LazyInit()
{
	if(m_DebugManager == NULL)
	{
		m_DebugManager = new D3D11DebugManager(this);
		
#if defined(ENABLE_NVIDIA_PERFKIT)
		
		HMODULE nvapi = LoadLibraryA(STRINGIZE(CONCAT(NVIDIA_PERFKIT_DIR, bin\\win7_x86\\NvPmApi.Core.dll)));
		if(nvapi == NULL)
		{
			RDCERR("Couldn't load perfkit");
			return;
		}
		
        NVPMGetExportTable_Pfn NVPMGetExportTable = (NVPMGetExportTable_Pfn)GetProcAddress(nvapi, "NVPMGetExportTable");
        if(NVPMGetExportTable == NULL)
		{
			RDCERR("Couldn't Get Symbol 'NVPMGetExportTable'");
			return;
		}
        
		NVPMRESULT nvResult = NVPMGetExportTable(&ETID_NvPmApi, (void **)&nvAPI);
        if(nvResult != NVPM_OK)
		{
			RDCERR("Couldn't NVPMGetExportTable");
			return;
		}

		nvResult = nvAPI->Init();

		if(nvResult != NVPM_OK)
		{
			RDCERR("Couldn't nvAPI->Init");
			return;
		}

		NVPMContext context(0);
		nvResult = nvAPI->CreateContextFromD3D11Device(m_pDevice, &context);

		if(nvResult != NVPM_OK)
		{
			RDCERR("Couldn't nvAPI->CreateContextFromD3D11Device");
			return;
		}

		nvAPI->EnumCountersByContext(context, &enumFunc);

		nvAPI->DestroyContext(context);
		nvAPI->Shutdown();
		nvAPI = NULL;
		FreeLibrary(nvapi);
#endif
	}
}

vector<DebugMessage> WrappedID3D11Device::GetDebugMessages()
{
	vector<DebugMessage> ret;

	if(!m_pInfoQueue)
		return ret;

	UINT64 numMessages = m_pInfoQueue->GetNumStoredMessagesAllowedByRetrievalFilter();

	for(UINT64 i=0; i < m_pInfoQueue->GetNumStoredMessagesAllowedByRetrievalFilter(); i++)
	{
		SIZE_T len = 0;
		m_pInfoQueue->GetMessage(i, NULL, &len);

		char *msgbuf = new char[len];
		D3D11_MESSAGE *message = (D3D11_MESSAGE *)msgbuf;

		m_pInfoQueue->GetMessage(i, message, &len);

		DebugMessage msg;
		msg.category = eDbgCategory_Miscellaneous;
		msg.severity = eDbgSeverity_Error;

		switch(message->Category)
		{
			case D3D11_MESSAGE_CATEGORY_APPLICATION_DEFINED:
				msg.category = eDbgCategory_Application_Defined;
				break;
			case D3D11_MESSAGE_CATEGORY_MISCELLANEOUS:
				msg.category = eDbgCategory_Miscellaneous;
				break;
			case D3D11_MESSAGE_CATEGORY_INITIALIZATION:
				msg.category = eDbgCategory_Initialization;
				break;
			case D3D11_MESSAGE_CATEGORY_CLEANUP:
				msg.category = eDbgCategory_Cleanup;
				break;
			case D3D11_MESSAGE_CATEGORY_COMPILATION:
				msg.category = eDbgCategory_Compilation;
				break;
			case D3D11_MESSAGE_CATEGORY_STATE_CREATION:
				msg.category = eDbgCategory_State_Creation;
				break;
			case D3D11_MESSAGE_CATEGORY_STATE_SETTING:
				msg.category = eDbgCategory_State_Setting;
				break;
			case D3D11_MESSAGE_CATEGORY_STATE_GETTING:
				msg.category = eDbgCategory_State_Getting;
				break;
			case D3D11_MESSAGE_CATEGORY_RESOURCE_MANIPULATION:
				msg.category = eDbgCategory_Resource_Manipulation;
				break;
			case D3D11_MESSAGE_CATEGORY_EXECUTION:
				msg.category = eDbgCategory_Execution;
				break;
#if defined(INCLUDE_D3D_11_1)
			case D3D11_MESSAGE_CATEGORY_SHADER:
				msg.category = eDbgCategory_Shaders;
				break;
#endif
			default:
				RDCWARN("Unexpected message category: %d", message->Category);
				break;
		}

		switch(message->Severity)
		{
			case D3D11_MESSAGE_SEVERITY_CORRUPTION:
				msg.severity = eDbgSeverity_Corruption;
				break;
			case D3D11_MESSAGE_SEVERITY_ERROR:
				msg.severity = eDbgSeverity_Error;
				break;
			case D3D11_MESSAGE_SEVERITY_WARNING:
				msg.severity = eDbgSeverity_Warning;
				break;
			case D3D11_MESSAGE_SEVERITY_INFO:
				msg.severity = eDbgSeverity_Info;
				break;
#if defined(INCLUDE_D3D_11_1)
			case D3D11_MESSAGE_SEVERITY_MESSAGE:
				msg.severity = eDbgSeverity_Info;
				break;
#endif
			default:
				RDCWARN("Unexpected message severity: %d", message->Severity);
				break;
		}

		msg.messageID = (uint32_t)message->ID;
		msg.description = string(message->pDescription);

		ret.push_back(msg);

		SAFE_DELETE_ARRAY(msgbuf);
	}

	// Docs are fuzzy on the thread safety of the info queue, but I'm going to assume it should only
	// ever be accessed on one thread since it's tied to the device & immediate context.
	// There doesn't seem to be a way to lock it for access and without that there's no way to know
	// that a new message won't be added between the time you retrieve the last one and clearing the
	// queue. There is also no way to pop a message that I can see, which would presumably be the
	// best way if its member functions are thread safe themselves (if the queue is protected internally).
	RDCASSERT(numMessages == m_pInfoQueue->GetNumStoredMessagesAllowedByRetrievalFilter());

	m_pInfoQueue->ClearStoredMessages();

	return ret;
}

void WrappedID3D11Device::ProcessChunk(uint64_t offset, D3D11ChunkType context)
{
	switch(context)
	{
	case DEVICE_INIT:
		{
			SERIALISE_ELEMENT(ResourceId, immContextId, ResourceId());

			// add a reference for the resource manager - normally it takes ownership of the resource on creation and releases it
			// to destruction, but we want to control our immediate context ourselves.
			m_pImmediateContext->AddRef(); 
			m_ResourceManager->AddLiveResource(immContextId, m_pImmediateContext);
			break;
		}
	case SET_RESOURCE_NAME:
		Serialise_SetResourceName(0x0, "");
		break;
	case RELEASE_RESOURCE:
		Serialise_ReleaseResource(0x0);
		break;
	case CREATE_SWAP_BUFFER:
		Serialise_SetSwapChainTexture(0x0, 0x0, 0, 0x0);
		break;
	case CREATE_TEXTURE_1D:
		Serialise_CreateTexture1D(0x0, 0x0, 0x0);
		break;
	case CREATE_TEXTURE_2D:
		Serialise_CreateTexture2D(0x0, 0x0, 0x0);
		break;
	case CREATE_TEXTURE_3D:
		Serialise_CreateTexture3D(0x0, 0x0, 0x0);
		break;
	case CREATE_BUFFER:
		Serialise_CreateBuffer(0x0, 0x0, 0x0);
		break;
	case CREATE_VERTEX_SHADER:
		Serialise_CreateVertexShader(0x0, 0, 0x0, 0x0);
		break;
	case CREATE_HULL_SHADER:
		Serialise_CreateHullShader(0x0, 0, 0x0, 0x0);
		break;
	case CREATE_DOMAIN_SHADER:
		Serialise_CreateDomainShader(0x0, 0, 0x0, 0x0);
		break;
	case CREATE_GEOMETRY_SHADER:
		Serialise_CreateGeometryShader(0x0, 0, 0x0, 0x0);
		break;
	case CREATE_GEOMETRY_SHADER_WITH_SO:
		Serialise_CreateGeometryShaderWithStreamOutput(0x0, 0, 0x0, 0, 0x0, 0, 0, 0x0, 0x0);
		break;
	case CREATE_PIXEL_SHADER:
		Serialise_CreatePixelShader(0x0, 0, 0x0, 0x0);
		break;
	case CREATE_COMPUTE_SHADER:
		Serialise_CreateComputeShader(0x0, 0, 0x0, 0x0);
		break;
	case GET_CLASS_INSTANCE:
		Serialise_GetClassInstance(0x0, 0, 0x0, 0x0);
		break;
	case CREATE_CLASS_INSTANCE:
		Serialise_CreateClassInstance(0x0, 0, 0, 0, 0, 0x0, 0x0);
		break;
	case CREATE_CLASS_LINKAGE:
		Serialise_CreateClassLinkage(0x0);
		break;
	case CREATE_SRV:
		Serialise_CreateShaderResourceView(0x0, 0x0, 0x0);
		break;
	case CREATE_RTV:
		Serialise_CreateRenderTargetView(0x0, 0x0, 0x0);
		break;
	case CREATE_DSV:
		Serialise_CreateDepthStencilView(0x0, 0x0, 0x0);
		break;
	case CREATE_UAV:
		Serialise_CreateUnorderedAccessView(0x0, 0x0, 0x0);
		break;
	case CREATE_INPUT_LAYOUT:
		Serialise_CreateInputLayout(0x0, 0, 0x0, 0, 0x0);
		break;
	case CREATE_BLEND_STATE:
		Serialise_CreateBlendState(0x0, 0x0);
		break;
	case CREATE_DEPTHSTENCIL_STATE:
		Serialise_CreateDepthStencilState(0x0, 0x0);
		break;
	case CREATE_RASTER_STATE:
		Serialise_CreateRasterizerState(0x0, 0x0);
		break;
#if defined(INCLUDE_D3D_11_1)
	case CREATE_BLEND_STATE1:
		Serialise_CreateBlendState1(0x0, 0x0);
		break;
	case CREATE_RASTER_STATE1:
		Serialise_CreateRasterizerState1(0x0, 0x0);
		break;
#else
	case CREATE_BLEND_STATE1:
	case CREATE_RASTER_STATE1:
		RDCERR("Replaying log with D3D11.1 events on a build without D3D11.1 support");
		break;
#endif
	case CREATE_SAMPLER_STATE:
		Serialise_CreateSamplerState(0x0, 0x0);
		break;
	case CREATE_QUERY:
		Serialise_CreateQuery(0x0, 0x0);
		break;
	case CREATE_PREDICATE:
		Serialise_CreatePredicate(0x0, 0x0);
		break;
	case CREATE_COUNTER:
		Serialise_CreateCounter(0x0, 0x0);
		break;
	case CREATE_DEFERRED_CONTEXT:
		Serialise_CreateDeferredContext(0, 0x0);
		break;
	case SET_EXCEPTION_MODE:
		Serialise_SetExceptionMode(0);
		break;
	case OPEN_SHARED_RESOURCE:
	{
		IID nul;
		Serialise_OpenSharedResource(0, nul, NULL);
		break;
	}
	case CAPTURE_SCOPE:
		Serialise_CaptureScope(offset);
		break;
	default:
		// ignore system chunks
		if(context == INITIAL_CONTENTS)
			Serialise_InitialState(NULL);
		else if(context < FIRST_CHUNK_ID)
			m_pSerialiser->SkipCurrentChunk();
		else
			m_pImmediateContext->ProcessChunk(offset, context, true);
		break;
	}
}

void WrappedID3D11Device::Serialise_CaptureScope(uint64_t offset)
{
	SERIALISE_ELEMENT(uint32_t, FrameNumber, m_FrameCounter);

	if(m_State >= WRITING)
	{
		GetResourceManager()->Serialise_InitialContentsNeeded(m_pSerialiser);
	}
	else
	{
		FetchFrameRecord record;
		record.frameInfo.fileOffset = offset;
		record.frameInfo.firstEvent = m_pImmediateContext->GetEventID();
		record.frameInfo.frameNumber = FrameNumber;
		record.frameInfo.immContextId = GetResourceManager()->GetOriginalID(m_pImmediateContext->GetResourceID());
		m_FrameRecord.push_back(record);

		GetResourceManager()->CreateInitialContents(m_pSerialiser);
	}
}

void WrappedID3D11Device::ReadLogInitialisation()
{
	uint64_t lastFrame = 0;
	uint64_t firstFrame = 0;

	LazyInit();

	m_pSerialiser->SetDebugText(true);

	m_pSerialiser->Rewind();

	while(!m_pSerialiser->AtEnd())
	{
		m_pSerialiser->SkipToChunk(CAPTURE_SCOPE);

		// found a capture chunk
		if(!m_pSerialiser->AtEnd())
		{
			lastFrame = m_pSerialiser->GetOffset();
			if(firstFrame == 0)
				firstFrame = m_pSerialiser->GetOffset();

			// skip this chunk
			m_pSerialiser->PushContext(NULL, CAPTURE_SCOPE, false);
			m_pSerialiser->SkipCurrentChunk();
			m_pSerialiser->PopContext(NULL, CAPTURE_SCOPE);
		}
	}

	m_pSerialiser->Rewind();

	int chunkIdx = 0;

	struct chunkinfo
	{
		chunkinfo() : count(0), total(0.0) {}
		int count;
		double total;
	};

	map<D3D11ChunkType,chunkinfo> chunkInfos;

	SCOPED_TIMER("chunk initialisation");

	while(1)
	{
		PerformanceTimer timer;

		uint64_t offset = m_pSerialiser->GetOffset();

		D3D11ChunkType context = (D3D11ChunkType)m_pSerialiser->PushContext(NULL, 1, false);

		chunkIdx++;

		ProcessChunk(offset, context);

		m_pSerialiser->PopContext(NULL, context);
		
		RenderDoc::Inst().SetProgress(FileInitialRead, float(m_pSerialiser->GetOffset())/float(m_pSerialiser->GetSize()));

		if(context == CAPTURE_SCOPE)
		{
			m_pImmediateContext->ReplayLog(READING, 0, 0, false);

			if(m_pSerialiser->GetOffset() > lastFrame)
				break;
		}

		chunkInfos[context].total += timer.GetMilliseconds();
		chunkInfos[context].count++;

		if(m_pSerialiser->AtEnd())
		{
			break;
		}
	}

	for(auto it=chunkInfos.begin(); it != chunkInfos.end(); ++it)
	{
		RDCDEBUG("%hs: %.3f total time in %d chunks - %.3f average",
				GetChunkName(it->first), it->second.total, it->second.count,
				it->second.total/double(it->second.count));
	}

	RDCDEBUG("Allocating %llu persistant bytes of memory for the log.", m_pSerialiser->GetSize() - firstFrame);
	
	m_pSerialiser->SetDebugText(false);
	
	m_pSerialiser->SetBase(firstFrame);
}

bool WrappedID3D11Device::Prepare_InitialState(ID3D11DeviceChild *res)
{
	ResourceType type = IdentifyTypeByPtr(res);
	ResourceId Id = GetIDForResource(res);

	RDCASSERT(m_State >= WRITING);
	
	{
		RDCDEBUG("Prepare_InitialState(%llu)", Id);

		if(type == Resource_Buffer)
			RDCDEBUG("    .. buffer");
		else if(type == Resource_UnorderedAccessView)
			RDCDEBUG("    .. UAV");
		else if(type == Resource_Texture1D ||
				type == Resource_Texture2D ||
				type == Resource_Texture3D)
		{
			if(type == Resource_Texture1D)
				RDCDEBUG("    .. tex1d");
			else if(type == Resource_Texture2D)
				RDCDEBUG("    .. tex2d");
			else if(type == Resource_Texture3D)
				RDCDEBUG("    .. tex3d");
		}
		else
			RDCERR("    .. other!");
	}

	if(type == Resource_UnorderedAccessView)
	{
		WrappedID3D11UnorderedAccessView *uav = (WrappedID3D11UnorderedAccessView *)res;

		D3D11_UNORDERED_ACCESS_VIEW_DESC udesc;
		uav->GetDesc(&udesc);
		
		if(udesc.ViewDimension == D3D11_UAV_DIMENSION_BUFFER &&
			(udesc.Buffer.Flags & (D3D11_BUFFER_UAV_FLAG_COUNTER|D3D11_BUFFER_UAV_FLAG_APPEND)) != 0)
		{
			ID3D11Buffer *stage = NULL;

			D3D11_BUFFER_DESC desc;
			desc.BindFlags = 0;
			desc.ByteWidth = 16;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			desc.Usage = D3D11_USAGE_STAGING;
			HRESULT hr = m_pDevice->CreateBuffer(&desc, NULL, &stage);

			if(FAILED(hr) || stage == NULL)
			{
				RDCERR("Failed to create staging buffer for UAV initial contents %08x", hr);
			}
			else
			{
				m_pImmediateContext->GetReal()->CopyStructureCount(stage, 0, UNWRAP(WrappedID3D11UnorderedAccessView, uav));

				m_ResourceManager->SetInitialContents(Id, stage, 0);
			}
		}
	}
	else if(type == Resource_Buffer)
	{
		WrappedID3D11Buffer *buf = (WrappedID3D11Buffer *)res;
		D3D11ResourceRecord *record = m_ResourceManager->GetResourceRecord(Id);

		ID3D11Buffer *stage = NULL;

		D3D11_BUFFER_DESC desc;
		desc.BindFlags = 0;
		desc.ByteWidth = record->Length;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		desc.Usage = D3D11_USAGE_STAGING;
		HRESULT hr = m_pDevice->CreateBuffer(&desc, NULL, &stage);

		if(FAILED(hr) || stage == NULL)
		{
			RDCERR("Failed to create staging buffer for buffer initial contents %08x", hr);
		}
		else
		{
			m_pImmediateContext->GetReal()->CopyResource(stage, UNWRAP(WrappedID3D11Buffer, buf));

			m_ResourceManager->SetInitialContents(Id, stage, 0);
		}
	}
	else if(type == Resource_Texture1D)
	{
		WrappedID3D11Texture1D *tex1D = (WrappedID3D11Texture1D *)res;
		D3D11ResourceRecord *record = m_ResourceManager->GetResourceRecord(Id);

		D3D11_TEXTURE1D_DESC desc;
		tex1D->GetDesc(&desc);

		UINT numSubresources = desc.MipLevels*desc.ArraySize;

		D3D11_TEXTURE1D_DESC stageDesc = desc;
		ID3D11Texture1D *stage = NULL;

		stageDesc.MiscFlags = 0;
		stageDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		stageDesc.BindFlags = 0;
		stageDesc.Usage = D3D11_USAGE_STAGING;

		HRESULT hr = m_pDevice->CreateTexture1D(&stageDesc, NULL, &stage);

		if(FAILED(hr))
		{
			RDCERR("Failed to create initial tex1D %08x", hr);
		}
		else
		{
			m_pImmediateContext->GetReal()->CopyResource(stage, UNWRAP(WrappedID3D11Texture1D, tex1D));

			m_ResourceManager->SetInitialContents(Id, stage, 0);
		}
	}
	else if(type == Resource_Texture2D)
	{
		WrappedID3D11Texture2D *tex2D = (WrappedID3D11Texture2D *)res;
		D3D11ResourceRecord *record = m_ResourceManager->GetResourceRecord(Id);

		D3D11_TEXTURE2D_DESC desc;
		tex2D->GetDesc(&desc);

		UINT numSubresources = desc.MipLevels*desc.ArraySize;

		bool multisampled = desc.SampleDesc.Count > 1 || desc.SampleDesc.Quality > 0;

		D3D11_TEXTURE2D_DESC stageDesc = desc;
		ID3D11Texture2D *stage = NULL;

		stageDesc.MiscFlags = 0;
		stageDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		stageDesc.BindFlags = 0;
		stageDesc.Usage = D3D11_USAGE_STAGING;

		// expand out each sample into an array slice. Hope
		// that this doesn't blow over the array size limit
		// (that would be pretty insane)
		if(multisampled)
		{
			stageDesc.SampleDesc.Count = 1;
			stageDesc.SampleDesc.Quality = 0;
			stageDesc.ArraySize *= desc.SampleDesc.Count;
		}

		HRESULT hr = m_pDevice->CreateTexture2D(&stageDesc, NULL, &stage);

		if(FAILED(hr))
		{
			RDCERR("Failed to create initial tex2D %08x", hr);
		}
		else
		{
			IDXGIKeyedMutex *mutex = NULL;
			
			if(desc.MiscFlags & D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX)
			{
				HRESULT hr = UNWRAP(WrappedID3D11Texture2D, tex2D)->QueryInterface(__uuidof(IDXGIKeyedMutex), (void **)&mutex);

				if(SUCCEEDED(hr) && mutex)
				{
					// complete guess but let's try and acquire key 0 so we can cop this texture out.
					mutex->AcquireSync(0, 10);

					// if it failed, give up. Otherwise we can release the sync below
					if(FAILED(hr))
						SAFE_RELEASE(mutex);
				}
				else
				{
					SAFE_RELEASE(mutex);
				}
			}

			if(multisampled)
				m_DebugManager->CopyTex2DMSToArray(stage, UNWRAP(WrappedID3D11Texture2D, tex2D));
			else
				m_pImmediateContext->GetReal()->CopyResource(stage, UNWRAP(WrappedID3D11Texture2D, tex2D));

			m_pImmediateContext->GetReal()->Flush();
			
			if(mutex)
			{
				mutex->ReleaseSync(0);

				SAFE_RELEASE(mutex);
			}

			m_ResourceManager->SetInitialContents(Id, stage, 0);
		}
	}
	else if(type == Resource_Texture3D)
	{
		WrappedID3D11Texture3D *tex3D = (WrappedID3D11Texture3D *)res;
		D3D11ResourceRecord *record = m_ResourceManager->GetResourceRecord(Id);

		D3D11_TEXTURE3D_DESC desc;
		tex3D->GetDesc(&desc);

		UINT numSubresources = desc.MipLevels;

		D3D11_TEXTURE3D_DESC stageDesc = desc;
		ID3D11Texture3D *stage = NULL;

		stageDesc.MiscFlags = 0;
		stageDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		stageDesc.BindFlags = 0;
		stageDesc.Usage = D3D11_USAGE_STAGING;

		HRESULT hr = m_pDevice->CreateTexture3D(&stageDesc, NULL, &stage);

		if(FAILED(hr))
		{
			RDCERR("Failed to create initial tex3D %08x", hr);
		}
		else
		{
			m_pImmediateContext->GetReal()->CopyResource(stage, UNWRAP(WrappedID3D11Texture3D, tex3D));

			m_ResourceManager->SetInitialContents(Id, stage, 0);
		}
	}

	return true;
}

bool WrappedID3D11Device::Serialise_InitialState(ID3D11DeviceChild *res)
{
	ResourceType type = Resource_Unknown;
	ResourceId Id = ResourceId();

	if(m_State >= WRITING)
	{
		type = IdentifyTypeByPtr(res);
		Id = GetIDForResource(res);

		if(type != Resource_Buffer)
		{
			m_pSerialiser->Serialise("type", type);
			m_pSerialiser->Serialise("Id", Id);
		}
	}
	else
	{
		m_pSerialiser->Serialise("type", type);
		m_pSerialiser->Serialise("Id", Id);
	}
	
	{
		RDCDEBUG("Serialise_InitialState(%llu)", Id);

		if(type == Resource_Buffer)
			RDCDEBUG("    .. buffer");
		else if(type == Resource_UnorderedAccessView)
			RDCDEBUG("    .. UAV");
		else if(type == Resource_Texture1D ||
				type == Resource_Texture2D ||
				type == Resource_Texture3D)
		{
			if(type == Resource_Texture1D)
				RDCDEBUG("    .. tex1d");
			else if(type == Resource_Texture2D)
				RDCDEBUG("    .. tex2d");
			else if(type == Resource_Texture3D)
				RDCDEBUG("    .. tex3d");
		}
		else
			RDCERR("    .. other!");
	}

	if(type == Resource_UnorderedAccessView)
	{
		WrappedID3D11UnorderedAccessView *uav = (WrappedID3D11UnorderedAccessView *)res;
		if(m_State < WRITING)
		{
			if(m_ResourceManager->HasLiveResource(Id))
			{
				uav = (WrappedID3D11UnorderedAccessView *)m_ResourceManager->GetLiveResource(Id);
			}
			else
			{
				uav = NULL;
				SERIALISE_ELEMENT(uint32_t, initCount, 0);
				return true;
			}
		}
		
		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		uav->GetDesc(&desc);

		if(desc.ViewDimension == D3D11_UAV_DIMENSION_BUFFER &&
			(desc.Buffer.Flags & (D3D11_BUFFER_UAV_FLAG_COUNTER|D3D11_BUFFER_UAV_FLAG_APPEND)) != 0)
		{
			if(m_State >= WRITING)
			{
				ID3D11Buffer *stage = (ID3D11Buffer *)m_ResourceManager->GetInitialContents(Id);

				D3D11_MAPPED_SUBRESOURCE mapped;
				HRESULT hr = m_pImmediateContext->GetReal()->Map(stage, 0, D3D11_MAP_READ, 0, &mapped);

				uint32_t countData = 0;

				if(FAILED(hr))
				{
					RDCERR("Failed to map while getting initial states %08x", hr);
				}
				else
				{
					countData = *((uint32_t *)mapped.pData);

					m_pImmediateContext->GetReal()->Unmap(stage, 0);
				}

				SERIALISE_ELEMENT(uint32_t, count, countData);
			}
			else
			{
				SERIALISE_ELEMENT(uint32_t, initCount, 0);

				m_ResourceManager->SetInitialContents(Id, NULL, initCount);
			}
		}
		else
		{
			SERIALISE_ELEMENT(uint32_t, initCount, 0);
		}
	}
	else if(type == Resource_Buffer)
	{
		if(m_State >= WRITING)
		{
			WrappedID3D11Buffer *buf = (WrappedID3D11Buffer *)res;
			D3D11ResourceRecord *record = m_ResourceManager->GetResourceRecord(Id);

			D3D11_BUFFER_DESC desc;
			desc.BindFlags = 0;
			desc.ByteWidth = record->Length;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;

			ID3D11Buffer *stage = (ID3D11Buffer *)m_ResourceManager->GetInitialContents(Id);

			D3D11_MAPPED_SUBRESOURCE mapped;
			HRESULT hr = m_pImmediateContext->GetReal()->Map(stage, 0, D3D11_MAP_READ, 0, &mapped);

			if(FAILED(hr))
			{
				RDCERR("Failed to map while getting initial states %08x", hr);
			}
			else
			{
				RDCASSERT(record->DataInSerialiser);

				MapIntercept intercept;
				intercept.SetD3D(mapped);
				intercept.Init(buf, record->GetDataPtr());
				intercept.CopyFromD3D();

				m_pImmediateContext->GetReal()->Unmap(stage, 0);
			}
		}
	}
	else if(type == Resource_Texture1D)
	{
		WrappedID3D11Texture1D *tex1D = (WrappedID3D11Texture1D *)res;
		if(m_State < WRITING)
			tex1D = (WrappedID3D11Texture1D *)m_ResourceManager->GetLiveResource(Id);
		
		D3D11ResourceRecord *record = NULL;
		if(m_State >= WRITING)
			record = m_ResourceManager->GetResourceRecord(Id);

		D3D11_TEXTURE1D_DESC desc;
		tex1D->GetDesc(&desc);
		
		SERIALISE_ELEMENT(uint32_t, numSubresources, desc.MipLevels*desc.ArraySize);
		
		{
			if(m_State < WRITING)
			{
				ID3D11Texture1D *contents = (ID3D11Texture1D *)m_ResourceManager->GetInitialContents(Id);

				RDCASSERT(!contents);
			}

			byte *inmemBuffer = NULL;
			D3D11_SUBRESOURCE_DATA *subData = NULL;

			if(m_State >= WRITING)
			{
				inmemBuffer = new byte[GetByteSize(desc.Width, 1, 1, desc.Format, 0)];
			}
			else
			{
				subData = new D3D11_SUBRESOURCE_DATA[numSubresources];
			}

			ID3D11Texture1D *stage = (ID3D11Texture1D *)m_ResourceManager->GetInitialContents(Id);

			for(UINT sub = 0; sub < numSubresources; sub++)
			{
				UINT mip = GetMipForSubresource(tex1D, sub);
				
				if(m_State >= WRITING)
				{
					D3D11_MAPPED_SUBRESOURCE mapped;

					HRESULT hr = m_pImmediateContext->GetReal()->Map(stage, sub, D3D11_MAP_READ, 0, &mapped);

					size_t dstPitch = GetByteSize(desc.Width, 1, 1, desc.Format, mip);

					if(FAILED(hr))
					{
						RDCERR("Failed to map in initial states %08x", hr);
					}
					else
					{
						uint32_t rowsPerLine = 1;

						byte *dst = inmemBuffer;
						byte *src = (byte *)mapped.pData;

						memcpy(inmemBuffer, mapped.pData, dstPitch);
					}

					size_t len = dstPitch;
					m_pSerialiser->SerialiseBuffer("", inmemBuffer, len);

					if(SUCCEEDED(hr))
						m_pImmediateContext->GetReal()->Unmap(stage, 0);
				}
				else
				{
					byte *data = NULL;
					size_t len = 0;
					m_pSerialiser->SerialiseBuffer("", data, len);

					subData[sub].pSysMem = data;
					subData[sub].SysMemPitch = GetByteSize(desc.Width, 1, 1, desc.Format, mip);
					subData[sub].SysMemSlicePitch = GetByteSize(desc.Width, 1, 1, desc.Format, mip);
				}
			}

			SAFE_DELETE_ARRAY(inmemBuffer);
			
			if(m_State < WRITING)
			{
				// We don't need to bind this, but IMMUTABLE requires at least one
				// BindFlags.
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				desc.CPUAccessFlags = 0;
				desc.Usage = D3D11_USAGE_IMMUTABLE;
				desc.MiscFlags = 0;

				ID3D11Texture1D *contents = NULL;
				HRESULT hr = m_pDevice->CreateTexture1D(&desc, subData, &contents);

				if(FAILED(hr) || contents == NULL)
				{
					RDCERR("Failed to create staging resource for Texture1D initial contents %08x", hr);
				}
				else
				{
					m_ResourceManager->SetInitialContents(Id, contents, 0);
				}

				for(UINT sub = 0; sub < numSubresources; sub++)
					SAFE_DELETE_ARRAY(subData[sub].pSysMem);
				SAFE_DELETE_ARRAY(subData);
			}
		}
	}
	else if(type == Resource_Texture2D)
	{
		WrappedID3D11Texture2D *tex2D = (WrappedID3D11Texture2D *)res;
		if(m_State < WRITING)
			tex2D = (WrappedID3D11Texture2D *)m_ResourceManager->GetLiveResource(Id);
		
		D3D11ResourceRecord *record = NULL;
		if(m_State >= WRITING)
			record = m_ResourceManager->GetResourceRecord(Id);

		D3D11_TEXTURE2D_DESC desc;
		tex2D->GetDesc(&desc);

		SERIALISE_ELEMENT(uint32_t, numSubresources, desc.MipLevels*desc.ArraySize);

		bool bigrt = ((desc.BindFlags & D3D11_BIND_RENDER_TARGET) != 0 ||
					  (desc.BindFlags & D3D11_BIND_DEPTH_STENCIL) != 0 ||
					  (desc.BindFlags & D3D11_BIND_UNORDERED_ACCESS) != 0) && (desc.Width > 64 && desc.Height > 64);

		if(bigrt && m_ResourceManager->ReadBeforeWrite(Id))
			bigrt = false;
		
		bool multisampled = desc.SampleDesc.Count > 1 || desc.SampleDesc.Quality > 0;

		if(multisampled)
			numSubresources *= desc.SampleDesc.Count;

		SERIALISE_ELEMENT(bool, omitted, bigrt && !RenderDoc::Inst().GetCaptureOptions().SaveAllInitials);

		if(omitted)
		{
			if(m_State >= WRITING)
			{
				RDCWARN("Not serialising texture 2D initial state. ID %llu", Id);
				if(bigrt)
					RDCWARN("Detected Write before Read of this target - assuming initial contents are unneeded.\n" \
							"Capture again with Save All Initials if this is wrong");
			}
		}
		else
		{
			if(m_State < WRITING)
			{
				ID3D11Texture2D *contents = (ID3D11Texture2D *)m_ResourceManager->GetInitialContents(Id);

				RDCASSERT(!contents);
			}

			byte *inmemBuffer = NULL;
			D3D11_SUBRESOURCE_DATA *subData = NULL;

			if(m_State >= WRITING)
			{
				inmemBuffer = new byte[GetByteSize(desc.Width, desc.Height, 1, desc.Format, 0)];
			}
			else
			{
				subData = new D3D11_SUBRESOURCE_DATA[numSubresources];
			}

			ID3D11Texture2D *stage = (ID3D11Texture2D *)m_ResourceManager->GetInitialContents(Id);

			for(UINT sub = 0; sub < numSubresources; sub++)
			{
				UINT mip = GetMipForSubresource(tex2D, sub);
				
				if(m_State >= WRITING)
				{
					D3D11_MAPPED_SUBRESOURCE mapped;

					HRESULT hr = m_pImmediateContext->GetReal()->Map(stage, sub, D3D11_MAP_READ, 0, &mapped);
					
					size_t dstPitch = GetByteSize(desc.Width, 1, 1, desc.Format, mip);
					size_t len = GetByteSize(desc.Width, desc.Height, 1, desc.Format, mip);

					uint32_t rowsPerLine = 1;
					if(IsBlockFormat(desc.Format))
						rowsPerLine = 4;
					
					if(FAILED(hr))
					{
						RDCERR("Failed to map in initial states %08x", hr);
					}
					else
					{
						byte *dst = inmemBuffer;
						byte *src = (byte *)mapped.pData;
						for(uint32_t row=0; row < desc.Height>>mip; row += rowsPerLine)
						{
							memcpy(dst, src, dstPitch);
							dst += dstPitch;
							src += mapped.RowPitch;
						}
					}

					m_pSerialiser->SerialiseBuffer("", inmemBuffer, len);

					m_pImmediateContext->GetReal()->Unmap(stage, sub);
				}
				else
				{
					byte *data = NULL;
					size_t len = 0;
					m_pSerialiser->SerialiseBuffer("", data, len);

					subData[sub].pSysMem = data;
					subData[sub].SysMemPitch = GetByteSize(desc.Width, 1, 1, desc.Format, mip);
					subData[sub].SysMemSlicePitch = GetByteSize(desc.Width, desc.Height, 1, desc.Format, mip);
				}
			}

			SAFE_DELETE_ARRAY(inmemBuffer);
			
			if(m_State < WRITING)
			{
				// We don't need to bind this, but IMMUTABLE requires at least one
				// BindFlags.
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				desc.CPUAccessFlags = 0;
				desc.MiscFlags = 0;

				switch(desc.Format)
				{
					case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
						desc.Format = DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS;
						break;
					case DXGI_FORMAT_D32_FLOAT:
						desc.Format = DXGI_FORMAT_R32_FLOAT;
						break;
					case DXGI_FORMAT_D24_UNORM_S8_UINT:
						desc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
						break;
					case DXGI_FORMAT_D16_UNORM:
						desc.Format = DXGI_FORMAT_R16_FLOAT;
						break;
					default:
						break;
				}

				D3D11_TEXTURE2D_DESC initialDesc = desc;
				// if multisampled, need to upload subData into an array with slices for each sample.
				if(multisampled)
				{
					initialDesc.SampleDesc.Count = 1;
					initialDesc.SampleDesc.Quality = 0;
					initialDesc.ArraySize *= desc.SampleDesc.Count;
				}
				
				initialDesc.Usage = D3D11_USAGE_IMMUTABLE;

				ID3D11Texture2D *contents = NULL;
				HRESULT hr = m_pDevice->CreateTexture2D(&initialDesc, subData, &contents);

				if(FAILED(hr) || contents == NULL)
				{
					RDCERR("Failed to create staging resource for Texture2D initial contents %08x", hr);
				}
				else
				{
					// if multisampled, contents is actually an array with slices for each sample.
					// need to copy back out to a real multisampled resource
					if(multisampled)
					{
						desc.BindFlags = IsDepthFormat(desc.Format) ? D3D11_BIND_DEPTH_STENCIL : D3D11_BIND_RENDER_TARGET;

						if(IsDepthFormat(desc.Format))
							desc.Format = GetDepthTypedFormat(desc.Format);

						ID3D11Texture2D *contentsMS = NULL;
						HRESULT hr = m_pDevice->CreateTexture2D(&desc, NULL, &contentsMS);
						
						m_DebugManager->CopyArrayToTex2DMS(contentsMS, contents);

						SAFE_RELEASE(contents);
						contents = contentsMS;
					}

					m_ResourceManager->SetInitialContents(Id, contents, 0);
				}
				
				for(UINT sub = 0; sub < numSubresources; sub++)
					SAFE_DELETE_ARRAY(subData[sub].pSysMem);
				SAFE_DELETE_ARRAY(subData);
			}
		}
	}
	else if(type == Resource_Texture3D)
	{
		WrappedID3D11Texture3D *tex3D = (WrappedID3D11Texture3D *)res;
		if(m_State < WRITING)
			tex3D = (WrappedID3D11Texture3D *)m_ResourceManager->GetLiveResource(Id);
		
		D3D11ResourceRecord *record = NULL;
		if(m_State >= WRITING)
			record = m_ResourceManager->GetResourceRecord(Id);

		D3D11_TEXTURE3D_DESC desc;
		tex3D->GetDesc(&desc);
		
		SERIALISE_ELEMENT(uint32_t, numSubresources, desc.MipLevels);
		
		{
			if(m_State < WRITING)
			{
				ID3D11Texture3D *contents = (ID3D11Texture3D *)m_ResourceManager->GetInitialContents(Id);

				RDCASSERT(!contents);
			}

			byte *inmemBuffer = NULL;
			D3D11_SUBRESOURCE_DATA *subData = NULL;

			if(m_State >= WRITING)
			{
				inmemBuffer = new byte[GetByteSize(desc.Width, desc.Height, desc.Depth, desc.Format, 0)];
			}
			else
			{
				subData = new D3D11_SUBRESOURCE_DATA[numSubresources];
			}

			ID3D11Texture3D *stage = (ID3D11Texture3D *)m_ResourceManager->GetInitialContents(Id);

			for(UINT sub = 0; sub < numSubresources; sub++)
			{
				UINT mip = GetMipForSubresource(tex3D, sub);
				
				if(m_State >= WRITING)
				{
					D3D11_MAPPED_SUBRESOURCE mapped;

					HRESULT hr = m_pImmediateContext->GetReal()->Map(stage, sub, D3D11_MAP_READ, 0, &mapped);
					
					size_t dstPitch = GetByteSize(desc.Width, 1, 1, desc.Format, mip);
					size_t dstSlicePitch = GetByteSize(desc.Width, desc.Height, 1, desc.Format, mip);

					uint32_t rowsPerLine = 1;
					if(IsBlockFormat(desc.Format))
						rowsPerLine = 4;
					
					if(FAILED(hr))
					{
						RDCERR("Failed to map in initial states %08x", hr);
					}
					else
					{
						byte *dst = inmemBuffer;
						byte *src = (byte *)mapped.pData;

						for(uint32_t slice=0; slice < RDCMAX(1U,desc.Depth>>mip); slice++)
						{
							byte *sliceDst = dst;
							byte *sliceSrc = src;

							for(uint32_t row=0; row < RDCMAX(1U,desc.Height>>mip); row += rowsPerLine)
							{
								memcpy(sliceDst, sliceSrc, dstPitch);
								sliceDst += dstPitch;
								sliceSrc += mapped.RowPitch;
							}

							dst += dstSlicePitch;
							src += mapped.DepthPitch;
						}
					}

					size_t len = dstSlicePitch*desc.Depth;
					m_pSerialiser->SerialiseBuffer("", inmemBuffer, len);

					m_pImmediateContext->GetReal()->Unmap(stage, 0);
				}
				else
				{
					byte *data = NULL;
					size_t len = 0;
					m_pSerialiser->SerialiseBuffer("", data, len);

					subData[sub].pSysMem = data;
					subData[sub].SysMemPitch = GetByteSize(desc.Width, 1, 1, desc.Format, mip);
					subData[sub].SysMemSlicePitch = GetByteSize(desc.Width, desc.Height, 1, desc.Format, mip);
				}
			}

			SAFE_DELETE_ARRAY(inmemBuffer);
			
			if(m_State < WRITING)
			{
				// We don't need to bind this, but IMMUTABLE requires at least one
				// BindFlags.
				desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
				desc.CPUAccessFlags = 0;
				desc.Usage = D3D11_USAGE_IMMUTABLE;
				desc.MiscFlags = 0;

				ID3D11Texture3D *contents = NULL;
				HRESULT hr = m_pDevice->CreateTexture3D(&desc, subData, &contents);

				if(FAILED(hr) || contents == NULL)
				{
					RDCERR("Failed to create staging resource for Texture3D initial contents %08x", hr);
				}
				else
				{
					m_ResourceManager->SetInitialContents(Id, contents, 0);
				}
				
				for(UINT sub = 0; sub < numSubresources; sub++)
					SAFE_DELETE_ARRAY(subData[sub].pSysMem);
				SAFE_DELETE_ARRAY(subData);
			}
		}
	}
	else
	{
		RDCERR("Trying to serialise initial state of unsupported resource type");
	}

	return true;
}

void WrappedID3D11Device::Create_InitialState(ResourceId id, ID3D11DeviceChild *live, bool hasData)
{
	ResourceType type = IdentifyTypeByPtr(live);
	
	{
		RDCDEBUG("Create_InitialState(%llu)", id);

		if(type == Resource_Buffer)
			RDCDEBUG("    .. buffer");
		else if(type == Resource_UnorderedAccessView)
			RDCDEBUG("    .. UAV");
		else if(type == Resource_Texture1D ||
				type == Resource_Texture2D ||
				type == Resource_Texture3D)
		{
			if(type == Resource_Texture1D)
				RDCDEBUG("    .. tex1d");
			else if(type == Resource_Texture2D)
				RDCDEBUG("    .. tex2d");
			else if(type == Resource_Texture3D)
				RDCDEBUG("    .. tex3d");
		}
		else
			RDCERR("    .. other!");
	}

	if(type == Resource_UnorderedAccessView)
	{
		WrappedID3D11UnorderedAccessView *uav = (WrappedID3D11UnorderedAccessView *)live;

		D3D11_UNORDERED_ACCESS_VIEW_DESC desc;
		uav->GetDesc(&desc);

		if(desc.ViewDimension == D3D11_UAV_DIMENSION_BUFFER &&
			(desc.Buffer.Flags & (D3D11_BUFFER_UAV_FLAG_COUNTER|D3D11_BUFFER_UAV_FLAG_APPEND)) != 0)
		{
			ID3D11Buffer *stage = NULL;

			D3D11_BUFFER_DESC desc;
			desc.BindFlags = 0;
			desc.ByteWidth = 16;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;
			desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
			desc.Usage = D3D11_USAGE_STAGING;
			HRESULT hr = m_pDevice->CreateBuffer(&desc, NULL, &stage);

			if(FAILED(hr) || stage == NULL)
			{
				RDCERR("Failed to create staging resource for UAV initial contents %08x", hr);
			}
			else
			{
				m_pImmediateContext->GetReal()->CopyStructureCount(stage, 0, UNWRAP(WrappedID3D11UnorderedAccessView, uav));

				D3D11_MAPPED_SUBRESOURCE mapped;
				hr = m_pImmediateContext->GetReal()->Map(stage, 0, D3D11_MAP_READ, 0, &mapped);

				uint32_t countData = 0;

				if(FAILED(hr))
				{
					RDCERR("Failed to map while creating initial states %08x", hr);
				}
				else
				{
					countData = *((uint32_t *)mapped.pData);

					m_pImmediateContext->GetReal()->Unmap(stage, 0);
				}

				m_ResourceManager->SetInitialContents(id, NULL, countData);

				SAFE_RELEASE(stage);
			}
		}
	}
	else if(type == Resource_Texture1D)
	{
		WrappedID3D11Texture1D *tex1D = (WrappedID3D11Texture1D *)live;

		D3D11_TEXTURE1D_DESC desc;
		tex1D->GetDesc(&desc);

		if(!hasData && desc.MipLevels == 1 && (desc.BindFlags & D3D11_BIND_RENDER_TARGET))
		{
			D3D11_RENDER_TARGET_VIEW_DESC rdesc;
			rdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE1D;
			rdesc.Format = GetTypedFormat(desc.Format);
			rdesc.Texture1D.MipSlice = 0;

			ID3D11RenderTargetView *initContents = NULL;

			HRESULT hr = m_pDevice->CreateRenderTargetView(UNWRAP(WrappedID3D11Texture1D, tex1D), &rdesc, &initContents);

			if(FAILED(hr))
			{
				RDCERR("Failed to create fast-clear RTV while creating initial states %08x", hr);
			}
			else
			{
				m_ResourceManager->SetInitialContents(id, initContents, 1);
			}
		}
		else if(!hasData && desc.MipLevels == 1 && (desc.BindFlags & D3D11_BIND_DEPTH_STENCIL))
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC ddesc;
			ddesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE1D;
			ddesc.Format = GetDepthTypedFormat(desc.Format);
			ddesc.Texture1D.MipSlice = 0;
			ddesc.Flags = 0;

			ID3D11DepthStencilView *initContents = NULL;

			HRESULT hr = m_pDevice->CreateDepthStencilView(UNWRAP(WrappedID3D11Texture1D, tex1D), &ddesc, &initContents);

			if(FAILED(hr))
			{
				RDCERR("Failed to create fast-clear DSV while creating initial states %08x", hr);
			}
			else
			{
				m_ResourceManager->SetInitialContents(id, initContents, 2);
			}
		}
		else if(desc.Usage != D3D11_USAGE_IMMUTABLE)
		{
			desc.CPUAccessFlags = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = 0;
			if(IsDepthFormat(desc.Format))
				desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			desc.MiscFlags &= ~D3D11_RESOURCE_MISC_GENERATE_MIPS;

			ID3D11Texture1D *initContents = NULL;

			HRESULT hr = m_pDevice->CreateTexture1D(&desc, NULL, &initContents);

			if(FAILED(hr))
			{
				RDCERR("Failed to create tex3D while creating initial states %08x", hr);
			}
			else
			{
				m_pImmediateContext->GetReal()->CopyResource(initContents, UNWRAP(WrappedID3D11Texture1D, tex1D));

				m_ResourceManager->SetInitialContents(id, initContents, 0);
			}
		}
	}
	else if(type == Resource_Texture2D)
	{
		WrappedID3D11Texture2D *tex2D = (WrappedID3D11Texture2D *)live;

		D3D11_TEXTURE2D_DESC desc;
		tex2D->GetDesc(&desc);

		bool isMS = (desc.SampleDesc.Count > 1 || desc.SampleDesc.Quality > 0);
		
		if(!hasData && desc.MipLevels == 1 && (desc.BindFlags & D3D11_BIND_RENDER_TARGET))
		{
			D3D11_RENDER_TARGET_VIEW_DESC rdesc;
			rdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			rdesc.Format = GetTypedFormat(desc.Format);
			rdesc.Texture2D.MipSlice = 0;

			if(isMS) rdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;

			ID3D11RenderTargetView *initContents = NULL;

			HRESULT hr = m_pDevice->CreateRenderTargetView(UNWRAP(WrappedID3D11Texture2D, tex2D), &rdesc, &initContents);

			if(FAILED(hr))
			{	
				RDCERR("Failed to create fast-clear RTV while creating initial states %08x", hr);
			}
			else
			{
				m_ResourceManager->SetInitialContents(id, initContents, 1);
			}
		}
		else if(!hasData && desc.MipLevels == 1 && (desc.BindFlags & D3D11_BIND_DEPTH_STENCIL))
		{
			D3D11_DEPTH_STENCIL_VIEW_DESC ddesc;
			ddesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			ddesc.Format = GetDepthTypedFormat(desc.Format);
			ddesc.Texture1D.MipSlice = 0;
			ddesc.Flags = 0;

			if(isMS) ddesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;

			ID3D11DepthStencilView *initContents = NULL;

			HRESULT hr = m_pDevice->CreateDepthStencilView(UNWRAP(WrappedID3D11Texture2D, tex2D), &ddesc, &initContents);

			if(FAILED(hr))
			{
				RDCERR("Failed to create fast-clear DSV while creating initial states %08x", hr);
			}
			else
			{
				m_ResourceManager->SetInitialContents(id, initContents, 2);
			}
		}
		else if(desc.Usage != D3D11_USAGE_IMMUTABLE)
		{
			desc.CPUAccessFlags = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = isMS ? D3D11_BIND_SHADER_RESOURCE : 0;
			if(IsDepthFormat(desc.Format))
				desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
			desc.MiscFlags &= ~D3D11_RESOURCE_MISC_GENERATE_MIPS;

			ID3D11Texture2D *initContents = NULL;

			HRESULT hr = m_pDevice->CreateTexture2D(&desc, NULL, &initContents);

			if(FAILED(hr))
			{	
				RDCERR("Failed to create tex2D while creating initial states %08x", hr);
			}
			else
			{
				m_pImmediateContext->GetReal()->CopyResource(initContents, UNWRAP(WrappedID3D11Texture2D, tex2D));

				m_ResourceManager->SetInitialContents(id, initContents, 0);
			}
		}
	}
	else if(type == Resource_Texture3D)
	{
		WrappedID3D11Texture3D *tex3D = (WrappedID3D11Texture3D *)live;

		D3D11_TEXTURE3D_DESC desc;
		tex3D->GetDesc(&desc);
			
		if(!hasData && desc.MipLevels == 1 && (desc.BindFlags & D3D11_BIND_RENDER_TARGET))
		{
			D3D11_RENDER_TARGET_VIEW_DESC rdesc;
			rdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE3D;
			rdesc.Format = GetTypedFormat(desc.Format);
			rdesc.Texture3D.FirstWSlice = 0;
			rdesc.Texture3D.MipSlice = 0;
			rdesc.Texture3D.WSize = desc.Depth;

			ID3D11RenderTargetView *initContents = NULL;

			HRESULT hr = m_pDevice->CreateRenderTargetView(UNWRAP(WrappedID3D11Texture3D, tex3D), &rdesc, &initContents);

			if(FAILED(hr))
			{
				RDCERR("Failed to create fast-clear RTV while creating initial states %08x", hr);
			}
			else
			{
				m_ResourceManager->SetInitialContents(id, initContents, 1);
			}
		}
		else if(!hasData && desc.Usage != D3D11_USAGE_IMMUTABLE)
		{
			desc.CPUAccessFlags = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			desc.BindFlags = 0;
			desc.MiscFlags &= ~D3D11_RESOURCE_MISC_GENERATE_MIPS;

			ID3D11Texture3D *initContents = NULL;

			HRESULT hr = m_pDevice->CreateTexture3D(&desc, NULL, &initContents);

			if(FAILED(hr))
			{
				RDCERR("Failed to create tex3D while creating initial states %08x", hr);
			}
			else
			{
				m_pImmediateContext->GetReal()->CopyResource(initContents, UNWRAP(WrappedID3D11Texture3D, tex3D));

				m_ResourceManager->SetInitialContents(id, initContents, 0);
			}
		}
	}
}

void WrappedID3D11Device::Apply_InitialState(ID3D11DeviceChild *live, ID3D11DeviceChild *initial, uint32_t count)
{
	ResourceType type = IdentifyTypeByPtr(live);

	if(type == Resource_UnorderedAccessView)
	{
		ID3D11UnorderedAccessView *uav = (ID3D11UnorderedAccessView *)live;

		m_pImmediateContext->CSSetUnorderedAccessViews(0, 1, &uav, &count);
	}
	else
	{
		if(count == 1)
		{
			float emptyCol[] = { 0.0f, 0.0f, 0.0f, 0.0f };
			m_pImmediateContext->GetReal()->ClearRenderTargetView((ID3D11RenderTargetView *)initial, emptyCol);
		}
		else if(count == 2)
		{
			m_pImmediateContext->GetReal()->ClearDepthStencilView((ID3D11DepthStencilView *)initial, D3D11_CLEAR_DEPTH|D3D11_CLEAR_STENCIL, 1.0f, 0);
		}
		else
		{
			ID3D11Resource *liveResource = (ID3D11Resource *)m_ResourceManager->UnwrapResource(live);
			ID3D11Resource *initialResource = (ID3D11Resource *)initial;

			m_pImmediateContext->GetReal()->CopyResource(liveResource, initialResource);
		}
	}
}

void WrappedID3D11Device::SetContextFilter(ResourceId id, uint32_t firstDefEv, uint32_t lastDefEv)
{
	m_ReplayDefCtx = id;
	m_FirstDefEv = firstDefEv;
	m_LastDefEv = lastDefEv;
}

void WrappedID3D11Device::ReplayLog(uint32_t frameID, uint32_t startEventID, uint32_t endEventID, ReplayLogType replayType)
{
	RDCASSERT(frameID < (uint32_t)m_FrameRecord.size());

	uint64_t offs = m_FrameRecord[frameID].frameInfo.fileOffset;

	m_pSerialiser->SetOffset(offs);

	bool partial = true;

	if(startEventID == 0 && (replayType == eReplay_WithoutDraw || replayType == eReplay_Full))
	{
		startEventID = m_FrameRecord[frameID].frameInfo.firstEvent;
		partial = false;
	}
	
	D3D11ChunkType header = (D3D11ChunkType)m_pSerialiser->PushContext(NULL, 1, false);

	RDCASSERT(header == CAPTURE_SCOPE);

	m_pSerialiser->SkipCurrentChunk();

	m_pSerialiser->PopContext(NULL, header);
	
	if(!partial)
	{
		GetResourceManager()->ApplyInitialContents();
		GetResourceManager()->ReleaseInFrameResources();
	}
	
	if(m_ReplayDefCtx == ResourceId())
	{
		if(replayType == eReplay_Full)
			m_pImmediateContext->ReplayLog(EXECUTING, startEventID, endEventID, partial);
		else if(replayType == eReplay_WithoutDraw)
			m_pImmediateContext->ReplayLog(EXECUTING, startEventID, RDCMAX(1U,endEventID)-1, partial);
		else if(replayType == eReplay_OnlyDraw)
			m_pImmediateContext->ReplayLog(EXECUTING, endEventID, endEventID, partial);
		else
			RDCFATAL("Unexpected replay type");
	}
	else
	{
		if(replayType == eReplay_Full || replayType == eReplay_WithoutDraw)
		{
			m_pImmediateContext->ReplayLog(EXECUTING, startEventID, endEventID, partial);
		}

		m_pSerialiser->SetOffset(offs);
		
		D3D11ChunkType header = (D3D11ChunkType)m_pSerialiser->PushContext(NULL, 1, false);
		m_pSerialiser->SkipCurrentChunk();
		m_pSerialiser->PopContext(NULL, header);

		m_pImmediateContext->ReplayFakeContext(m_ReplayDefCtx);
		
		if(replayType == eReplay_Full)
		{
			m_pImmediateContext->ClearState();

			m_pImmediateContext->ReplayLog(EXECUTING, m_FirstDefEv, m_LastDefEv, true);
		}
		else if(replayType == eReplay_WithoutDraw && m_LastDefEv-1 >= m_FirstDefEv)
		{
			m_pImmediateContext->ClearState();

			m_pImmediateContext->ReplayLog(EXECUTING, m_FirstDefEv, RDCMAX(m_LastDefEv,1U)-1, true);
		}
		else if(replayType == eReplay_OnlyDraw)
		{
			m_pImmediateContext->ReplayLog(EXECUTING, m_LastDefEv, m_LastDefEv, true);
		}

		m_pImmediateContext->ReplayFakeContext(ResourceId());
	}
}

void WrappedID3D11Device::ReleaseSwapchainResources(IDXGISwapChain *swap)
{
	if(swap)
	{
		DXGI_SWAP_CHAIN_DESC desc;
		swap->GetDesc(&desc);

		Keyboard::RemoveInputWindow(desc.OutputWindow);
	}

	auto it = m_SwapChains.find(swap);
	if(it != m_SwapChains.end())
	{
		SAFE_RELEASE(it->second);
		m_SwapChains.erase(it);
	}

	if(swap == m_SwapChain)
	{
		if(m_SwapChains.empty())
			m_SwapChain = NULL;
		else
			m_SwapChain = m_SwapChains.begin()->first;
	}
}

bool WrappedID3D11Device::Serialise_SetSwapChainTexture(IDXGISwapChain *swap, DXGI_SWAP_CHAIN_DESC *swapDesc, UINT buffer, ID3D11Texture2D *pTex)
{
	SERIALISE_ELEMENT(DXGI_FORMAT, swapFormat, swapDesc->BufferDesc.Format);
	SERIALISE_ELEMENT(uint32_t, BuffNum, buffer);
	SERIALISE_ELEMENT(ResourceId, pTexture, GetIDForResource(pTex));

	if(m_State >= WRITING)
	{
		D3D11_TEXTURE2D_DESC desc;

		pTex->GetDesc(&desc);

		SERIALISE_ELEMENT(D3D11_TEXTURE2D_DESC, Descriptor, desc);
	}
	else
	{
		ID3D11Texture2D *fakeBB;

		SERIALISE_ELEMENT(D3D11_TEXTURE2D_DESC, Descriptor, D3D11_TEXTURE2D_DESC());

		D3D11_TEXTURE2D_DESC realDescriptor = Descriptor;

		// DXGI swap chain back buffers can be freely cast as a special-case.
		// translate the format to a typeless format to allow for this.
		// the original type will be stored in the texture below
		Descriptor.Format = GetTypelessFormat(Descriptor.Format);

		HRESULT hr = m_pDevice->CreateTexture2D(&Descriptor, NULL, &fakeBB);

		if(FAILED(hr))
		{
			RDCERR("Failed to create fake back buffer, HRESULT: 0x%08x", hr);
		}
		else
		{
			WrappedID3D11Texture2D *wrapped = new WrappedID3D11Texture2D(fakeBB, this, TEXDISPLAY_INDIRECT_VIEW);
			fakeBB = wrapped;

			wrapped->m_RealDescriptor = new D3D11_TEXTURE2D_DESC(realDescriptor);

			SetDebugName(fakeBB, "Serialised Swap Chain Buffer");

			GetResourceManager()->AddLiveResource(pTexture, fakeBB);
		}
	}

	return true;
}

void WrappedID3D11Device::SetSwapChainTexture(IDXGISwapChain *swap, DXGI_SWAP_CHAIN_DESC *swapDesc, UINT buffer, ID3D11Texture2D *pTex)
{
	D3D11_TEXTURE2D_DESC desc;
	pTex->GetDesc(&desc);
	
	ResourceId id = GetIDForResource(pTex);

	LazyInit();
	
	// there shouldn't be a resource record for this texture as it wasn't created via
	// CreateTexture2D
	RDCASSERT(id != ResourceId() && !GetResourceManager()->HasResourceRecord(id));

	if(m_State >= WRITING)
	{
		D3D11ResourceRecord *record = GetResourceManager()->AddResourceRecord(id);
		record->DataInSerialiser = false;
		record->SpecialResource = true;
		record->Length = 0;
		record->NumSubResources = 0;
		record->SubResources = NULL;

		SCOPED_LOCK(m_D3DLock);

		SCOPED_SERIALISE_CONTEXT(CREATE_SWAP_BUFFER);

		Serialise_SetSwapChainTexture(swap, swapDesc, buffer, pTex);

		record->AddChunk(scope.Get());
	}
	
	if(buffer == 0 && m_State >= WRITING)
	{
		ID3D11RenderTargetView *rtv = NULL;
		HRESULT hr = m_pDevice->CreateRenderTargetView(UNWRAP(WrappedID3D11Texture2D, pTex), NULL, &rtv);

		if(FAILED(hr))
			RDCERR("Couldn't create RTV for swapchain tex %08x", hr);

		m_SwapChains[swap] = rtv;
	}
	
	if(swap)
	{
		DXGI_SWAP_CHAIN_DESC desc;
		swap->GetDesc(&desc);

		Keyboard::AddInputWindow(desc.OutputWindow);
	}

	if(m_SwapChain == NULL)
	{
		m_SwapChain = swap;

		GetDebugManager()->SetOutputDimensions(desc.Width, desc.Height);
	}
}

void WrappedID3D11Device::SetMarker(uint32_t col, const wchar_t *name)
{
	if(m_pCurrentWrappedDevice == NULL)
		return;

	m_pCurrentWrappedDevice->m_pImmediateContext->ThreadSafe_SetMarker(col, name);
}

int WrappedID3D11Device::BeginEvent(uint32_t col, const wchar_t *name)
{
	if(m_pCurrentWrappedDevice == NULL)
		return 0;

	return m_pCurrentWrappedDevice->m_pImmediateContext->ThreadSafe_BeginEvent(col, name);
}

int WrappedID3D11Device::EndEvent()
{
	if(m_pCurrentWrappedDevice == NULL)
		return 0;

	return m_pCurrentWrappedDevice->m_pImmediateContext->ThreadSafe_EndEvent();
}

HRESULT WrappedID3D11Device::Present(IDXGISwapChain *swap, UINT SyncInterval, UINT Flags)
{
	if((Flags & DXGI_PRESENT_TEST) != 0)
		return S_OK;
	
	RenderDoc::Inst().SetCurrentDriver(RDC_D3D11);
	m_pCurrentWrappedDevice = this;
	
	if(m_State == WRITING_IDLE)
		RenderDoc::Inst().Tick();
	
	m_pImmediateContext->EndFrame();

	m_FrameCounter++; // first present becomes frame #1, this function is at the end of the frame

	m_pImmediateContext->BeginFrame();

	if(m_State == WRITING_IDLE)
	{
		D3D11RenderState old = *m_pImmediateContext->GetCurrentPipelineState();

		m_FrameTimes.push_back(m_FrameTimer.GetMilliseconds());
		m_TotalTime += m_FrameTimes.back();
		m_FrameTimer.Restart();

		// update every second
		if(m_TotalTime > 1000.0)
		{
			m_MinFrametime = 10000.0;
			m_MaxFrametime = 0.0;
			m_AvgFrametime = 0.0;

			m_TotalTime = 0.0;

			for(size_t i=0; i < m_FrameTimes.size(); i++)
			{
				m_AvgFrametime += m_FrameTimes[i];
				if(m_FrameTimes[i] < m_MinFrametime)
					m_MinFrametime = m_FrameTimes[i];
				if(m_FrameTimes[i] > m_MaxFrametime)
					m_MaxFrametime = m_FrameTimes[i];
			}

			m_AvgFrametime /= double(m_FrameTimes.size());

			m_FrameTimes.clear();
		}

		ID3D11RenderTargetView *rtv = m_SwapChains[swap];

		m_pImmediateContext->GetReal()->OMSetRenderTargets(1, &rtv, NULL);

		int w = GetDebugManager()->GetWidth();
		int h = GetDebugManager()->GetHeight();

		DXGI_SWAP_CHAIN_DESC swapDesc = {0};
		swap->GetDesc(&swapDesc);
		GetDebugManager()->SetOutputDimensions(swapDesc.BufferDesc.Width, swapDesc.BufferDesc.Height);

		if(swap == m_SwapChain)
		{
			GetDebugManager()->RenderText(0.0f, 0.0f, 1.0f, "Frame: %d. F12/PrtScr to capture. %.2lf ms (%.2lf .. %.2lf) (%.0lf FPS)",
				m_FrameCounter, m_AvgFrametime, m_MinFrametime, m_MaxFrametime, 1000.0f/m_AvgFrametime);
			size_t i=0;
			for(i=0; i < m_FrameRecord.size(); i++)
				GetDebugManager()->RenderText(0.0f, (float)(i+1)*18.0f, 1.0f, "Captured frame %d.\n", m_FrameRecord[i].frameInfo.frameNumber);

			if(m_FailedFrame > 0)
			{
				const char *reasonString = "Unknown reason";
				switch(m_FailedReason)
				{
					case CaptureFailed_UncappedCmdlist: reasonString = "Uncapped command list"; break;
					case CaptureFailed_UncappedUnmap: reasonString = "Uncapped Map()/Unmap()"; break;
					default: break;
				}

				GetDebugManager()->RenderText(0.0f, (float)(++i)*18.0f, 1.0f, "Failed capture at frame %d:\n", m_FailedFrame);
				GetDebugManager()->RenderText(0.0f, (float)(++i)*18.0f, 1.0f, "    %hs\n", reasonString);
			}

#if !defined(RELEASE)
			GetDebugManager()->RenderText(0.0f, float(++i)*18.0f, 1.0f, "%llu chunks - %.2f MB", Chunk::NumLiveChunks(), float(Chunk::TotalMem())/1024.0f/1024.0f);
#endif
		}
		else
		{
			GetDebugManager()->RenderText(0.0f, 0.0f, 1.0f, "Inactive swapchain, F11 to cycle");
		}
		
		GetDebugManager()->SetOutputDimensions(w, h);

		old.ApplyState(m_pImmediateContext);

		if(RenderDoc::Inst().ShouldFocusToggle())
		{
			IDXGISwapChain *s = m_SwapChain;
			m_SwapChain = NULL;

			for(auto it=m_SwapChains.begin(); it!=m_SwapChains.end(); ++it)
			{
				auto next = it; next++;
				if(it->first == s)
				{
					if(next != m_SwapChains.end())
						m_SwapChain = next->first;
					else
						m_SwapChain = m_SwapChains.begin()->first;
					break;
				}
			}

			if(m_SwapChain == NULL)
				m_SwapChain = swap;

			DXGI_SWAP_CHAIN_DESC swapDesc = {0};
			m_SwapChain->GetDesc(&swapDesc);
			GetDebugManager()->SetOutputDimensions(swapDesc.BufferDesc.Width, swapDesc.BufferDesc.Height);
		}
	}

	if(swap != m_SwapChain)
	{
		return S_OK;
	}
	
	// kill any current capture
	if(m_State == WRITING_CAPFRAME)
	{
		CaptureFailReason reason;

		if(m_pImmediateContext->HasSuccessfulCapture(reason))
		{
			SCOPED_LOCK(m_D3DLock);

			RDCLOG("Finished capture, Frame %u", m_FrameCounter);

			m_Failures = 0;
			m_FailedFrame = 0;
			m_FailedReason = CaptureSucceeded;

			m_pImmediateContext->EndCaptureFrame();
			m_pImmediateContext->FinishCapture();

			for(auto it = m_DeferredContexts.begin(); it != m_DeferredContexts.end(); ++it)
			{
				WrappedID3D11DeviceContext *context = *it;

				if(context)
				{
					context->FinishCapture();
				}
				else
				{
					RDCERR("NULL deferred context in resource record!");
				}
			}

			const uint32_t maxSize = 1024;

			byte *thpixels = NULL;
			uint32_t thwidth = 0;
			uint32_t thheight = 0;

			{
				ID3D11RenderTargetView *rtv = m_SwapChains[swap];

				ID3D11Resource *res = NULL;

				rtv->GetResource(&res); res->Release();

				ID3D11Texture2D *tex = (ID3D11Texture2D *)res;

				D3D11_TEXTURE2D_DESC desc;
				tex->GetDesc(&desc);

				desc.BindFlags = 0;
				desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
				desc.MiscFlags = 0;
				desc.Usage = D3D11_USAGE_STAGING;

				bool msaa = (desc.SampleDesc.Count > 1) || (desc.SampleDesc.Quality > 0);

				desc.SampleDesc.Count = 1;
				desc.SampleDesc.Quality = 0;

				ID3D11Texture2D *stagingTex = NULL;

				HRESULT hr = m_pDevice->CreateTexture2D(&desc, NULL, &stagingTex);

				if(FAILED(hr))
				{
					RDCERR("Couldn't create staging texture to create thumbnail. %08x", hr);
				}
				else
				{
					if(msaa)
					{
						desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
						desc.CPUAccessFlags = 0;
						desc.Usage = D3D11_USAGE_DEFAULT;
						
						ID3D11Texture2D *resolveTex = NULL;

						HRESULT hr = m_pDevice->CreateTexture2D(&desc, NULL, &resolveTex);

						if(FAILED(hr))
						{
							RDCERR("Couldn't create resolve texture to create thumbnail. %08x", hr);
							tex = NULL;
						}
						else
						{
							m_pImmediateContext->GetReal()->ResolveSubresource(resolveTex, 0, tex, 0, desc.Format);
							m_pImmediateContext->GetReal()->CopyResource(stagingTex, resolveTex);
							resolveTex->Release();
						}
					}
					else
					{
						m_pImmediateContext->GetReal()->CopyResource(stagingTex, tex);
					}

					if(tex)
					{
						ResourceFormat fmt = MakeResourceFormat(desc.Format);

						D3D11_MAPPED_SUBRESOURCE mapped;
						hr = m_pImmediateContext->GetReal()->Map(stagingTex, 0, D3D11_MAP_READ, 0, &mapped);

						if(FAILED(hr))
						{
							RDCERR("Couldn't map staging texture to create thumbnail. %08x", hr);
						}
						else
						{
							byte *data = (byte *)mapped.pData;

							float aspect = float(desc.Width)/float(desc.Height);

							thwidth = RDCMIN(maxSize, desc.Width);
							thwidth &= ~0x7; // align down to multiple of 8
							thheight = uint32_t(float(thwidth)/aspect);

							thpixels = new byte[3*thwidth*thheight];

							float widthf = float(desc.Width);
							float heightf = float(desc.Height);

							uint32_t stride = fmt.compByteWidth*fmt.compCount;

							bool buf1010102 = false;
							bool bufBGRA = false;

							if(fmt.special && fmt.specialFormat == eSpecial_R10G10B10A2)
							{
								stride = 4;
								buf1010102 = true;
							}
							if(fmt.special && fmt.specialFormat == eSpecial_B8G8R8A8)
							{
								stride = 4;
								bufBGRA = true;
							}

							byte *dst = thpixels;

							for(uint32_t y=0; y < thheight; y++)
							{
								for(uint32_t x=0; x < thwidth; x++)
								{
									float xf = float(x)/float(thwidth);
									float yf = float(y)/float(thheight);

									byte *src = &data[ stride*uint32_t(xf*widthf) + mapped.RowPitch*uint32_t(yf*heightf) ];

									if(buf1010102)
									{
										uint32_t *src1010102 = (uint32_t *)src;
										Vec4f unorm = ConvertFromR10G10B10A2(*src1010102);
										dst[0] = (byte)(unorm.x*255.0f);
										dst[1] = (byte)(unorm.y*255.0f);
										dst[2] = (byte)(unorm.z*255.0f);
									}
									else if(bufBGRA)
									{
										dst[0] = src[2];
										dst[1] = src[1];
										dst[2] = src[0];
									}
									else
									{
										dst[0] = src[0];
										dst[1] = src[1];
										dst[2] = src[2];
									}

									dst += 3;
								}
							}

							m_pImmediateContext->GetReal()->Unmap(stagingTex, 0);
						}
					}

					stagingTex->Release();
				}

			}

			byte *jpgbuf = NULL;
			int len = thwidth*thheight;

			{
				jpgbuf = new byte[len];

				jpge::params p;

				p.m_quality = 40;

				bool success = jpge::compress_image_to_jpeg_file_in_memory(jpgbuf, len, thwidth, thheight, 3, thpixels, p);

				if(!success)
				{
					RDCERR("Failed to compress to jpg");
					SAFE_DELETE_ARRAY(jpgbuf);
					thwidth = 0;
					thheight = 0;
				}
			}

			Serialiser *m_pFileSerialiser = RenderDoc::Inst().OpenWriteSerialiser(m_FrameCounter, m_InitParams, jpgbuf, len, thwidth, thheight);
			
			SAFE_DELETE_ARRAY(jpgbuf);
			SAFE_DELETE(thpixels);

			{
				SCOPED_SERIALISE_CONTEXT(DEVICE_INIT);

				SERIALISE_ELEMENT(ResourceId, immContextId, m_pImmediateContext->GetResourceID());

				m_pFileSerialiser->Insert(scope.Get(true));
			}

			RDCDEBUG("Inserting Resource Serialisers");	

			GetResourceManager()->InsertReferencedChunks(m_pFileSerialiser);
			
			GetResourceManager()->InsertInitialContentsChunks(m_pSerialiser, m_pFileSerialiser);

			RDCDEBUG("Creating Capture Scope");	

			{
				SCOPED_SERIALISE_CONTEXT(CAPTURE_SCOPE);

				Serialise_CaptureScope(0);

				m_pFileSerialiser->Insert(scope.Get(true));
			}

			{
				RDCDEBUG("Getting Resource Record");	

				D3D11ResourceRecord *record = m_ResourceManager->GetResourceRecord(m_pImmediateContext->GetResourceID());

				RDCDEBUG("Accumulating context resource list");	

				map<int32_t, Chunk *> recordlist;
				record->Insert(recordlist);

				RDCDEBUG("Flushing %u records to file serialiser", (uint32_t)recordlist.size());	

				for(auto it = recordlist.begin(); it != recordlist.end(); ++it)
					m_pFileSerialiser->Insert(it->second);

				RDCDEBUG("Done");	
			}

			m_CurFileSize += m_pFileSerialiser->FlushToDisk();

			SAFE_DELETE(m_pFileSerialiser);

			RenderDoc::Inst().SuccessfullyWrittenLog();

			m_State = WRITING_IDLE;

			m_pImmediateContext->CleanupCapture();

			m_pImmediateContext->FreeCaptureData();

			for(auto it = m_DeferredContexts.begin(); it != m_DeferredContexts.end(); ++it)
			{
				WrappedID3D11DeviceContext *context = *it;

				if(context)
					context->CleanupCapture();
				else
					RDCERR("NULL deferred context in resource record!");
			}

			GetResourceManager()->MarkUnwrittenResources();

			GetResourceManager()->ClearReferencedResources();
		}
		else
		{
			RDCLOG("Failed to capture, frame %u", m_FrameCounter);

			m_Failures++;
			
			{
				D3D11RenderState old = *m_pImmediateContext->GetCurrentPipelineState();

				ID3D11RenderTargetView *rtv = m_SwapChains[swap];

				if(rtv)
				{
					m_pImmediateContext->GetReal()->OMSetRenderTargets(1, &rtv, NULL);

					int w = GetDebugManager()->GetWidth();
					int h = GetDebugManager()->GetHeight();

					DXGI_SWAP_CHAIN_DESC swapDesc = {0};
					swap->GetDesc(&swapDesc);
					GetDebugManager()->SetOutputDimensions(swapDesc.BufferDesc.Width, swapDesc.BufferDesc.Height);

					const char *reasonString = "Unknown reason";
					switch(reason)
					{
						case CaptureFailed_UncappedCmdlist: reasonString = "Uncapped command list"; break;
						case CaptureFailed_UncappedUnmap: reasonString = "Uncapped Map()/Unmap()"; break;
						default: break;
					}

					GetDebugManager()->RenderText(0.0f, 0.0f, 1.0f, "Failed to capture frame %u: %hs", m_FrameCounter, reasonString);

					GetDebugManager()->SetOutputDimensions(w, h);
				}

				old.ApplyState(m_pImmediateContext);
			}

			m_FrameRecord.back().frameInfo.frameNumber = m_FrameCounter+1;
			
			m_pImmediateContext->CleanupCapture();

			for(auto it = m_DeferredContexts.begin(); it != m_DeferredContexts.end(); ++it)
			{
				WrappedID3D11DeviceContext *context = *it;

				if(context)
					context->CleanupCapture();
				else
					RDCERR("NULL deferred context in resource record!");
			}

			GetResourceManager()->ClearReferencedResources();
			
			if(m_Failures > 5) // failed too many times
			{
				m_pImmediateContext->FinishCapture();

				m_FrameRecord.pop_back();

				for(auto it = m_DeferredContexts.begin(); it != m_DeferredContexts.end(); ++it)
				{
					WrappedID3D11DeviceContext *context = *it;

					if(context)
					{
						context->FinishCapture();
					}
					else
					{
						RDCERR("NULL deferred context in resource record!");
					}
				}

				m_pImmediateContext->FreeCaptureData();

				m_FailedFrame = m_FrameCounter;
				m_FailedReason = reason;

				m_State = WRITING_IDLE;

				for(auto it = m_DeferredContexts.begin(); it != m_DeferredContexts.end(); ++it)
				{
					WrappedID3D11DeviceContext *context = *it;

					if(context)
						context->CleanupCapture();
					else
						RDCERR("NULL deferred context in resource record!");
				}

				GetResourceManager()->MarkUnwrittenResources();
			}
			else
			{			
				GetResourceManager()->MarkResourceFrameReferenced(m_ResourceID, eFrameRef_Write);
				GetResourceManager()->PrepareInitialContents();

				m_pImmediateContext->AttemptCapture();
				m_pImmediateContext->BeginCaptureFrame();

				for(auto it = m_DeferredContexts.begin(); it != m_DeferredContexts.end(); ++it)
				{
					WrappedID3D11DeviceContext *context = *it;

					if(context)
					{
						context->AttemptCapture();
					}
					else
					{
						RDCERR("NULL deferred context in resource record!");
					}
				}
			}

			if(m_pInfoQueue)
				m_pInfoQueue->ClearStoredMessages();
		}
	}

	if(RenderDoc::Inst().ShouldTriggerCapture(m_FrameCounter) && m_State == WRITING_IDLE)
	{
		SCOPED_LOCK(m_D3DLock);

		m_State = WRITING_CAPFRAME;

		m_Failures = 0;
		m_FailedFrame = 0;
		m_FailedReason = CaptureSucceeded;

		FetchFrameRecord record;
		record.frameInfo.frameNumber = m_FrameCounter+1;
		m_FrameRecord.push_back(record);

		GetResourceManager()->ClearReferencedResources();

		GetResourceManager()->MarkResourceFrameReferenced(m_ResourceID, eFrameRef_Write);

		m_pImmediateContext->AttemptCapture();
		m_pImmediateContext->BeginCaptureFrame();

		for(auto it = m_DeferredContexts.begin(); it != m_DeferredContexts.end(); ++it)
		{
			WrappedID3D11DeviceContext *context = *it;

			if(context)
			{
				context->AttemptCapture();
			}
			else
			{
				RDCERR("NULL deferred context in resource record!");
			}
		}
		
		GetResourceManager()->PrepareInitialContents();

		if(m_pInfoQueue)
			m_pInfoQueue->ClearStoredMessages();

		RDCLOG("Starting capture, frame %u", m_FrameCounter);
	}

	return S_OK;
}

void WrappedID3D11Device::AddDeferredContext(WrappedID3D11DeviceContext *defctx)
{
	RDCASSERT(m_DeferredContexts.find(defctx) == m_DeferredContexts.end());
	m_DeferredContexts.insert(defctx);
}

void WrappedID3D11Device::RemoveDeferredContext(WrappedID3D11DeviceContext *defctx)
{
	RDCASSERT(m_DeferredContexts.find(defctx) != m_DeferredContexts.end());
	m_DeferredContexts.erase(defctx);
}

bool WrappedID3D11Device::Serialise_SetResourceName(ID3D11DeviceChild *res, const char *nm)
{
	SERIALISE_ELEMENT(ResourceId, resource, GetIDForResource(res));
	string name = nm ? nm : "";
	m_pSerialiser->Serialise("name", name);

	if(m_State < WRITING && GetResourceManager()->HasLiveResource(resource))
	{
		ID3D11DeviceChild *r = GetResourceManager()->GetLiveResource(resource);

		SetDebugName(r, name.c_str());
	}

	return true;
}

void WrappedID3D11Device::SetResourceName(ID3D11DeviceChild *res, const char *name)
{
	if(m_State >= WRITING)
	{
		ResourceId idx = GetIDForResource(res);
		D3D11ResourceRecord *record = GetResourceManager()->GetResourceRecord(idx);

		if(record == NULL)
			record = m_DeviceRecord;

		RDCASSERT(idx != ResourceId());
		
		SCOPED_LOCK(m_D3DLock);
		{
			SCOPED_SERIALISE_CONTEXT(SET_RESOURCE_NAME);

			Serialise_SetResourceName(res, name);

			record->AddChunk(scope.Get());
		}
	}
}

bool WrappedID3D11Device::Serialise_ReleaseResource(ID3D11DeviceChild *res)
{
	ResourceType resourceType = Resource_Unknown;
	ResourceId resource = GetIDForResource(res);
	
	if(m_State >= WRITING)
	{
		resourceType = IdentifyTypeByPtr(res);
	}

	if(m_State == WRITING_IDLE || m_State < WRITING)
	{
		SERIALISE_ELEMENT(ResourceId, serRes, GetIDForResource(res));
		SERIALISE_ELEMENT(ResourceType, serType, resourceType);

		resourceType = serType;
		resource = serRes;
	}
	
	if(m_State >= WRITING)
	{
		D3D11ResourceRecord *record = GetResourceManager()->GetResourceRecord(resource);
		if(record)
			record->Delete(m_ResourceManager);

		switch(resourceType)
		{
			case Resource_RenderTargetView:
			{
				WrappedID3D11RenderTargetView* view = (WrappedID3D11RenderTargetView *)res;

				D3D11ResourceRecord *viewRecord = view->GetResourceRecord();
				if(viewRecord)
					viewRecord->Delete(m_ResourceManager);
				break;
			}
			case Resource_ShaderResourceView:
			{
				WrappedID3D11ShaderResourceView* view = (WrappedID3D11ShaderResourceView *)res;

				D3D11ResourceRecord *viewRecord = view->GetResourceRecord();
				if(viewRecord)
					viewRecord->Delete(m_ResourceManager);
				break;
			}
			case Resource_DepthStencilView:
			{
				WrappedID3D11DepthStencilView* view = (WrappedID3D11DepthStencilView *)res;

				D3D11ResourceRecord *viewRecord = view->GetResourceRecord();
				if(viewRecord)
					viewRecord->Delete(m_ResourceManager);
				break;
			}
			case Resource_UnorderedAccessView:
			{
				WrappedID3D11UnorderedAccessView* view = (WrappedID3D11UnorderedAccessView *)res;

				D3D11ResourceRecord *viewRecord = view->GetResourceRecord();
				if(viewRecord)
					viewRecord->Delete(m_ResourceManager);
				break;
			}
		}
	}
	if(m_State < WRITING && GetResourceManager()->HasLiveResource(resource))
	{
		ID3D11DeviceChild *res = GetResourceManager()->GetLiveResource(resource);
		GetResourceManager()->EraseLiveResource(resource);
		SAFE_RELEASE(res);
	}

	return true;
}

void WrappedID3D11Device::ReleaseResource(ID3D11DeviceChild *res)
{
	ResourceId idx = GetIDForResource(res);

	// wrapped resources get released all the time, we don't want to
	// try and slerp in a resource release. Just the explicit ones
	if(m_State < WRITING)
	{
		if(GetResourceManager()->HasLiveResource(idx))
			GetResourceManager()->EraseLiveResource(idx);
		return;
	}
	
	SCOPED_LOCK(m_D3DLock);

	ResourceType type = IdentifyTypeByPtr(res);

	D3D11ResourceRecord *record = m_DeviceRecord;

	bool removegpu = true;

	if(m_State == WRITING_IDLE)
	{
		if(type == Resource_ShaderResourceView ||
			type == Resource_DepthStencilView ||
			type == Resource_UnorderedAccessView ||
			type == Resource_RenderTargetView ||
			type == Resource_Buffer ||
			type == Resource_Texture1D ||
			type == Resource_Texture2D ||
			type == Resource_Texture3D ||
			type == Resource_CommandList)
		{
			if(type == Resource_ShaderResourceView ||
				type == Resource_DepthStencilView ||
				type == Resource_UnorderedAccessView ||
				type == Resource_RenderTargetView
				)
			{
				ID3D11View *view = (ID3D11View *)res;
				ID3D11Resource *viewRes = NULL;
				view->GetResource(&viewRes);
				idx = GetIDForResource(viewRes);
				SAFE_RELEASE(viewRes);

				removegpu = false;
			}

			record = GetResourceManager()->GetResourceRecord(idx);
			RDCASSERT(record);

			if(record->SpecialResource)
			{
				record = m_DeviceRecord;
			}		
			else if(record->GetRefCount() == 1)
			{
				// we're about to decrement this chunk out of existance!
				// don't hold onto the record to add the chunk.
				record = NULL;
			}
		}
	}

	if(removegpu)
		GetResourceManager()->MarkCleanResource(idx);

	if(type == Resource_DeviceContext)
	{
		RemoveDeferredContext((WrappedID3D11DeviceContext *)res);
	}

	bool serialiseRelease = true;

	WrappedID3D11CommandList *cmdList = (WrappedID3D11CommandList *)res;
	
	// don't serialise releases of counters or queries since we ignore them.
	// Also don't serialise releases of command lists that weren't captured,
	// since their creation won't be in the log either.
	if(type == Resource_Counter || type == Resource_Query ||
	   (type == Resource_CommandList && !cmdList->IsCaptured())
	  )
		serialiseRelease = false;

	if(type == Resource_CommandList && !cmdList->IsCaptured())
	{
		record = GetResourceManager()->GetResourceRecord(idx);
		if(record)
			record->Delete(GetResourceManager());
	}
	
	if(serialiseRelease)
	{
		if(m_State == WRITING_CAPFRAME)
		{
			Serialise_ReleaseResource(res);
		}
		else
		{
			SCOPED_SERIALISE_CONTEXT(RELEASE_RESOURCE);
			Serialise_ReleaseResource(res);

			if(record)
			{
				record->AddChunk(scope.Get());
			}
		}

		if(record == NULL)
		{
			// if record is NULL then we just deleted a reference-less resource.
			// That means it is not used and can be safely discarded, so just
			// throw away the serialiser contents
			m_pSerialiser->Rewind();
		}
	}
}

WrappedID3D11DeviceContext *WrappedID3D11Device::GetDeferredContext( size_t idx )
{
	auto it = m_DeferredContexts.begin();

	for(size_t i=0; i < idx; i++)
	{
		++it;
		if(it == m_DeferredContexts.end())
			return NULL;
	}

	return *it;
}
