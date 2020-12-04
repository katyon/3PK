#include	"player.h"

#include	"shot.h"
#include	"particle.h"
#include    "input.h"

extern Player player;

#define MOVELIMIT (20.f)

/*******************************************************************************
    「プレイヤー」クラスの初期化
*******************************************************************************/
void	Player::Initialize(const char* filename)
{
    obj.Initialize();
    obj.Load(filename);

    obj1 = obj2 = obj;

	hp = 30;
	exist = true;

    player.pos = { .0f, .0f, -15.0f };
    turret.Initialize();
    turret.Load(filename);
    turret_pos = { 0, 0, 500 };

}



/*******************************************************************************
    「プレイヤー」クラスの解放
*******************************************************************************/
void	Player::Release()
{
    obj.Release();
    turret.Release();
}



/*******************************************************************************
    「プレイヤー」クラスの描画
*******************************************************************************/
void	Player::Render(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir)
{
    if (exist)
    {
        obj.pos = this->pos;
        obj.angle.y = this->angle;
        obj.Render(view, projection, light_dir);
        if (acceleFlg)
        {
            obj1.pos = previous_pos1;
            obj1.angle.y = player.angle;
            obj1.color = { 0.0, 1.0, 1.0, 0.5 };
            obj1.Render(view, projection, light_dir);

            obj2.pos = previous_pos2;
            obj2.angle.y = player.angle;
            obj2.color = { 1.0, 0.0, 1.0, 0.3 };
            obj2.Render(view, projection, light_dir);
        }

        turret.pos = turret_pos;
        turret.angle.y = turret_angle;
        turret.color = { 0.1, 0.5, 1.0f, 0.8f };
        turret.Render(view, projection, light_dir);
    }
    if (resetFlg)
    {
        obj.scale.x -= 1.0 / 60;
        obj.scale.z -= 1.0 / 60;
        obj.scale.y += 2.0 / 60;
        if (obj.scale.x < 0 || obj.scale.z < 0)
        {
            obj.scale.x = 0;
            obj.scale.z = 0;
        }
    }
}


/*******************************************************************************
    「プレイヤー」クラスの移動
*******************************************************************************/
void	Player::Move()
{
    const float dangle = DirectX::XMConvertToRadians(2.0f);		//	1度
    const float speed = 0.20f;									//	プレイヤーの速度



#if 0
    if (pInputManager->inputKeyState(DIK_A))
    {
        angle += 1 * DirectX::XM_PI / 180;
    }
    if (pInputManager->inputKeyState(DIK_D))
    {
        angle -= 1 * DirectX::XM_PI / 180;
    }
#endif

    if (exist)
    {
        //　回避　スペースキー
        {
            if (pInputManager->inputKeyState(DIK_RIGHT))
            {
                if (acceleFlg)
                {
                    if (!left_dash)
                    {
                        right_dash = true;
                    }
                }
                else if (player.pos.x < MOVELIMIT) { pos.x += speed; }
            }
            if (pInputManager->inputKeyState(DIK_LEFT))
            {
                if (acceleFlg)
                {
                    if (!right_dash)
                    {
                        left_dash = true;
                    }
                }
                else if (player.pos.x > -(MOVELIMIT - 1.5)) { pos.x -= speed; }
            }
            if (pInputManager->inputKeyState(DIK_UP))
            {
                if (acceleFlg)
                {
                    if (!down_dash)
                    {
                        up_dash = true;
                    }
                }
                else if (player.pos.z < (MOVELIMIT - 1.5)) { pos.z += speed; }
            }
            if (pInputManager->inputKeyState(DIK_DOWN))
            {
                if (acceleFlg)
                {
                    if (!up_dash)
                    {
                        down_dash = true;
                    }
                }
                else if (player.pos.z > -MOVELIMIT) { pos.z -= speed; }
            }

            //　ダッシュ回避
            if (pInputManager->inputKeyTrigger(DIK_SPACE))
            {
                if (!acceleFlg)
                {
                    accele = 3.0f;
                    acceleFlg = true;
                }
            }
            Dash();
        }

        turret_timer++;
        //	タレットショット
        if (turret_timer % 10 == 0)
        {
            const float	SHOT_SPEED = 0.3f;
            const float OFS_FRONT = 1.0f;
            const float OFS_HEIGHT = 0.55f;

            DirectX::XMFLOAT3 p = turret_pos;
            p.x += sinf(turret_angle) * OFS_FRONT;
            p.z += cosf(turret_angle) * OFS_FRONT;
            p.y += OFS_HEIGHT;
            shotManager.Set(p, turret_angle, SHOT_SPEED, 0.15f);

            //	パーティクル管理クラスの設置関数の呼び出し(実験用)
            //pParticleManager->Set(p, 1.0f, DirectX::XMFLOAT4(0.8f, 0.4f, 0.2f, 0.6f));

            /*******************************************************************************
                パーティクルを用いた演出(砲撃の際に発生する火花っぽい物)
            *******************************************************************************/
            for (int n = 0; n < 5; n++)
            {
                DirectX::XMFLOAT3	vec, power;
                static const float	MUZZLE_SPEED = SHOT_SPEED * 0.4f;
                static const float SCALE = 0.05f;
                static const DirectX::XMFLOAT4 COLOR(0.8f, 0.4f, 0.2f, 0.5f);

                vec.x = ((rand() % 2001) - 1000) * 0.00002f + sinf(angle) * MUZZLE_SPEED;
                vec.z = ((rand() % 2001) - 1000) * 0.00002f + cosf(angle) * MUZZLE_SPEED;
                vec.y = ((rand() % 2001) - 1000) * 0.00002f;

                power.x = 0.0f;
                power.z = 0.0f;
                power.y = -0.002f;

                pParticleManager->Set(p, vec, power, SCALE, COLOR, 5);
            }
        }

        ////	散弾銃 Xキー
        //if (pInputManager->inputKeyTrigger(DIK_X))
        //{
        //	const float	SHOT_SPEED = 0.5f;
        //	const float OFS_FRONT = 1.0f;
        //	const float OFS_HEIGHT = 0.55f;

        //	DirectX::XMFLOAT3 p = pos;
        //	p.x += sinf(angle) * OFS_FRONT;
        //	p.z += cosf(angle) * OFS_FRONT;
        //	p.y += OFS_HEIGHT;

        //	for (int shotgun = 0; shotgun < 10; shotgun++)
        //	{
        //		float shotgun_angle = angle;
        //		shotgun_angle += ((rand() % 12 - 6) * DirectX::XM_PI) / 180;
        //		shotManager.Set(p, shotgun_angle, SHOT_SPEED - shotgun * 0.02, 0.1f);
        //	}

        //	//	パーティクル管理クラスの設置関数の呼び出し(実験用)
        //	//pParticleManager->Set(p, 1.0f, DirectX::XMFLOAT4(0.8f, 0.4f, 0.2f, 0.6f));

        //	/*******************************************************************************
        //		パーティクルを用いた演出(砲撃の際に発生する火花っぽい物)
        //	*******************************************************************************/
        //	for (int n = 0; n < 5; n++)
        //	{
        //		DirectX::XMFLOAT3	vec, power;
        //		static const float	MUZZLE_SPEED = SHOT_SPEED * 0.4f;
        //		static const float SCALE = 0.05f;
        //		static const DirectX::XMFLOAT4 COLOR(0.2f, 0.4f, 0.8f, 0.5f);

        //		vec.x = ((rand() % 2001) - 1000) * 0.00002f + sinf(angle) * MUZZLE_SPEED;
        //		vec.z = ((rand() % 2001) - 1000) * 0.00002f + cosf(angle) * MUZZLE_SPEED;
        //		vec.y = ((rand() % 2001) - 1000) * 0.00002f;

        //		power.x = 0.0f;
        //		power.z = 0.0f;
        //		power.y = -0.002f;

        //		pParticleManager->Set(p, vec, power, SCALE, COLOR, 5);
        //	}
        //}

        ////	連射銃 Bキー
        //if (pInputManager->inputKeyState(DIK_B))
        //{
        //	const float	SHOT_SPEED = 0.5f;
        //	const float OFS_FRONT = 1.0f;
        //	const float OFS_HEIGHT = 0.55f;

        //	DirectX::XMFLOAT3 p = player.previous_pos1;
        //	p.x += sinf(angle) * OFS_FRONT;
        //	p.z += cosf(angle) * OFS_FRONT;
        //	p.y += OFS_HEIGHT;
        //	shotManager.Set(p, angle, SHOT_SPEED, 0.2f);

        //	//	パーティクル管理クラスの設置関数の呼び出し(実験用)
        //	//pParticleManager->Set(p, 1.0f, DirectX::XMFLOAT4(0.8f, 0.4f, 0.2f, 0.6f));

        //	/*******************************************************************************
        //		パーティクルを用いた演出(砲撃の際に発生する火花っぽい物)
        //	*******************************************************************************/
        //	for (int n = 0; n < 5; n++)
        //	{
        //		DirectX::XMFLOAT3	vec, power;
        //		static const float	MUZZLE_SPEED = SHOT_SPEED * 0.4f;
        //		static const float SCALE = 0.05f;
        //		static const DirectX::XMFLOAT4 COLOR(0.4f, 0.8f, 0.2f, 0.5f);

        //		vec.x = ((rand() % 2001) - 1000) * 0.00002f + sinf(angle) * MUZZLE_SPEED;
        //		vec.z = ((rand() % 2001) - 1000) * 0.00002f + cosf(angle) * MUZZLE_SPEED;
        //		vec.y = ((rand() % 2001) - 1000) * 0.00002f;

        //		power.x = 0.0f;
        //		power.z = 0.0f;
        //		power.y = -0.002f;

        //		pParticleManager->Set(p, vec, power, SCALE, COLOR, 5);
        //	}
        //}

        if (previous_pos1.x != obj.pos.x || previous_pos1.z != obj.pos.z)
        {
            previous_pos1 = obj.pos;
            if (previous_pos2.x != previous_pos1.x || previous_pos2.z != previous_pos1.z)
            {
                previous_pos2 = previous_pos1;
            }
        }

        //　Turret Update
        if (pInputManager->inputKeyTrigger(DIK_Z))
        {
            turret_pos = player.pos;
        }
        angle = atan2f(turret_pos.x - player.pos.x, turret_pos.z - player.pos.z);					//	回転
        turret_angle = atan2f(player.pos.x - turret_pos.x, player.pos.z - turret_pos.z);					//	回転

        //this->pos = player.GetPosition();
    }
}

void Player::Dash()
{
    const float speed = 0.20f;									//	プレイヤーの速度
    if (acceleFlg)
    {
        accele -= 0.1f;
        if (right_dash)
        {
            if (player.pos.x < MOVELIMIT)
                pos.x += speed * accele;
        }
        else if (left_dash)
        {
            if (player.pos.x > -(MOVELIMIT - 1.5))
                pos.x -= speed * accele;
        }

        if (up_dash)
        {
            if (player.pos.z < (MOVELIMIT - 1.5))
                pos.z += speed * accele;
        }
        else if (down_dash)
        {
            if (player.pos.z > -MOVELIMIT)
                pos.z -= speed * accele;
        }

        if (accele < 0)
        {
            acceleFlg = false;
        }
    }
    else
    {
        right_dash = false;
        left_dash = false;
        up_dash = false;
        down_dash = false;
    }
}

DirectX::XMFLOAT3 Player::GetPosition()
{
    return this->pos;
}


void PlayerAfterImage::Move()
{

}

void PlayerAfterImage::Render(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir)
{

}