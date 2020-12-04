#pragma once

#include	<DirectXMath.h>

#include	"my_mesh.h"

#define	pStage	Stage::getInstance()

class Stage
{
public:
	static Stage& getInstance()
	{
		static Stage instance;
		return	instance;
	}

	MyMesh				obj;				//	「モデルオブジェクト」

	DirectX::XMFLOAT3	pos;				//	「位置(座標)」
	float				angle;				//	「回転角度」
	bool                exist;

	void	Initialize(const char*);
	void	Release();
	void	Update();
	void	Render(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir);
};