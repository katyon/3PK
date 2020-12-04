#pragma once

#include	<DirectXMath.h>

#include	"my_mesh.h"



/*******************************************************************************
	�u�v���C���[�v�N���X
*******************************************************************************/
class Player
{
public:
	MyMesh				obj;				//	�u���f���I�u�W�F�N�g�v
	MyMesh				obj1;				//	 �c���p
	MyMesh				obj2;				//	 �c���p

	MyMesh				turret;             //   �U���p�^���b�g

	DirectX::XMFLOAT3	pos;				//	�u�ʒu(���W)�v
	float				angle;				//	�u��]�p�x�v
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
	float				turret_angle;				//	�u��]�p�x�v
	int                 turret_timer;

	bool                resetFlg = false;
	int                 teleport_time = 0;

	void	Dash();

	void	Initialize( const char* );																							//	�������֐�
	void	Release();																											//	����֐�
	void	Move();																												//	�ړ��֐�
	void	Render( const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir );	//	�`��֐�

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
