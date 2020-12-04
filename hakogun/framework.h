#pragma once

#include <windows.h>
#include <tchar.h>
#include <sstream>

#include <d3d11.h>

#include "misc.h"
#include "high_resolution_timer.h"

#include "blender.h"
#include "sprite.h"

#include "Game.h"


//#define		BENCHMARK_RENDER

const LONG SCREEN_WIDTH = 1280;
const LONG SCREEN_HEIGHT = 720;
const LONG FRAME_RATE = 60;			//	これを変更しても実際のフレームレートは変わらない　どこで変わっているかは自分で調査しなさい


class framework
{
	static const int _max = 1024;

	const HWND hwnd;

	framework(HWND hwnd) : hwnd(hwnd), device(nullptr), context(nullptr),
		swap_chain(nullptr), render_target_view(nullptr), depth_stencil_view(nullptr)
	{}
	~framework()
	{
		blender::Release();

		pGame.Release();

		if(depth_stencil_view)	depth_stencil_view->Release();
		if(render_target_view)	render_target_view->Release();
		if(swap_chain)			swap_chain->Release();
		if(context)				context->Release();
		if(device)				device->Release();
	}

public:
	static framework& getInstance(HWND hwnd = nullptr)
	{
		static framework instance(hwnd);
		return instance;
	}

	int run()
	{
		MSG msg = {};

		if (!initialize()) return 0;
		while (WM_QUIT != msg.message)
		{
			if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else
			{
				timer.tick();
				calculate_frame_stats();
				update(timer.time_interval());
				render(timer.time_interval());
			}
		}
		return static_cast<int>(msg.wParam);
	}

	LRESULT CALLBACK handle_message(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
	{
		switch (msg)
		{
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc;
			hdc = BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
			break;
		}
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_CREATE:
			break;
		case WM_KEYDOWN:
			if (wparam == VK_ESCAPE) PostMessage(hwnd, WM_CLOSE, 0, 0);
			break;
		case WM_ENTERSIZEMOVE:
			// WM_EXITSIZEMOVE is sent when the user grabs the resize bars.
			timer.stop();
			break;
		case WM_EXITSIZEMOVE:
			// WM_EXITSIZEMOVE is sent when the user releases the resize bars.
			// Here we reset everything based on the new window dimensions.
			timer.start();
			break;
		default:
			return DefWindowProc(hwnd, msg, wparam, lparam);
		}
		return 0;
	}

private:
	ID3D11Device*				device;
	ID3D11DeviceContext*		context;
	IDXGISwapChain*				swap_chain;
	ID3D11RenderTargetView*		render_target_view;
	ID3D11DepthStencilView*		depth_stencil_view;

	float						clearColor[4];

	Text						text;


	bool initialize();
	void update(float elapsed_time);
	void render(float elapsed_time);

private:
	high_resolution_timer timer;
	void calculate_frame_stats()
	{
		// Code computes the average frames per second, and also the 
		// average time it takes to render one frame.  These stats 
		// are appended to the window caption bar.
		static int		frames = 0;
		static float	time_tlapsed = 0.0f;

		frames++;

		// Compute averages over one second period.
		if ((timer.time_stamp() - time_tlapsed) >= 1.0f)
		{
			float fps = static_cast<float>(frames);				// fps = frameCnt / 1
			float mspf = 1000.0f / fps;
			std::ostringstream outs;
			outs.precision(6);
			outs << "FPS : " << fps << " / " << "Frame Time : " << mspf << " (ms)";
			SetWindowTextA(hwnd, outs.str().c_str());

			// Reset for next average.
			frames = 0;
			time_tlapsed += 1.0f;
		}
	}

public:
	inline bool	SetText(const char* str, float x, float y, float dispSizeW = 0, float dispSizeH = 0)
	{
		return	text.Set(str, x, y, dispSizeW, dispSizeH);
	}
	inline void	SetClearColor(float r, float g, float b)
	{
		clearColor[0] = r, clearColor[1] = g, clearColor[2] = b;
		clearColor[3] = 1.0f;										//	ここは常に1
	}
	inline static ID3D11Device*			getDevice()	{ return	framework::getInstance().device;	};
	inline static ID3D11DeviceContext*	getContext(){ return	framework::getInstance().context;	};
};

#define	pframework framework::getInstance()
