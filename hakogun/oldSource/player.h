#pragma once

#include	<DirectXMath.h>

#include	"my_mesh.h"



/*******************************************************************************
	�u�v���C���[�v�N���X
*******************************************************************************/
#define	pPlayer Player::getInstance()
class Player
{
public:
	static Player& getInstance()
	{
		static Player instance;
		return	instance;
	}

	MyMesh				skinned_obj;				//	�u���f���I�u�W�F�N�g�v
	MyMesh				skinned_obj1;				//	 �c���p
	MyMesh				skinned_obj2;				//	 �c���p

	DirectX::XMFLOAT3	pos;				//	�u�ʒu(���W)�v
	float				angle;				//	�u��]�p�x�v
	float				accele = 0.0f;
	int                 accele_timer = 2.0;
	bool                acceleFlg = false;

	bool				left_dash = false;
	bool				right_dash = false;
	bool				up_dash = false;
	bool				down_dash = false;

	float				X_movement;
	float				Z_movement;

	int					previous_timer = 0;
	DirectX::XMFLOAT3	previous_pos1;
	DirectX::XMFLOAT3	previous_pos2;

	void	Dash();

	void	Initialize(ID3D11Device* device, const char* fbx_filename);																							//	�������֐�
	void	Release();																											//	����֐�
	void	Move(ID3D11Device* device);																												//	�ړ��֐�
	void	Render(ID3D11DeviceContext* context,const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir );	//	�`��֐�

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
