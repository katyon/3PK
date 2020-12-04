#include "framework.h"
#include "input.h"


#pragma comment( lib, "d3d11.lib" )
#pragma comment( lib, "dxgi.lib" )

#ifdef _DEBUG
	#pragma comment(lib, "./DirectXTK-master/Debug/DirectXTK.lib")
	#pragma comment(lib, "./FBXSDK/debug/libfbxsdk-md.lib")
#else
	#pragma comment(lib, "./DirectXTK-master/Release/DirectXTK.lib")
	#pragma comment(lib, "./FBXSDK/release/libfbxsdk-md.lib")
#endif // _DEBUG



bool framework::initialize()
{
	HRESULT hr = S_OK;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	UINT numFeatureLevels = ARRAYSIZE(featureLevels);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.BufferCount = 1;
	sd.BufferDesc.Width = SCREEN_WIDTH;
	sd.BufferDesc.Height = SCREEN_HEIGHT;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hwnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++)
	{
		D3D_DRIVER_TYPE driverType = driverTypes[driverTypeIndex];
		D3D_FEATURE_LEVEL featureLevel;
		hr = D3D11CreateDeviceAndSwapChain(NULL, driverType, NULL, createDeviceFlags, featureLevels, numFeatureLevels,
			D3D11_SDK_VERSION, &sd, &swap_chain, &device, &featureLevel, &context);
		if (SUCCEEDED(hr))
			break;
	}
	if (FAILED(hr))
		return false;


	ID3D11Texture2D* pBackBuffer = NULL;
	hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
	if (FAILED(hr))
		return false;

	hr = device->CreateRenderTargetView(pBackBuffer, NULL, &render_target_view);
	pBackBuffer->Release();
	if (FAILED(hr))
		return false;


	D3D11_TEXTURE2D_DESC txDesc;
	ZeroMemory( &txDesc, sizeof( txDesc ) );
	txDesc.Width              = SCREEN_WIDTH;
	txDesc.Height             = SCREEN_HEIGHT;
	txDesc.MipLevels          = 1;
	txDesc.ArraySize          = 1;
	txDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
	txDesc.SampleDesc.Count   = 1;
	txDesc.SampleDesc.Quality = 0;
	txDesc.Usage              = D3D11_USAGE_DEFAULT;
	txDesc.BindFlags          = D3D11_BIND_DEPTH_STENCIL;
	txDesc.CPUAccessFlags     = 0;
	txDesc.MiscFlags          = 0;
	ID3D11Texture2D *pDepthStencilTexture;
	hr = device->CreateTexture2D( &txDesc, NULL, &pDepthStencilTexture );
	if ( FAILED( hr ) )		return false;

	D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
	ZeroMemory( &dsDesc, sizeof( dsDesc ) );        
	dsDesc.Format             = txDesc.Format;
	dsDesc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsDesc.Texture2D.MipSlice = 0;
	hr = device->CreateDepthStencilView( pDepthStencilTexture, &dsDesc, &depth_stencil_view );
	pDepthStencilTexture->Release();
	if ( FAILED( hr ) )		return false;

	blender::Init(device, context);

	// Setup the viewport
	D3D11_VIEWPORT vp;
	vp.Width = (FLOAT)SCREEN_WIDTH;
	vp.Height = (FLOAT)SCREEN_HEIGHT;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	context->RSSetViewports(1, &vp);

	text.Initialize(device, L"./Data/fonts/font6.png", 32);

	SetClearColor(0.0f, 0.125f, 0.3f);

	///////////////////////////////////////////////////
	//	DirectX‰Šú‰»I—¹ ˆÈ~•`‰æ•¨‘Ì‚Ì‰Šú‰»
	////////////////////////////////////////////////////

	GAME.Initialize();

	return true;
}


void framework::update(float elapsed_time/*Elapsed seconds from last frame*/)
{
	pInputManager->Run();
	GAME.Update();
}


void framework::render(float elapsed_time/*Elapsed seconds from last frame*/)
{
	// Just clear the backbuffer
	context->ClearRenderTargetView(render_target_view, clearColor);

	context->ClearDepthStencilView(
		depth_stencil_view,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f, 0 );

	context->OMSetRenderTargets(1, &render_target_view, depth_stencil_view);

	///////////////////////////////////////////////////
	//		•`‰æŠJŽn
	////////////////////////////////////////////////////

	benchmark bm;
	bm.begin();

	GAME.Render();

	float bmTimer = bm.end();



#ifdef BENCHMARK_RENDER

	static const int MAX = 100;
	static float arrayTimer[MAX] = {};
	static int pointer = 0;
	
	arrayTimer[pointer] = bmTimer;
	pointer = (pointer + 1) % MAX;
	
	float sum = 1000;
	for (auto &t : arrayTimer)
	{
		if (t == 0.0f)	continue;
		if(sum > t)	sum = t;
	}

	char str[128];
	sprintf_s(str, "BENCH MARK:%.3fms", sum*1000);

	text.Set(str, 0, 0);

#endif // BENCHMARK_RENDER

	text.Render(context);

	///////////////////////////////////////////////////
	//		•`‰æI—¹
	////////////////////////////////////////////////////
	swap_chain->Present(1, 0);
}




