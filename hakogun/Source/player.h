#pragma once

#include	<DirectXMath.h>

#include	"my_mesh.h"



/*******************************************************************************
	「プレイヤー」クラス
*******************************************************************************/
class Player
{
public:
	MyMesh				obj;				//	「モデルオブジェクト」
	MyMesh				obj1;				//	 残像用
	MyMesh				obj2;				//	 残像用

	MyMesh				turret;             //   攻撃用タレット

	DirectX::XMFLOAT3	pos;				//	「位置(座標)」
	float				angle;				//	「回転角度」
	bool                exist;

	float				accele = 0.0f;
	int                 accele_timer = 2.0;
	bool                acceleFlg = false;

	int                 hp;
	bool				left_dash = false;
	bool				right_dash = false;
	bool				up_dash = false;
	bool				down_dash = false;

	int					previous_timer = 0;
	DirectX::XMFLOAT3	previous_pos1;
	DirectX::XMFLOAT3	previous_pos2;

	DirectX::XMFLOAT3	turret_pos;
	float				turret_angle;				//	「回転角度」
	int                 turret_timer;

	bool                resetFlg = false;
	int                 teleport_time = 0;

	void	Dash();

	void	Initialize( const char* );																							//	初期化関数
	void	Release();																											//	解放関数
	void	Move();																												//	移動関数
	void	Render( const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir );	//	描画関数

	DirectX::XMFLOAT3 GetPosition();
};

class PlayerAfterImage : public Player
{
public:
	DirectX::XMFLOAT3 pos;
	void Move();
	void Render(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir);
};


#pragma region store

//if (acceleFlg && pInputManager->inputKeyState(DIK_D))
//{
//	accele -= 0.1f;
//	pos.x += speed * accele;
//	pos.x += speed;
//}
//if (acceleFlg && pInputManager->inputKeyState(DIK_A))
//{
//	accele -= 0.1f;
//	pos.x -= speed * accele;
//	pos.x -= speed;
//}
//if (acceleFlg && pInputManager->inputKeyState(DIK_W))
//{
//	accele -= 0.1f;
//	pos.z += speed * accele;
//	pos.z += speed;
//}
//if (acceleFlg && pInputManager->inputKeyState(DIK_S))
//{
//	accele -= 0.1f;
//	pos.z -= speed * accele;
//	pos.z -= speed;
//}
#pragma endregion
