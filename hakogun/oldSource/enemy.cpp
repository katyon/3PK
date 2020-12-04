#include	"enemy.h"

#include	"framework.h"
#include	"player.h"
#include    "shot.h"
#include    "input.h"


extern	Player player;

/*******************************************************************************
    �u�G�v�N���X�̃����o�֐�
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
    //	�s�����m���ŕ���
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
    case INITIALIZE:		//	������
        state++;
        //		break;
    case SELECT:
    {
        //	���������g�ݕ�������
        //	�ȉ��̔z��Ȃǂ�switch�̊O�Ő錾���Ă��ǂ����A
        //	�����ɔz�u����ꍇ��"{}"���g�����Ńu���b�N�ϐ��Ƃ���
        //	�p�ӂ���K�v������
        int	rateSelect[] = { 10, 10, 10, 10, 50, 10, -1 };
        int rate, rateSum = 0;
        int n = 0;

        rate = rand() % 100;
        while (rateSelect[n] != -1)
        {
            rateSum += rateSelect[n];
            if (rate < rateSum)
            {
                state = WAIT_INIT + n * 2;			//	1�s�Ŏ������Ă݂�
                break;
            }
            n++;
        }
    }
    break;
    case WAIT_INIT:			//	�ҋ@_������
        timer = 2 * FRAME_RATE;
        state++;
        //break;
    case WAIT_PROC:			//	�ҋ@_����(2�b�㎟�̃��[�h��)
        timer--;
        if (timer < 0)		state = SELECT;	//	�s���I����
        break;

    case STRAIGHT_INIT:		//	���i_������
        timer = 3 * FRAME_RATE;
        state++;
        //break;
    case STRAIGHT_PROC:		//	���i_����(�����Ă�������֑O�i)
        pos.x += sinf(angle) * speed;
        pos.z += cosf(angle) * speed;

        timer--;
        if (timer < 0)		state = SELECT;	//	�s���I����
        break;

    case ROTATE_INIT:		//	����_������
        timer = 5 * FRAME_RATE;
        state++;
        //break;
    case ROTATE_PROC:		//	����_����(�E���ɐ���)
        angle += dangle;					//	��]

        pos.x += sinf(angle) * speed;		//	���i
        pos.z += cosf(angle) * speed;		//

        timer--;
        if (timer < 0)		state = SELECT;	//	�s���I����
        break;
    case TARGET_INIT:		//	�v���C���[�֒��i_������
        timer = 3 * FRAME_RATE;
        angle = atan2f(player.pos.x - pos.x, player.pos.z - pos.z);					//	��]
        state++;
        //break;
    case TARGET_PROC:		//	�v���C���[�֒��i_����(�E���ɐ���)

        pos.x += sinf(angle) * speed;		//	���i
        pos.z += cosf(angle) * speed;		//

        timer--;
        if (timer < 0)		state = SELECT;	//	�s���I����
        break;
    case SHOT_NORMAL_INIT:		//	�V���b�g_������
        timer = 3 * FRAME_RATE;
        state++;
        //break;
    case SHOT_NORMAL_PROC:		//	�V���b�g_����
        angle = atan2f(player.pos.x - pos.x, player.pos.z - pos.z);					//	��]

        p.x += sinf(angle) * OFS_FRONT;
        p.z += cosf(angle) * OFS_FRONT;
        p.y += OFS_HEIGHT + 0.5;

        if (timer % 20 == 0)
            shotManager.Set(device, p, angle + (rand() % 12 - 6) * DirectX::XM_PI / 180, SHOT_SPEED, 0.2f);

        timer--;
        if (timer < 0)		state = SELECT;	//	�s���I����
        break;

    case SHOT_FAN_INIT:
        timer = 5 * FRAME_RATE;
        angle = atan2f(player.pos.x - pos.x, player.pos.z - pos.z);					//	��]
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
        if (timer < 0)		state = SELECT;	//	�s���I����

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
    �u�G�Ǘ��v�N���X�̃����o�֐�
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