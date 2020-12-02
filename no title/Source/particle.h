#pragma once

#include	<DirectXMath.h>
#include	"geometric_primitive.h"
#include	"framework.h"


/*******************************************************************************
	粒子構造体の定義
*******************************************************************************/
struct Particle
{
	DirectX::XMFLOAT3	pos				= {0,0,0};
	float				scale			= 1.0f;
	DirectX::XMFLOAT4	color			= {0,0,0,1};
	bool 				exist			= false;

	DirectX::XMFLOAT3	speed			= {0,0,0};
	DirectX::XMFLOAT3	accel			= {0,0,0};
	int					dispTimer		= 0;
	int					dispMaxTimer	= 0;
};


/*******************************************************************************
	パーティクル管理クラスの定義
*******************************************************************************/
class ParticleManager
{
private:
	static const int		MAX = 1000;
	Particle				data[MAX];
	GeometricBoard			*polygon;

	ParticleManager() : polygon(nullptr){
		polygon = new GeometricBoard(FRAMEWORK.getDevice());
	}
	~ParticleManager()	{
		if (polygon)		delete	polygon;
	}

public:
	void	Update();
	void	Render(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection);
	bool	Set(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 speed, DirectX::XMFLOAT3 accel,
				float scale, DirectX::XMFLOAT4 color, int timer);
	bool	Set(const Particle& d);

	static ParticleManager*	getInstance()
	{
		static ParticleManager instance;
		return	&instance;
	}
};

#define	pParticleManager	ParticleManager::getInstance()


