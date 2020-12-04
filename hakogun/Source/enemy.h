#pragma once

#include	<DirectXMath.h>

#include	"my_mesh.h"

/*******************************************************************************
TODO:08 �G�̐������̎擾
����    WAVE�Ǘ��������Ő�΂ɕK�v�ɂȂ���Ƃ��āA
		�G�̐��������擾������@���쐬����
--------------------------------------------------------------------------------
�菇    �G�Ǘ��N���X����u���ݐ���(����)���Ă���G�̐��v��
		�m����@���쐬����
		����A��̓I�Ȏ�i�͎w�����Ȃ����A�\�Ȍ���
		�ᕉ�S/�����ɂȂ�悤�Ɉӎ����鎖
*******************************************************************************/

/*******************************************************************************
	�u�G�v�N���X
*******************************************************************************/
class Enemy
{
private:
	//	�G�s���Ǘ��ԍ��̐錾
	enum	ENEMY_ACT
	{
		INITIALIZE	= 0,		//	�u�������v
		SELECT,					//	�u�s���I���v
		WAIT_INIT,				//	�u�ҋ@_�������v
		WAIT_PROC,				//	�u�ҋ@_�����v
		STRAIGHT_INIT,			//	�u���i_�������v
		STRAIGHT_PROC,			//	�u���i_�����v
		ROTATE_INIT,			//	�u����_�������v
		ROTATE_PROC,			//	�u����_�����v
		TARGET_INIT,			//	�u�v���C���[�֒��i_�������v
		TARGET_PROC,			//	�u�v���C���[�֒��i_�����v
		SHOT_NORMAL_INIT,
		SHOT_NORMAL_PROC,
		SHOT_FAN_INIT,
		SHOT_FAN_PROC,
		BOSS_NORMAL_INIT,
		BOSS_NORMAL_PROC,
		BOSS_FAN_INIT,
		BOSS_FAN_PROC,
		BOSS_TORNADO_INIT,
		BOSS_TORNADO_PROC,
	};

public:
	MyMesh				obj;		//	�u���f���I�u�W�F�N�g�v
	DirectX::XMFLOAT3	pos;		//	�u�ʒu(���W)�v
	float				angle;		//	�u��]�p�x�v

	float				before_angle; // ���݂̌���
	float				after_angle;  // �ڕW�̌���(�v���C���[����)
	float               slerp_angle;  // �Ȃ��p 

	DirectX::XMFLOAT4	color;		//	�u�F�v
	bool				exist;		//	�u���݃t���O�v
	int					rateSelect[10] = {};


	//	�G�v�N���X�ɏ���ǉ�
	int					state;		//	�u��ԁv
	int					timer;		//	�u�^�C�}�[�v
	int                 hp;

	float               tank_rotate;

	void	Initialize( const char* );																							//	�������֐�

	void	Release();																											//	����֐�
	void	Move();																												//	�ړ��֐�
	void	Render( const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir );	//	�`��֐�
};


/*******************************************************************************
	�u�G�Ǘ��v�N���X
*******************************************************************************/
class EnemyManager
{
public:
	static const int	MAX = 32;				//	�u�ő吔�v	

private:
	Enemy				data[MAX];				//	�u�G�f�[�^�v


public:
	int     select_action_count = 0;

	void	Initialize();																										//	�������֐�
	void	Release();																											//	����֐�
	void	Update();																											//	�X�V�֐�
	void	Render( const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir );	//	�`��֐�
  
	bool    AP();

	Enemy*	Set(const char* filename, DirectX::XMFLOAT3 pos, float angle, DirectX::XMFLOAT4 color, int hp);								//	�G�ݒ�֐�
	Enemy* EnemyManager::Set(MyMesh& mesh, DirectX::XMFLOAT3 pos, float angle, DirectX::XMFLOAT4 color, int hp);

	Enemy*	Get(int no){
		if ((unsigned int)no >= MAX) return	nullptr;
		return		&data[no];
	}


};

extern	EnemyManager	enemyManager;		//	��������ǉ�
