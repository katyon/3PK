#pragma once

#include <directxmath.h>

#include "static_mesh.h"
#include "skinned_mesh.h"

class MyMesh
{
private:
	bool				bLoad;

public:
	static_mesh*	obj;
	skinned_mesh* skinned_obj;

	DirectX::XMFLOAT3	pos;				//	「位置」
	DirectX::XMFLOAT3	angle;				//	「(回転)角度」
	DirectX::XMFLOAT3	scale;				//	「大きさ」

	DirectX::XMFLOAT4	color;

	void	Initialize();
	bool	Load(ID3D11Device* device,const char* fbx_filename);
	void	SetPrimitive(static_mesh* primitive);

	void	SetMesh(MyMesh& org);
	void	Release();
	void	Render(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir);

	DirectX::XMMATRIX	GetWorldMatrix();

};


