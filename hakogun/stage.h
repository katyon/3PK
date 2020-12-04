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

	MyMesh				obj;				//	�u���f���I�u�W�F�N�g�v

	DirectX::XMFLOAT3	pos;				//	�u�ʒu(���W)�v
	float				angle;				//	�u��]�p�x�v
	bool                exist;

	void	Initialize(const char*);
	void	Release();
	void	Update();
	void	Render(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir);
};