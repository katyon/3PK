#include	"player.h"

#include	"shot.h"
#include	"particle.h"
#include    "input.h"

extern Player player;

/*******************************************************************************
    「プレイヤー」クラスの初期化
*******************************************************************************/
void	Player::Initialize(ID3D11Device* device, const char* fbx_filename)
{
    skinned_obj.Initialize();
    skinned_obj.Load(device, fbx_filename);

    skinned_obj1 = skinned_obj2 = skinned_obj;
}



/*******************************************************************************
    「プレイヤー」クラスの解放
*******************************************************************************/
void	Player::Release()
{
    skinned_obj.Release();
}



/*******************************************************************************
    「プレイヤー」クラスの描画
*******************************************************************************/
void	Player::Render(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir)
{
    skinned_obj.pos = this->pos;
    skinned_obj.angle.y = this->angle;
    skinned_obj.Render(view, projection, light_dir);
    if (acceleFlg)
    {
        skinned_obj1.pos = previous_pos1;
        skinned_obj1.angle.y = player.angle;
        skinned_obj1.color = { 0.0, 1.0, 1.0, 0.5 };
        skinned_obj1.Render(view, projection, light_dir);

        skinned_obj2.pos = previous_pos2;
        skinned_obj2.angle.y = player.angle;
        skinned_obj2.color = { 0.0, 1.0, 1.0, 0.5 };
        skinned_obj2.Render(view, projection, light_dir);
    }
}


/*******************************************************************************
    「プレイヤー」クラスの移動
*******************************************************************************/
void	Player::Move()
{
    const float dangle = DirectX::XMConvertToRadians(2.0f);		//	1度
    const float speed = 0.20f;									//	プレイヤーの速度

    ////	回転
    //if (pInputManager->inputKeyState(DIK_LEFT))	 angle -= dangle;
    //if (pInputManager->inputKeyState(DIK_RIGHT)) angle += dangle;

    //if (pInputManager->inputKeyState(DIK_UP))
    //{
    //	float dx = sinf(angle);			//	移動ベクトル(X成分)
    //	float dz = cosf(angle);			//	移動ベクトル(Z成分)
    //	pos.x += dx * speed;
    //	pos.z += dz * speed;
    //}
    {
        if (pInputManager->inputKeyState(DIK_D))
        {
            if (acceleFlg)
            {
                if (!left_dash)
                {
                    right_dash = true;
                }
            }
            else { pos.x += speed; }
        }
        if (pInputManager->inputKeyState(DIK_A))
        {
            if (acceleFlg)
            {
                if (!right_dash)
                {
                    left_dash = true;
                }
            }
            else { pos.x -= speed; }
        }
        if (pInputManager->inputKeyState(DIK_W))
        {
            if (acceleFlg)
            {
                if (!down_dash)
                {
                    up_dash = true;
                }
            }
            else { pos.z += speed; }
        }
        if (pInputManager->inputKeyState(DIK_S))
        {
            if (acceleFlg)
            {
                if (!up_dash)
                {
                    down_dash = true;
                }
            }
            else { pos.z -= speed; }
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

    //	単発銃 Zキー
    if (pInputManager->inputKeyTrigger(DIK_Z))
    {
        const float	SHOT_SPEED = 0.5f;
        const float OFS_FRONT = 0.45f;
        const float OFS_HEIGHT = 0.55f;

        DirectX::XMFLOAT3 p = pos;
        p.x += sinf(angle) * OFS_FRONT;
        p.z += cosf(angle) * OFS_FRONT;
        p.y += OFS_HEIGHT;
        shotManager.Set(p, angle, SHOT_SPEED, 0.2f);

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

    //	散弾銃 Xキー
    if (pInputManager->inputKeyTrigger(DIK_X))
    {
        const float	SHOT_SPEED = 0.5f;
        const float OFS_FRONT = 0.45f;
        const float OFS_HEIGHT = 0.55f;

        DirectX::XMFLOAT3 p = pos;
        p.x += sinf(angle) * OFS_FRONT;
        p.z += cosf(angle) * OFS_FRONT;
        p.y += OFS_HEIGHT;

        for (int shotgun = 0; shotgun < 10; shotgun++)
        {
            float shotgun_angle = angle;
            shotgun_angle += ((rand() % 12 - 6) * DirectX::XM_PI) / 180;
            shotManager.Set(p, shotgun_angle, SHOT_SPEED - shotgun * 0.02, 0.1f);
        }

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
            static const DirectX::XMFLOAT4 COLOR(0.2f, 0.4f, 0.8f, 0.5f);

            vec.x = ((rand() % 2001) - 1000) * 0.00002f + sinf(angle) * MUZZLE_SPEED;
            vec.z = ((rand() % 2001) - 1000) * 0.00002f + cosf(angle) * MUZZLE_SPEED;
            vec.y = ((rand() % 2001) - 1000) * 0.00002f;

            power.x = 0.0f;
            power.z = 0.0f;
            power.y = -0.002f;

            pParticleManager->Set(p, vec, power, SCALE, COLOR, 5);
        }
    }

    //	連射銃 Aキー
    if (pInputManager->inputKeyState(DIK_B))
    {
        const float	SHOT_SPEED = 0.5f;
        const float OFS_FRONT = 0.45f;
        const float OFS_HEIGHT = 0.55f;

        DirectX::XMFLOAT3 p = player.previous_pos1;
        p.x += sinf(angle) * OFS_FRONT;
        p.z += cosf(angle) * OFS_FRONT;
        p.y += OFS_HEIGHT;
        shotManager.Set(p, angle, SHOT_SPEED, 0.2f);

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
            static const DirectX::XMFLOAT4 COLOR(0.4f, 0.8f, 0.2f, 0.5f);

            vec.x = ((rand() % 2001) - 1000) * 0.00002f + sinf(angle) * MUZZLE_SPEED;
            vec.z = ((rand() % 2001) - 1000) * 0.00002f + cosf(angle) * MUZZLE_SPEED;
            vec.y = ((rand() % 2001) - 1000) * 0.00002f;

            power.x = 0.0f;
            power.z = 0.0f;
            power.y = -0.002f;

            pParticleManager->Set(p, vec, power, SCALE, COLOR, 5);
        }
    }

    if (previous_pos1.x != skinned_obj.pos.x || previous_pos1.z != skinned_obj.pos.z)
    {
        previous_pos1 = skinned_obj.pos;
        if (previous_pos2.x != previous_pos1.x || previous_pos2.z != previous_pos1.z)
        {
            previous_pos2 = previous_pos1;
        }
    }

    this->pos = player.GetPosition();
}

void Player::Dash()
{
    const float speed = 0.20f;									//	プレイヤーの速度
    if (acceleFlg)
    {
        accele -= 0.1f;
        if (right_dash)
        {
            pos.x += speed * accele;
        }
        else if (left_dash)
        {
            pos.x -= speed * accele;
        }

        if (up_dash)
        {
            pos.z += speed * accele;
        }
        else if (down_dash)
        {
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