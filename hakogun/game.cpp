#include	"game.h"
#include	"framework.h"

#include	"my_mesh.h"
#include	"player.h"
#include	"enemy.h"
#include	"camera.h"
#include	"shot.h"
#include	"particle.h"
#include	"wave.h"
#include	"stage.h"

#include	"collision.h"
#include    "input.h"

#include	<time.h>
#include    <Windows.h>
#include    <directxmath.h>

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


void	Game::Initialize()
{
    srand((unsigned)time(NULL));

    //	���e�ϊ��s��
    projection = camera.GetProjectionMatrix();

    //	����(���s��)
    light_direction = DirectX::XMFLOAT4(0, -1, 1, 0);		//	��+�� ���� ��+�O�ւ̃��C�g


    //	�u�n�ʁv��������
    field.Initialize();
    field.SetPrimitive(new GeometricRect(pFramework.getDevice()));
    field.color = DirectX::XMFLOAT4(0.4f, 0.2f, 0.6f, 1.0f);
    field.scale = DirectX::XMFLOAT3(40.0f, 20.0f, 40.0f);


    //	�u�v���C���[�v��������
    player.Initialize("./Data/Fbx/ziki.fbx");
    player_after_image.Initialize("./Data/Fbx/ziki.fbx");

    //	�G�𐶐�
    enemyManager.Initialize();
    //const char*			tank  = "./Data/tank.fbx";
    //float				angle = DirectX::XMConvertToRadians(180.0f);
    //DirectX::XMFLOAT4	color = DirectX::XMFLOAT4(1.0f, .0f, .0f, 1.0f);
    //enemyManager.Set(tank, DirectX::XMFLOAT3(-5.0f, .0f, +5.0f), angle, color);
    //enemyManager.Set(tank, DirectX::XMFLOAT3(  .0f, .0f, +5.0f), angle, color);
    //enemyManager.Set(tank, DirectX::XMFLOAT3(+5.0f, .0f, +5.0f), angle, color);

    //�@�|�[�^����ݒu
    portal.Initialize();
    portal.SetPrimitive(new GeometricSphere(pFramework.getDevice()));
    portal.color = DirectX::XMFLOAT4(0.8f, 0.2f, 0.2f, 1.0f);
    portal.scale = DirectX::XMFLOAT3(2.0f, 2.0f, 2.0f);
    portal.pos = DirectX::XMFLOAT3(.0f, .0f, +15.0f);

    warpFlg = false;

    //	�e�ۊǗ���������
    shotManager.Initialize();

    pWaveManager->init(modelNames01, waveData01);

    pStage.Initialize("./Data/Fbx/stage.fbx");
    pStage.pos.y -= 1.0f;
    //pStage.angle = 0.0f;
    pStage.obj.scale= DirectX::XMFLOAT3(0.01f, 0.01f, 0.01f);
    pStage.obj.color = DirectX::XMFLOAT4(0.4f, 0.4f, 0.4f, 1);

    for (int i = 0; i < 8; i++)
    {
        Explanation[i] = false;
    }
}



void	Game::Release()
{
    field.Release();			//	�u�n�ʁv�̉������
    portal.Release();			//	�u�|�[�^���v�̉������
    player.Release();			//	�u�v���C���[�v�̉������
    enemyManager.Release();		//	�G�Ǘ��v�̉������
    shotManager.Release();		//	�e�ۊǗ������
    pStage.Release();
    for (int i = 0; i < 8; i++)
    {
        Explanation[i] = false;
    }
}




bool	Game::Update()
{

    player.Move();					//	�u�v���C���[�v�̈ړ�����
    enemyManager.Update();			//	�u�G�Ǘ��v�̍X�V����
    camera.Update();
    shotManager.Update();			//	�e�ۊǗ����X�V
    player_after_image.Move();

    pWaveManager->create();
    pStage.Update();

    //if (GetAsyncKeyState(' ') & 1)
    //{
    //	pWaveManager->WaveProceed();
    //}

    if (enemyManager.AP())
    {
        warpFlg = true;
        //pWaveManager->WaveProceed();
    }

    /*******************************************************************************
        �p�[�e�B�N���Ǘ����X�V
    *******************************************************************************/
    pParticleManager->Update();


    //	�v���C���[�ƒe�ۂƂ̏Փ˔���
    for (int sn = 0; sn < ShotManager::MAX; sn++)
    {
        Shot* s = shotManager.Get(sn);
        if (!s || !s->exist)	continue;

        if (!player.exist)	continue;

        if (s->obj.scale.x != 0.15f)
        {
            if (!player.acceleFlg)
            {
                if (Collision::HitSphere(s->pos, 0.2f, player.pos, player.obj.scale.x))
                {
                    s->obj.Release();
                    s->exist = false;
                    player.hp--;
                    if (player.hp < 0)
                    {
                        player.obj.Release();
                        player.exist = false;
                    }


                    /*******************************************************************************
                        �p�[�e�B�N����p�����j�󉉏o�̍쐬
                    *******************************************************************************/
                    DirectX::XMFLOAT3	pos = s->pos;
                    pos.x += player.pos.x;		pos.x /= 2.0f;
                    pos.y += player.pos.y;		pos.y /= 2.0f;
                    pos.z += player.pos.z;		pos.z /= 2.0f;
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
    }

    //	�G�ƒe�ۂƂ̏Փ˔���
    for (int sn = 0; sn < ShotManager::MAX; sn++)
    {
        Shot* s = shotManager.Get(sn);
        if (!s || !s->exist)	continue;

        for (int en = 0; en < EnemyManager::MAX; en++)
        {
            Enemy* e = enemyManager.Get(en);
            if (!e || !e->exist)	continue;

            if (s->obj.scale.x != 0.3f)
            {
                if (Collision::HitSphere(s->pos, 0.2f, e->pos, e->obj.scale.x))
                {
                    s->obj.Release();
                    s->exist = false;
                    e->hp--;
                    if (e->hp < 0)
                    {
                        e->obj.Release();
                        e->exist = false;
                    }


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
    }
    if (warpFlg)
    {
        if (Collision::HitSphere(portal.pos, 1.0f, player.pos, player.obj.scale.x))
        {
            player.resetFlg = true;
        }
        if (player.resetFlg)
        {
            player.teleport_time += 1;

            if (player.teleport_time > 60)
            {
                player.pos = { .0f, .0f, -15.0f };
                player.obj.scale.x = 1;
                player.obj.scale.y = 1;
                player.obj.scale.z = 1;
                //teleport_time = 0;
                //turret_pos = { 0, 0, 500 };
                player.resetFlg = false;
                player.teleport_time = 0;

                player.turret_pos = { 0, 0, 500 };
                pWaveManager->WaveProceed();
                enemyManager.select_action_count += 1;
                warpFlg = false;
            }
        }
    }

    if (enemyManager.select_action_count == 0)
    {
        if (pInputManager->inputKeyTrigger(DIK_Z)) Explanation[0] = true;

        if (Explanation[0] && pInputManager->inputKeyTrigger(DIK_DOWN))  Explanation[1] = true;
        if (Explanation[0] && pInputManager->inputKeyTrigger(DIK_UP))    Explanation[2] = true;
        if (Explanation[0] && pInputManager->inputKeyTrigger(DIK_LEFT))  Explanation[3] = true;
        if (Explanation[0] && pInputManager->inputKeyTrigger(DIK_RIGHT)) Explanation[4] = true;
        if (Explanation[1] && Explanation[2] && Explanation[3] && Explanation[4]) Explanation[5] = true;

        if (Explanation[5] && pInputManager->inputKeyTrigger(DIK_Z)) Explanation[6] = true;
        if (Explanation[6] && pInputManager->inputKeyTrigger(DIK_SPACE)) Explanation[7] = true;
    }
    return	true;
}


void	Game::Render()
{
    blender::Set(blender::ALPHA);

    //	�r���[�ϊ��s��
    view = camera.GetViewMatrix();

    pStage.Render(view, projection, light_direction);
    //field.Render(view, projection, light_direction);			//	�u�n�ʁv�̕`�揈��
    player.Render(view, projection, light_direction);			//	�u�v���C���[�v�̕`�揈��
    player_after_image.Render(view, projection, light_direction);
    enemyManager.Render(view, projection, light_direction);		//	�u�G�Ǘ��v�̕`�揈��
    shotManager.Render(view, projection, light_direction);		//	�e�ۊǗ���`��

    if (warpFlg)
    {
        portal.Render(view, projection, light_direction);			//	�u�|�[�^���v�̕`�揈��
    }

    /*******************************************************************************
        �p�[�e�B�N���Ǘ���`��
    *******************************************************************************/
    pParticleManager->Render(view, projection);

    if (enemyManager.select_action_count == 0)
    {
        if (!Explanation[0])
            pFramework.sprites[7]->Render(pFramework.getContext(), 810, 120, 300, 300, 0, 0, 300, 300);
        if (Explanation[0] && !Explanation[5])
            pFramework.sprites[6]->Render(pFramework.getContext(), 810, 120, 300, 300, 0, 0, 300, 300);
        if (Explanation[5] && !Explanation[6])
            pFramework.sprites[7]->Render(pFramework.getContext(), 810, 120, 300, 300, 0, 0, 300, 300);
        if (Explanation[6] && !Explanation[7])
            pFramework.sprites[8]->Render(pFramework.getContext(), 810, 120, 300, 300, 0, 0, 300, 300);
    }
}