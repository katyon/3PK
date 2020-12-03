#include	"game.h"
#include	"framework.h"

#include	"my_mesh.h"
#include	"player.h"
#include	"enemy.h"
#include	"camera.h"
#include	"shot.h"
#include	"particle.h"
#include	"wave.h"

#include	"collision.h"

#include	<time.h>
#include    <Windows.h>

/*******************************************************************************
	�O���[�o���ϐ�
*******************************************************************************/
MyMesh			field;				//	�n�ʗp�ϐ��u�n�ʁv
MyMesh			portal;
Player			player;				//	�v���C���[�p�ϐ��u�v���C���[�v
Enemy           enemy;
Camera			camera;				//	�J�����p�ϐ��u�J�����v
EnemyManager	enemyManager;		//	�G�Ǘ��p�ϐ��u�G�Ǘ��v
ShotManager		shotManager;		//	�e�Ǘ��p�ϐ��u�e�Ǘ��v

PlayerAfterImage player_after_image;

extern char* modelNames01[];
extern Wave waveData01[];

/*******************************************************************************
TODO:07 WAVE�Ǘ��N���X�̌Ăяo��
����    Game�N���X�̊e����WAVE�Ǘ��N���X�̃��\�b�h���Ăяo��
--------------------------------------------------------------------------------
�菇    Game�N���X�̊e���ňȉ��̏��������s����
		�EGame::Initialize�֐���WAVE�Ǘ��N���X�̏������֐����Ăяo��
		�@���̎��A������TODO_06�Œǉ������f�[�^�Ƃ���
		�EGame::Initialize�֐��ňȑO���炠�����G�o���������R�����g������
		�@(�G�o����WAVE�Ǘ��N���X�Ɉꌳ������ׁA�s�v�ƂȂ�)
		�EGame::Update�֐���WAVE�Ǘ��N���X�̐����֐����Ăяo��
		�@���̎��A�G�̍X�V�����O�ɍs�����������肷��
		�EGame::Update�֐���(�g���Ă��Ȃ�)�����̃L�[������������
		�@WAVE�Ǘ��N���X��WAVE�i�s�֐����Ăяo��
�m�F	�N�����ɓG��3�̏o�����A�ݒ肵���L�[�������x�ɓG��2�́�3�̂Əo�������OK
*******************************************************************************/

/*******************************************************************************
TODO:09 WAVE�S�Ō��m�ɔ��������Ăяo��
����    �G�̌�WAVE�S�ł��m�F������A����WAVE���Ăяo���悤�ɍ쐬����
--------------------------------------------------------------------------------
�菇    Game::Update�֐����ŁA�G�̑S�ł����m������WAVE��
		�i�s�����悤�ɕύX����
		���̍ہATODO_07�ōs���Ă����u�L�[��������WAVE�i�s�v��
		�폜���Ă�����
�m�F	�G��S�ł����閈�Ɏ���WAVE���i�s����A�ŏIWAVE��S�ł����Ă�
		���ɖ�肪�������OK
*******************************************************************************/


void	Game::Initialize(ID3D11Device* device)
{
	srand((unsigned)time(NULL));

	//	���e�ϊ��s��
	projection = camera.GetProjectionMatrix();

	//	����(���s��)
	light_direction = DirectX::XMFLOAT4(0, -1, 1, 0);		//	��+�� ���� ��+�O�ւ̃��C�g


	//	�u�n�ʁv��������
	field.Initialize();
	field.SetPrimitive( new GeometricRect(pFramework.getDevice().Get()));
	field.color = DirectX::XMFLOAT4(0.4f, 0.2f, 0.6f, 1.0f);
	field.scale = DirectX::XMFLOAT3(40.0f, 20.0f, 40.0f);


	//	�u�v���C���[�v��������
	player.Initialize( device,"./Data/tank.fbx" );
	player_after_image.Initialize(device,"./Data/tank.fbx");

	//	�G�𐶐�
	enemyManager.Initialize();
	const char*			tank  = "./Data/tank.fbx";
	float				angle = DirectX::XMConvertToRadians(180.0f);
	DirectX::XMFLOAT4	color = DirectX::XMFLOAT4(1.0f, .0f, .0f, 1.0f);
	//enemyManager.Set(tank, DirectX::XMFLOAT3(-5.0f, .0f, +5.0f), angle, color);
	//enemyManager.Set(tank, DirectX::XMFLOAT3(  .0f, .0f, +5.0f), angle, color);
	//enemyManager.Set(tank, DirectX::XMFLOAT3(+5.0f, .0f, +5.0f), angle, color);

	//�@�|�[�^����ݒu
	portal.Initialize();
	portal.SetPrimitive(new GeometricSphere(pFramework.device.Get()));
	portal.color = DirectX::XMFLOAT4(0.8f, 0.2f, 0.2f, 1.0f);
	portal.scale = DirectX::XMFLOAT3(2.0f, 2.0f, 2.0f);
	portal.pos = DirectX::XMFLOAT3(.0f, .0f, +15.0f);

	//	�e�ۊǗ���������
	shotManager.Initialize();

	pWaveManager->init(modelNames01, waveData01);


}



void	Game::Release()
{
	field.Release();			//	�u�n�ʁv�̉������
	portal.Release();			//	�u�|�[�^���v�̉������
	player.Release();			//	�u�v���C���[�v�̉������
	enemyManager.Release();		//	�G�Ǘ��v�̉������
	shotManager.Release();		//	�e�ۊǗ������
}




bool	Game::Update()
{

	player.Move();					//	�u�v���C���[�v�̈ړ�����
	enemyManager.Update();			//	�u�G�Ǘ��v�̍X�V����
	camera.Update();
	shotManager.Update();			//	�e�ۊǗ����X�V
	player_after_image.Move();

	pWaveManager->create();

	//if (GetAsyncKeyState(' ') & 1)
	//{
	//	pWaveManager->WaveProceed();
	//}

	if (enemyManager.AP())
	{
		pWaveManager->WaveProceed();
	}

	/*******************************************************************************
		�p�[�e�B�N���Ǘ����X�V
	*******************************************************************************/
	pParticleManager->Update();


	//	�G�ƒe�ۂƂ̏Փ˔���
	for (int sn = 0; sn < ShotManager::MAX; sn++)
	{
		Shot* s = shotManager.Get(sn);
		if (!s || !s->exist)	continue;

		for (int en = 0; en < EnemyManager::MAX; en++)
		{
			Enemy* e = enemyManager.Get(en);
			if (!e || !e->exist)	continue;

			if (Collision::HitSphere(s->pos, 0.2f, e->pos, e->skinned_obj.scale.x))
			{
				s->obj.Release();
				s->exist = false;
				e->skinned_obj.Release();
				e->exist = false;
				
				/*******************************************************************************
					�p�[�e�B�N����p�����j�󉉏o�̍쐬
				*******************************************************************************/
				DirectX::XMFLOAT3	pos = s->pos;
				pos.x += e->pos.x;		pos.x /= 2.0f;
				pos.y += e->pos.y;		pos.y /= 2.0f;
				pos.z += e->pos.z;		pos.z /= 2.0f;
				for (int n = 0; n < 10; n++)
				{
					DirectX::XMFLOAT3	vec, power;
					const float scale = 0.15f;
					const DirectX::XMFLOAT4 color(0.8f, 0.4f, 0.2f, 0.5f);

					vec.x = ((rand() % 2001) - 1000) * 0.001f * 0.03f;
					vec.y = ((rand() % 2001) - 1000) * 0.001f * 0.03f;
					vec.z = ((rand() % 2001) - 1000) * 0.001f * 0.03f;

				
					power.x = 0.0f;
					power.z = 0.0f;
					power.y = -0.001f;

					pParticleManager->Set(pos, vec, power, scale, color, 30);
				}
			}
		}
	}
	if (Collision::HitSphere(portal.pos, 1.0f, player.pos, player.skinned_obj.scale.x))
	{
		player.angle++;
	}

	return	true;
}


void	Game::Render()
{
	blender::Set(blender::ALPHA);

	//	�r���[�ϊ��s��
	view = camera.GetViewMatrix();

	field.Render(view, projection, light_direction);			//	�u�n�ʁv�̕`�揈��
	portal.Render(view, projection, light_direction);			//	�u�|�[�^���v�̕`�揈��
	player.Render(view, projection, light_direction);			//	�u�v���C���[�v�̕`�揈��
	enemyManager.Render(view, projection, light_direction);		//	�u�G�Ǘ��v�̕`�揈��
	shotManager.Render(view, projection, light_direction);		//	�e�ۊǗ���`��

	player_after_image.Render(view, projection, light_direction);

	/*******************************************************************************
		�p�[�e�B�N���Ǘ���`��
	*******************************************************************************/
	pParticleManager->Render(view, projection);
}