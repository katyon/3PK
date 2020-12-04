#pragma once


#include <d3d11.h>
#include <directxmath.h>

inline	ID3D11Device* getDevice();
inline	ID3D11DeviceContext* getContext();
inline	bool					SetText(const char* str, float x, float y, float dispSizeW = 0, float dispSizeH = 0);
