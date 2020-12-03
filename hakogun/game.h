#pragma once

#include	"directxmath.h"

class Game
{
public:
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX projection;
	DirectX::XMFLOAT4 light_direction;


	void	Initialize();
	void	Release();
	bool	Update();
	void	Render();


	static Game& getInstance()
	{
		static Game instance;
		return	instance;
	}
};

#define	pGame Game::getInstance()
