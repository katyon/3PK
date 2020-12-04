#include "framework.h"
#include "input.h"
#include "scene_manager.h"

#include "player.h"

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
    ZeroMemory(&txDesc, sizeof(txDesc));
    txDesc.Width = SCREEN_WIDTH;
    txDesc.Height = SCREEN_HEIGHT;
    txDesc.MipLevels = 1;
    txDesc.ArraySize = 1;
    txDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    txDesc.SampleDesc.Count = 1;
    txDesc.SampleDesc.Quality = 0;
    txDesc.Usage = D3D11_USAGE_DEFAULT;
    txDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    txDesc.CPUAccessFlags = 0;
    txDesc.MiscFlags = 0;
    ID3D11Texture2D* pDepthStencilTexture;
    hr = device->CreateTexture2D(&txDesc, NULL, &pDepthStencilTexture);
    if (FAILED(hr))		return false;

    D3D11_DEPTH_STENCIL_VIEW_DESC dsDesc;
    ZeroMemory(&dsDesc, sizeof(dsDesc));
    dsDesc.Format = txDesc.Format;
    dsDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsDesc.Texture2D.MipSlice = 0;
    hr = device->CreateDepthStencilView(pDepthStencilTexture, &dsDesc, &depth_stencil_view);
    pDepthStencilTexture->Release();
    if (FAILED(hr))		return false;

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

    SetClearColor(0.0f, 0.0f, 0.0f);

    ///////////////////////////////////////////////////
    //	DirectXèâä˙âªèIóπ à»ç~ï`âÊï®ëÃÇÃèâä˙âª
    ////////////////////////////////////////////////////

    sprites[0] = std::make_unique<Sprite>(device, L"./Data/Sprites/title.png");
    sprites[1] = std::make_unique<Sprite>(device, L"./Data/Sprites/clear.png");

    pSceneManager.init();

    return true;
}


void framework::update(float elapsed_time/*Elapsed seconds from last frame*/)
{
    pInputManager->Run();
    pSceneManager.update();
}


void framework::render(float elapsed_time/*Elapsed seconds from last frame*/)
{
    // Just clear the backbuffer
    context->ClearRenderTargetView(render_target_view, clearColor);

    context->ClearDepthStencilView(
        depth_stencil_view,
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        1.0f, 0);

    context->OMSetRenderTargets(1, &render_target_view, depth_stencil_view);

    ///////////////////////////////////////////////////
    //		ï`âÊäJén
    ////////////////////////////////////////////////////

    benchmark bm;
    bm.begin();

    pSceneManager.render();

    float bmTimer = bm.end();

    //	// Just clear the backbuffer
    //	context->ClearRenderTargetView(render_target_view, clearColor);
    //
    //	context->ClearDepthStencilView(
    //		depth_stencil_view,
    //		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
    //		1.0f, 0);
    //
    //	context->OMSetRenderTargets(1, &render_target_view, depth_stencil_view);

    //
    //	benchmark bm;
    //	bm.begin();
    //
    //	pSceneManager.render();
    //
    //	float bmTimer = bm.end();

        //DirectX::XMFLOAT4 light_direction(0, 0, 1, 0);

        //DirectX::XMMATRIX P;	// projection matrix
        //{
        //	D3D11_VIEWPORT viewport;
        //	UINT num_viewports = 1;
        //	immediate_context->RSGetViewports(&num_viewports, &viewport);
        //	float aspect_ratio = viewport.Width / viewport.Height;
        //	//P = DirectX::XMMatrixOrthographicLH(2 * aspect_ratio, 2, 0.1f, 100.0f);
        //	P = DirectX::XMMatrixPerspectiveFovLH(30 * 0.01745f, aspect_ratio, 0.1f, 1000.0f);
        //}
        //DirectX::XMMATRIX V;	// view matrix
        //{
        //	DirectX::XMVECTOR eye, focus, up;
        //	eye = DirectX::XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f);
        //	focus = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
        //	up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        //	V = DirectX::XMMatrixLookAtLH(eye, focus, up);
        //}
        //DirectX::XMMATRIX W;	// world matrix
        //{
        //	DirectX::XMFLOAT3 position(0, 0, 0);
        //	//DirectX::XMFLOAT3 dimensions(1.0f, 1.0f, 1.0f);
        //	DirectX::XMFLOAT3 dimensions(0.01f, 0.01f, 0.01f);
        //	static DirectX::XMFLOAT3 angles(0, 0, 0);

        //	angles.x += 30 * elapsed_time;
        //	angles.y += 30 * elapsed_time;
        //	angles.z += 30 * elapsed_time;

        //	DirectX::XMMATRIX S, R, T;
        //	//W = DirectX::XMMatrixIdentity();
        //	S = DirectX::XMMatrixScaling(dimensions.x, dimensions.y, dimensions.z);
        //	R = DirectX::XMMatrixRotationRollPitchYaw(angles.x * 0.01745f, angles.y * 0.01745f, angles.z * 0.01745f);
        //	T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
        //	W = S * R * T;
        //}

        //static bool wireframe = false;
        //if (GetAsyncKeyState(' ') & 1)
        //{
        //	wireframe = !wireframe;
        //}

        ////immediate_context->OMSetBlendState(blender.states[blender::BS_ALPHA].Get(), nullptr, 0xFFFFFFFF);
        //{
        //	DirectX::XMFLOAT4X4 world_view_projection;
        //	DirectX::XMFLOAT4X4 world;

        //	DirectX::XMMATRIX PW;	// world matrix
        //	{
        //		DirectX::XMFLOAT3 position = pPlayer.pos;
        //		DirectX::XMFLOAT3 dimensions(1.0f, 1.0f, 1.0f);
        //		static DirectX::XMFLOAT3 angles(0.0f, pPlayer.angle, 0.0f);

        //		DirectX::XMMATRIX S, R, T;
        //		//W = DirectX::XMMatrixIdentity();
        //		S = DirectX::XMMatrixScaling(dimensions.x, dimensions.y, dimensions.z);
        //		R = DirectX::XMMatrixRotationRollPitchYaw(angles.x * 0.01745f, angles.y * 0.01745f, angles.z * 0.01745f);
        //		T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
        //		PW = S * R * T;
        //	}

        //	//DirectX::XMStoreFloat4x4(&world_view_projection, W * V * P);
        //	//DirectX::XMStoreFloat4x4(&world, W);
        //	DirectX::XMFLOAT4 material_color(1.0f, 1.0f, 1.0f, 1.0f);

        //	//player->render(immediate_context.Get(), world_view_projection, world, light_direction, material_color, wireframe);

        //	DirectX::XMStoreFloat4x4(&world_view_projection, PW * pGame.view * pGame.projection);
        //	DirectX::XMStoreFloat4x4(&world, PW);

        //	player->render(immediate_context.Get(), world_view_projection, world, pGame.light_direction, material_color, wireframe);

        //	pSceneManager.render(immediate_context.Get());

        //	//DirectX::XMFLOAT2 pos = {};
        //	//pos = mouse->GetMousePos();
        //	//char str[128];

        //	//sprintf_s(str, 128, "posx = %.2f", pos.x);
        //	//font->textout(immediate_context.Get(), str, 0, 0, 16, 16, 1, 1, 1, 1);
        //	//sprintf_s(str, 128, "posy = %.2f", pos.y);
        //	//font->textout(immediate_context.Get(), str, 0, 40, 16, 16, 1, 1, 1, 1);

        //	//DirectX::XMFLOAT2 velocity = mouse->GetMouseVelocity();

        //	//sprintf_s(str, 128, "velocity_x = %.2f", velocity.x);
        //	//font->textout(immediate_context.Get(), str, 0, 80, 16, 16, 1, 1, 1, 1);
        //	//sprintf_s(str, 128, "velocity_y = %.2f", velocity.y);
        //	//font->textout(immediate_context.Get(), str, 0, 120, 16, 16, 1, 1, 1, 1);

    //#ifdef BENCHMARK_RENDER
    //
    //	static const int MAX = 100;
    //	static float arrayTimer[MAX] = {};
    //	static int pointer = 0;
    //
    //	arrayTimer[pointer] = bmTimer;
    //	pointer = (pointer + 1) % MAX;
    //
    //	float sum = 1000;
    //	for (auto& t : arrayTimer)
    //	{
    //		if (t == 0.0f)	continue;
    //		if (sum > t)	sum = t;
    //	}
    //
    //	char str[128];
    //	sprintf_s(str, "BENCH MARK:%.3fms", sum * 1000);
    //
    //	text.Set(str, 0, 0);
    //
    //#endif // BENCHMARK_RENDER
    //
    //	text.Render(context);
    //}

#ifdef BENCHMARK_RENDER

    static const int MAX = 100;
    static float arrayTimer[MAX] = {};
    static int pointer = 0;

    arrayTimer[pointer] = bmTimer;
    pointer = (pointer + 1) % MAX;

    float sum = 1000;
    for (auto& t : arrayTimer)
    {
        if (t == 0.0f)	continue;
        if (sum > t)	sum = t;
    }

    char str[128];
    sprintf_s(str, "BENCH MARK:%.3fms", sum * 1000);

    text.Set(str, 0, 0);

#endif // BENCHMARK_RENDER
#ifdef _DEBUG
    char str[128];
    extern Player player;
    sprintf_s(str, "plpos.x:%f", player.pos.x);
    text.Set(str, 0, 0);
    sprintf_s(str, "plpos.y:%f", player.pos.y);
    text.Set(str, 0, 30);
    sprintf_s(str, "plpos.z:%f", player.pos.z);
    text.Set(str, 0, 60);
#endif // _DEBUG


    text.Render(context);

    ///////////////////////////////////////////////////
    //		ï`âÊèIóπ
    ////////////////////////////////////////////////////
    swap_chain->Present(1, 0);
}




