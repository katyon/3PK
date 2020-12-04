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
    グローバル変数
*******************************************************************************/
MyMesh			field;				//	地面用変数「地面」
MyMesh			portal;
Player			player;				//	プレイヤー用変数「プレイヤー」
Enemy           enemy;
Camera			camera;				//	カメラ用変数「カメラ」
EnemyManager	enemyManager;		//	敵管理用変数「敵管理」
ShotManager		shotManager;		//	弾管理用変数「弾管理」

PlayerAfterImage player_after_image;

extern char* modelNames01[];
extern Wave waveData01[];

/*******************************************************************************
TODO:07 WAVE管理クラスの呼び出し
説明    Gameクラスの各所でWAVE管理クラスのメソッドを呼び出す
--------------------------------------------------------------------------------
手順    Gameクラスの各所で以下の処理を実行せよ
        ・Game::Initialize関数でWAVE管理クラスの初期化関数を呼び出す
        　この時、引数はTODO_06で追加したデータとする
        ・Game::Initialize関数で以前からあった敵出現処理をコメント化する
        　(敵出現はWAVE管理クラスに一元化する為、不要となる)
        ・Game::Update関数でWAVE管理クラスの生成関数を呼び出す
        　この時、敵の更新よりも前に行った方が安定する
        ・Game::Update関数で(使われていない)何かのキーを押した時に
        　WAVE管理クラスのWAVE進行関数を呼び出す
確認	起動時に敵が3体出現し、設定したキーを押す度に敵が2体→3体と出現すればOK
*******************************************************************************/

/*******************************************************************************
TODO:09 WAVE全滅検知に伴う増援呼び出し
説明    敵の現WAVE全滅を確認したら、次のWAVEを呼び出すように作成する
--------------------------------------------------------------------------------
手順    Game::Update関数内で、敵の全滅を検知したらWAVEが
        進行されるように変更する
        この際、TODO_07で行っていた「キー押したらWAVE進行」を
        削除しておく事
確認	敵を全滅させる毎に次のWAVEが進行され、最終WAVEを全滅させても
        特に問題が無ければOK
*******************************************************************************/


void	Game::Initialize()
{
    srand((unsigned)time(NULL));

    //	投影変換行列
    projection = camera.GetProjectionMatrix();

    //	光源(平行光)
    light_direction = DirectX::XMFLOAT4(0, -1, 1, 0);		//	上+奥 から 下+前へのライト


    //	「地面」を初期化
    field.Initialize();
    field.SetPrimitive(new GeometricRect(pFramework.getDevice()));
    field.color = DirectX::XMFLOAT4(0.4f, 0.2f, 0.6f, 1.0f);
    field.scale = DirectX::XMFLOAT3(40.0f, 20.0f, 40.0f);


    //	「プレイヤー」を初期化
    player.Initialize("./Data/Fbx/ziki.fbx");
    player_after_image.Initialize("./Data/Fbx/ziki.fbx");

    //	敵を生成
    enemyManager.Initialize();
    //const char*			tank  = "./Data/tank.fbx";
    //float				angle = DirectX::XMConvertToRadians(180.0f);
    //DirectX::XMFLOAT4	color = DirectX::XMFLOAT4(1.0f, .0f, .0f, 1.0f);
    //enemyManager.Set(tank, DirectX::XMFLOAT3(-5.0f, .0f, +5.0f), angle, color);
    //enemyManager.Set(tank, DirectX::XMFLOAT3(  .0f, .0f, +5.0f), angle, color);
    //enemyManager.Set(tank, DirectX::XMFLOAT3(+5.0f, .0f, +5.0f), angle, color);

    //　ポータルを設置
    portal.Initialize();
    portal.SetPrimitive(new GeometricSphere(pFramework.getDevice()));
    portal.color = DirectX::XMFLOAT4(0.8f, 0.2f, 0.2f, 1.0f);
    portal.scale = DirectX::XMFLOAT3(2.0f, 2.0f, 2.0f);
    portal.pos = DirectX::XMFLOAT3(.0f, .0f, +15.0f);

    warpFlg = false;

    //	弾丸管理を初期化
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
    field.Release();			//	「地面」の解放処理
    portal.Release();			//	「ポータル」の解放処理
    player.Release();			//	「プレイヤー」の解放処理
    enemyManager.Release();		//	敵管理」の解放処理
    shotManager.Release();		//	弾丸管理を解放
    pStage.Release();
    for (int i = 0; i < 8; i++)
    {
        Explanation[i] = false;
    }
}




bool	Game::Update()
{

    player.Move();					//	「プレイヤー」の移動処理
    enemyManager.Update();			//	「敵管理」の更新処理
    camera.Update();
    shotManager.Update();			//	弾丸管理を更新
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
        パーティクル管理を更新
    *******************************************************************************/
    pParticleManager->Update();


    //	プレイヤーと弾丸との衝突判定
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
                        パーティクルを用いた破壊演出の作成
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

    //	敵と弾丸との衝突判定
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
                        パーティクルを用いた破壊演出の作成
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

    //	ビュー変換行列
    view = camera.GetViewMatrix();

    pStage.Render(view, projection, light_direction);
    //field.Render(view, projection, light_direction);			//	「地面」の描画処理
    player.Render(view, projection, light_direction);			//	「プレイヤー」の描画処理
    player_after_image.Render(view, projection, light_direction);
    enemyManager.Render(view, projection, light_direction);		//	「敵管理」の描画処理
    shotManager.Render(view, projection, light_direction);		//	弾丸管理を描画

    if (warpFlg)
    {
        portal.Render(view, projection, light_direction);			//	「ポータル」の描画処理
    }

    /*******************************************************************************
        パーティクル管理を描画
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