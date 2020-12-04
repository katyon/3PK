#pragma once

#include <directxmath.h>

#include "static_mesh.h"


class MyMesh
{
private:
	bool				bLoad;

public:
	GeometricPrimitive*	obj;

	DirectX::XMFLOAT3	pos;				//	「位置」
	DirectX::XMFLOAT3	angle;				//	「(回転)角度」
	DirectX::XMFLOAT3	scale;				//	「大きさ」

	DirectX::XMFLOAT4	color;

	void	Initialize();
	bool	Load(const char* fbx_filename);
	void	SetPrimitive(GeometricPrimitive* primitive);
	void	SetMesh(MyMesh& org);
	void	Release();
	void	Render(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir);

	DirectX::XMMATRIX	GetWorldMatrix();

};