#include	"enemy.h"

#include	"framework.h"
#include	"player.h"
#include    "shot.h"
#include    "input.h"


extern	Player player;

/*******************************************************************************
    「敵」クラスのメンバ関数
*******************************************************************************/
void	Enemy::Initialize(ID3D11Device* device, const char* fbx_filename)
{
    skinned_obj.Initialize();
    skinned_obj.Load(device, fbx_filename);

    state = INITIALIZE;
    timer = 0;
}

void	Enemy::Release()
{
    skinned_obj.Release();
}

void	Enemy::Move(ID3D11Device* device)
{
    //	行動を確率で分岐
    const float dangle = DirectX::XMConvertToRadians(1.0f);
    const float speed = 0.02f;

    if (pInputManager->inputKeyTrigger(DIK_L))
    {

    }

    const float	SHOT_SPEED = 0.3f;
    const float OFS_FRONT = 0.45f;
    const float OFS_HEIGHT = 0.55f;



    DirectX::XMFLOAT3 p = pos;

    switch (state)
    {
    case INITIALIZE:		//	初期化
        state++;
        //		break;
    case SELECT:
    {
        //	こういう組み方もある
        //	以下の配列などはswitchの外で宣言しても良いが、
        //	ここに配置する場合は"{}"を使う事でブロック変数として
        //	用意する必要がある
        int	rateSelect[] = { 10, 10, 10, 10, 50, 10, -1 };
        int rate, rateSum = 0;
        int n = 0;

        rate = rand() % 100;
        while (rateSelect[n] != -1)
        {
            rateSum += rateSelect[n];
            if (rate < rateSum)
            {
                state = WAIT_INIT + n * 2;			//	1行で実装してみる
                break;
            }
            n++;
        }
    }
    break;
    case WAIT_INIT:			//	待機_初期化
        timer = 2 * FRAME_RATE;
        state++;
        //break;
    case WAIT_PROC:			//	待機_処理(2秒後次のモードへ)
        timer--;
        if (timer < 0)		state = SELECT;	//	行動選択へ
        break;

    case STRAIGHT_INIT:		//	直進_初期化
        timer = 3 * FRAME_RATE;
        state++;
        //break;
    case STRAIGHT_PROC:		//	直進_処理(向いている方向へ前進)
        pos.x += sinf(angle) * speed;
        pos.z += cosf(angle) * speed;

        timer--;
        if (timer < 0)		state = SELECT;	//	行動選択へ
        break;

    case ROTATE_INIT:		//	旋回_初期化
        timer = 5 * FRAME_RATE;
        state++;
        //break;
    case ROTATE_PROC:		//	旋回_処理(右回りに旋回)
        angle += dangle;					//	回転

        pos.x += sinf(angle) * speed;		//	直進
        pos.z += cosf(angle) * speed;		//

        timer--;
        if (timer < 0)		state = SELECT;	//	行動選択へ
        break;
    case TARGET_INIT:		//	プレイヤーへ直進_初期化
        timer = 3 * FRAME_RATE;
        angle = atan2f(player.pos.x - pos.x, player.pos.z - pos.z);					//	回転
        state++;
        //break;
    case TARGET_PROC:		//	プレイヤーへ直進_処理(右回りに旋回)

        pos.x += sinf(angle) * speed;		//	直進
        pos.z += cosf(angle) * speed;		//

        timer--;
        if (timer < 0)		state = SELECT;	//	行動選択へ
        break;
    case SHOT_NORMAL_INIT:		//	ショット_初期化
        timer = 3 * FRAME_RATE;
        state++;
        //break;
    case SHOT_NORMAL_PROC:		//	ショット_処理
        angle = atan2f(player.pos.x - pos.x, player.pos.z - pos.z);					//	回転

        p.x += sinf(angle) * OFS_FRONT;
        p.z += cosf(angle) * OFS_FRONT;
        p.y += OFS_HEIGHT + 0.5;

        if (timer % 20 == 0)
            shotManager.Set(device, p, angle + (rand() % 12 - 6) * DirectX::XM_PI / 180, SHOT_SPEED, 0.2f);

        timer--;
        if (timer < 0)		state = SELECT;	//	行動選択へ
        break;

    case SHOT_FAN_INIT:
        timer = 5 * FRAME_RATE;
        angle = atan2f(player.pos.x - pos.x, player.pos.z - pos.z);					//	回転
        this->tank_rotate = 0.5;
        state++;

    case SHOT_FAN_PROC:
        p = pos;

        p.x += sinf(angle) * OFS_FRONT;
        p.z += cosf(angle) * OFS_FRONT;
        p.y += OFS_HEIGHT + 0.5;

        if (timer % 20 == 0)
            shotManager.Set(device, p, angle, SHOT_SPEED, 0.2f);

        timer--;
        //angle++;  // Toooooooooorrrnado!!!
        angle += this->tank_rotate * DirectX::XM_PI / 180;
        //angle += TANK_ROTATE * 180 / DirectX::XM_PI; // Tornado shot

        switch (timer)
        {
        case 30:
            tank_rotate *= -1;
            break;
        case 90:
            tank_rotate *= -1;
            break;
        case 150:
            tank_rotate *= -1;
            break;
        case 210:
            tank_rotate *= -1;
            break;
        case 270:
            tank_rotate *= -1;
            break;
        }
        if (timer < 0)		state = SELECT;	//	行動選択へ

        break;
    }

}

void	Enemy::Render(ID3D11DeviceContext* context, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir)
{
    skinned_obj.pos = this->pos;
    skinned_obj.angle.y = this->angle;
    skinned_obj.color = this->color;

    skinned_obj.Render(context, view, projection, light_dir);
}



/*******************************************************************************
    「敵管理」クラスのメンバ関数
*******************************************************************************/
void	EnemyManager::Initialize()
{
    for (auto& d : data)	d.exist = false;
}

void	EnemyManager::Release()
{
    for (auto& d : data)
    {
        if (d.exist)
            d.Release();
    }
}

void	EnemyManager::Update(ID3D11Device* device)
{
    for (auto& d : data)
    {
        if (d.exist)
        {
            d.Move(device);

        }
    }
}

void	EnemyManager::Render(ID3D11DeviceContext* context, const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir)
{
    for (auto& d : data)
    {
        if (d.exist)	d.Render(context, view, projection, light_dir);
    }
}

Enemy* EnemyManager::Set(ID3D11Device* device, const char* filename, DirectX::XMFLOAT3 pos, float angle, DirectX::XMFLOAT4 color)
{
    for (auto& d : data)
    {
        if (d.exist)	continue;

        d.Initialize(device, filename);
        d.pos = pos;
        d.angle = angle;
        d.color = color;
        d.exist = true;
        return	&d;
    }

    return	nullptr;
}

Enemy* EnemyManager::Set(MyMesh& mesh, DirectX::XMFLOAT3 pos, float angle, DirectX::XMFLOAT4 color)
{
    for (auto& d : data)
    {
        if (d.exist)	continue;

        //d.Initialize(filename);
        d.skinned_obj.Initialize();
        d.skinned_obj.SetMesh(mesh);
        d.pos = pos;
        d.angle = angle;
        d.color = color;
        d.exist = true;
        return	&d;
    }

    return	nullptr;
}


bool EnemyManager::AP()
{
    for (int i = 0; i < MAX; i++)
    {
        if (data[i].exist)
        {
            return 0;
        }
    }
    return 1;
}