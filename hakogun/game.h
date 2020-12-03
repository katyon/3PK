#pragma once
#include <d3d11.h>
#include	"directxmath.h"

class Game
{
public:
	DirectX::XMMATRIX view;
	DirectX::XMMATRIX projection;
	DirectX::XMFLOAT4 light_direction;


	void	Initialize(ID3D11Device* device);
	void	Release();
	bool	Update(ID3D11Device* device);
	void	Render(ID3D11DeviceContext* context);


	static Game& getInstance()
	{
		static Game instance;
		return	instance;
	}
};

#define	pGame Game::getInstance()
