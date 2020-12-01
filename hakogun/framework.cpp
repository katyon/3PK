#include "framework.h"
#include "shader.h"

#include <memory>

#include "blender.h"

#include <algorithm>
#include <vector>
#include <queue>

#include "geometric_primitive.h"
#include <stdlib.h>

bool framework::initialize()
{
	HRESULT hr = S_OK;

	RECT rc;
	GetClientRect(hwnd, &rc);
	UINT screen_width = rc.right - rc.left;
	UINT screen_height = rc.bottom - rc.top;

	// Create Device
	{
		UINT create_device_flags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
		create_device_flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		D3D_DRIVER_TYPE driver_types[] =
		{
			D3D_DRIVER_TYPE_UNKNOWN,
			D3D_DRIVER_TYPE_HARDWARE,
			D3D_DRIVER_TYPE_WARP,
			D3D_DRIVER_TYPE_REFERENCE,
		};
		D3D_FEATURE_LEVEL feature_levels[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1,
		};
		D3D_FEATURE_LEVEL feature_level;
		for (D3D_DRIVER_TYPE driver_type : driver_types)
		{
			hr = D3D11CreateDevice(nullptr, driver_type, nullptr, create_device_flags, feature_levels, ARRAYSIZE(feature_levels), D3D11_SDK_VERSION, device.GetAddressOf(), &feature_level, immediate_context.GetAddressOf());
			if (SUCCEEDED(hr))
			{
				break;
			}
		}
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	// Create Swap Chain
	BOOL enable_4x_msaa = TRUE;
	{
		DXGI_SWAP_CHAIN_DESC swap_chain_desc;
		swap_chain_desc.BufferDesc.Width = screen_width;
		swap_chain_desc.BufferDesc.Height = screen_height;
		swap_chain_desc.BufferDesc.RefreshRate.Numerator = 60;
		swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
		swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		UINT msaa_quality_level;
		device->CheckMultisampleQualityLevels(swap_chain_desc.BufferDesc.Format, 4, &msaa_quality_level);
		swap_chain_desc.SampleDesc.Count = enable_4x_msaa ? 4 : 1;
		swap_chain_desc.SampleDesc.Quality = enable_4x_msaa ? msaa_quality_level - 1 : 0;
		swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swap_chain_desc.BufferCount = 1;
		swap_chain_desc.OutputWindow = hwnd;
		swap_chain_desc.Windowed = TRUE;
		swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
		swap_chain_desc.Flags = 0;

		Microsoft::WRL::ComPtr<IDXGIFactory1> dxgi_factory;
		Microsoft::WRL::ComPtr<IDXGIDevice> dxgi_device;

		HRESULT hr = device.Get()->QueryInterface(__uuidof(IDXGIDevice), reinterpret_cast<void**>(dxgi_device.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		Microsoft::WRL::ComPtr<IDXGIAdapter> adapter;
		hr = dxgi_device->GetAdapter(adapter.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = adapter->GetParent(__uuidof(IDXGIFactory1), reinterpret_cast<void**>(dxgi_factory.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = dxgi_factory->CreateSwapChain(device.Get(), &swap_chain_desc, swap_chain.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}
	// Create Render Target View
	D3D11_TEXTURE2D_DESC back_buffer_desc;
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> back_buffer;
		hr = swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(back_buffer.GetAddressOf()));
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		hr = device->CreateRenderTargetView(back_buffer.Get(), NULL, render_target_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		back_buffer->GetDesc(&back_buffer_desc);
	}
	// Create Depth Stencil View
	D3D11_TEXTURE2D_DESC depth_stencil_buffer_desc = back_buffer_desc;
	{
		Microsoft::WRL::ComPtr<ID3D11Texture2D> depth_stencil_buffer;
		depth_stencil_buffer_desc.MipLevels = 1;
		depth_stencil_buffer_desc.ArraySize = 1;
		depth_stencil_buffer_desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;	//DXGI_FORMAT_D32_FLOAT
		depth_stencil_buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		depth_stencil_buffer_desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depth_stencil_buffer_desc.CPUAccessFlags = 0;
		depth_stencil_buffer_desc.MiscFlags = 0;
		hr = device->CreateTexture2D(&depth_stencil_buffer_desc, NULL, depth_stencil_buffer.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));

		D3D11_DEPTH_STENCIL_VIEW_DESC depth_stencil_view_desc;
		depth_stencil_view_desc.Format = depth_stencil_buffer_desc.Format;
		depth_stencil_view_desc.ViewDimension = enable_4x_msaa ? D3D11_DSV_DIMENSION_TEXTURE2DMS : D3D11_DSV_DIMENSION_TEXTURE2D;
		depth_stencil_view_desc.Flags = 0;
		depth_stencil_view_desc.Texture2D.MipSlice = 0;
		hr = device->CreateDepthStencilView(depth_stencil_buffer.Get(), &depth_stencil_view_desc, depth_stencil_view.GetAddressOf());
		_ASSERT_EXPR(SUCCEEDED(hr), hr_trace(hr));
	}

	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<FLOAT>(screen_width);
	viewport.Height = static_cast<FLOAT>(screen_height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	immediate_context->RSSetViewports(1, &viewport);

	if (mouse->InitInput() == false)
	{
		return false;
	}

	//sprites[0] = std::make_unique<sprite>(device.Get(), L"player-sprites.png");
	font = std::make_unique<sprite>(device.Get(), L"./fonts/font0.png");

	fbx_mesh = std::make_unique<skinned_mesh>(device.Get(), "kikagaku100.fbx");
	//fbx_mesh = std::make_unique<skinned_mesh>(device.Get(), "tank.fbx");

	//fbx_mesh = std::make_unique<skinned_mesh>(device.Get(), "kikagaku500.fbx");

	return true;
}
void framework::update(float elapsed_time/*Elapsed seconds from last frame*/)
{
	mouse->UpdateInput();
}
void framework::render(float elapsed_time/*Elapsed seconds from last frame*/)
{
	HRESULT hr = S_OK;

	FLOAT color[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	immediate_context->ClearRenderTargetView(render_target_view.Get(), color);
	immediate_context->ClearDepthStencilView(depth_stencil_view.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	immediate_context->OMSetRenderTargets(1, render_target_view.GetAddressOf(), depth_stencil_view.Get());

	static float angle = 0;
	angle += 6.0f * elapsed_time; // 1 round per 60 seconds


	static blender blender(device.Get());


	DirectX::XMFLOAT4 light_direction(0, 0, 1, 0);

	DirectX::XMMATRIX P;	// projection matrix
	{
		D3D11_VIEWPORT viewport;
		UINT num_viewports = 1;
		immediate_context->RSGetViewports(&num_viewports, &viewport);
		float aspect_ratio = viewport.Width / viewport.Height;
		//P = DirectX::XMMatrixOrthographicLH(2 * aspect_ratio, 2, 0.1f, 100.0f);
		P = DirectX::XMMatrixPerspectiveFovLH(30 * 0.01745f, aspect_ratio, 0.1f, 1000.0f);
	}
	DirectX::XMMATRIX V;	// view matrix
	{
		DirectX::XMVECTOR eye, focus, up;
		eye = DirectX::XMVectorSet(0.0f, 0.0f, -10.0f, 1.0f);
		focus = DirectX::XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		V = DirectX::XMMatrixLookAtLH(eye, focus, up);
	}
	DirectX::XMMATRIX W;	// world matrix
	{
		DirectX::XMFLOAT3 position(0, 0, 0);
		//DirectX::XMFLOAT3 dimensions(1.0f, 1.0f, 1.0f);
		DirectX::XMFLOAT3 dimensions(0.01f, 0.01f, 0.01f);
		static DirectX::XMFLOAT3 angles(0, 0, 0);

		angles.x += 30 * elapsed_time;
		angles.y += 30 * elapsed_time;
		angles.z += 30 * elapsed_time;

		DirectX::XMMATRIX S, R, T;
		//W = DirectX::XMMatrixIdentity();
		S = DirectX::XMMatrixScaling(dimensions.x, dimensions.y, dimensions.z);
		R = DirectX::XMMatrixRotationRollPitchYaw(angles.x*0.01745f, angles.y*0.01745f, angles.z*0.01745f);
		T = DirectX::XMMatrixTranslation(position.x, position.y, position.z);
		W = S * R * T;
	}

	static bool wireframe = false;
	if (GetAsyncKeyState(' ') & 1)
	{
		wireframe = !wireframe;
	}

	immediate_context->OMSetBlendState(blender.states[blender::BS_ALPHA].Get(), nullptr, 0xFFFFFFFF);
	{
		DirectX::XMFLOAT4X4 world_view_projection;
		DirectX::XMFLOAT4X4 world;

		DirectX::XMStoreFloat4x4(&world_view_projection, W * V * P);
		DirectX::XMStoreFloat4x4(&world, W);

		DirectX::XMFLOAT4 material_color(1.0f, 1.0f, 1.0f, 1.0f);

		fbx_mesh->render(immediate_context.Get(), world_view_projection, world, light_direction, material_color, wireframe);

		DirectX::XMFLOAT2 pos = {};
		pos = mouse->GetMousePos();
		char str[128];

		sprintf_s(str, 128, "posx = %.2f", pos.x);
		font->textout(immediate_context.Get(), str, 0, 0, 16, 16, 1, 1, 1, 1);
		sprintf_s(str, 128, "posy = %.2f", pos.y);
		font->textout(immediate_context.Get(), str, 0, 40, 16, 16, 1, 1, 1, 1);

		DirectX::XMFLOAT2 velocity = mouse->GetMouseVelocity();

		sprintf_s(str, 128, "velocity_x = %.2f", velocity.x);
		font->textout(immediate_context.Get(), str, 0, 80, 16, 16, 1, 1, 1, 1);
		sprintf_s(str, 128, "velocity_y = %.2f", velocity.y);
		font->textout(immediate_context.Get(), str, 0, 120, 16, 16, 1, 1, 1, 1);
	}

	swap_chain->Present(0, 0);
}

