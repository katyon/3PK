#pragma once

#include	"directxmath.h"

class Game
{
public:
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX projection;
	DirectX::XMFLOAT4 light_direction;

	bool    warpFlg;
	float	ƒ¿ = 1.0f;  //@•`‰æ—p•Ï”


	void	Initialize();
	void	Release();
	bool	Update();
	void	Render();

	bool	Explanation[8];

	static Game& getInstance()
	{
		static Game instance;
		return	instance;
	}
};

#define	pGame	Game::getInstance()
