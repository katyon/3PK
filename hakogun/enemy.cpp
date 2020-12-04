#include	"enemy.h"

#include	"framework.h"
#include	"player.h"
#include    "shot.h"
#include    "input.h"
#include    "wave.h"

extern	Player player;
extern  EnemyManager enemyManager;

/*******************************************************************************
	�u�G�v�N���X�̃����o�֐�
*******************************************************************************/
void	Enemy::Initialize(const char* filename)
{
	obj.Initialize();
	obj.Load(filename);
	obj.scale = { 2.0f, 2.0f, 2.0f };

	state = INITIALIZE;
	timer = 0;
}

void	Enemy::Release()
{
	obj.Release();
}

void	Enemy::Move()
{
	//	�s�����m���ŕ���
	const float dangle = DirectX::XMConvertToRadians(1.0f);
	const float speed = 0.02f;

	const float	SHOT_SPEED = 0.2f;
	const float OFS_FRONT = 1.0f;
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
		if (enemyManager.select_action_count == 1)
		{
			rateSelect[0] = 10;		rateSelect[1] = 30;		rateSelect[2] = 20;		rateSelect[3] = 20;		rateSelect[4] = 10;		rateSelect[5] = 10;
			rateSelect[6] = 0;		rateSelect[7] = 0;		rateSelect[8] = 0;		rateSelect[9] = -1;
		}
		if (enemyManager.select_action_count == 2)
		{
			//rateSelect[0] = 10;		rateSelect[1] = 15;		rateSelect[2] = 15;		rateSelect[3] = 20;		rateSelect[4] = 20;		rateSelect[5] = 20;		rateSelect[6] = -1;
			rateSelect[0] = 10;		rateSelect[1] = 20;		rateSelect[2] = 20;		rateSelect[3] = 20;		rateSelect[4] = 15;		rateSelect[5] = 15;
			rateSelect[6] = 0;		rateSelect[7] = 0;		rateSelect[8] = 0;		rateSelect[9] = -1;
		}
		if (enemyManager.select_action_count == 3)
		{
			//rateSelect[0] = 10;		rateSelect[1] = 15;		rateSelect[2] = 15;		rateSelect[3] = 20;		rateSelect[4] = 20;		rateSelect[5] = 20;		rateSelect[6] = -1;
			rateSelect[0] = 0;		rateSelect[1] = 10;		rateSelect[2] = 20;		rateSelect[3] = 20;		rateSelect[4] = 25;		rateSelect[5] = 25;
			rateSelect[6] = 0;		rateSelect[7] = 0;		rateSelect[8] = 0;		rateSelect[9] = -1;
		}
		if (enemyManager.select_action_count == 4)
		{
			rateSelect[0] = 0;		rateSelect[1] = 0;		rateSelect[2] = 0;		rateSelect[3] = 0;		rateSelect[4] = 0;		rateSelect[5] = 0;
			rateSelect[6] = 35;		rateSelect[7] = 35;		rateSelect[8] = 30;		rateSelect[9] = -1;
		}

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
		angle = atan2f(player.pos.x - pos.x, player.pos.z - pos.z);					//	��]

		pos.x += sinf(angle) * speed;
		pos.z += cosf(angle) * speed;

		timer--;
		if (timer < 0)		state = SELECT;	//	�s���I����
		break;

	case ROTATE_INIT:		//	����_������
		timer = 5 * FRAME_RATE;
		state++;
		//break;
	case ROTATE_PROC:		//	�v���C���[�̎�������
		angle = atan2f(player.pos.x - pos.x, player.pos.z - pos.z);					//	��]

		pos.x += sinf(angle + DirectX::XM_PI / 2) * speed;		//	���i
		pos.z += cosf(angle + DirectX::XM_PI / 2) * speed;		//

		timer--;
		if (timer < 0)		state = SELECT;	//	�s���I����
		break;

	case TARGET_INIT:		//	�v���C���[�֒��i_������
		timer = 3 * FRAME_RATE;
		state++;
		//break;
	case TARGET_PROC:		//	�v���C���[�֒��i_����(�E���ɐ���)
		angle = atan2f(player.pos.x - pos.x, player.pos.z - pos.z);					//	��]

		pos.x += sinf(angle + DirectX::XM_PI / 2) * speed;		//	���i
		pos.z += cosf(angle + DirectX::XM_PI / 2) * speed;		//

		timer--;
		if (timer < 0)		state = SELECT;	//	�s���I����
		break;

	case SHOT_NORMAL_INIT:		//	�V���b�g_������
		timer = 3 * FRAME_RATE;

		state++;
		//break;
	case SHOT_NORMAL_PROC:		//	�V���b�g_����

		//before_angle = angle;
		//after_angle = atan2f(player.pos.x - pos.x, player.pos.z - pos.z);					//	��]
		//slerp_angle = after_angle - before_angle;
		//if (slerp_angle > DirectX::XM_PI)
		//{
		//	slerp_angle -= 2 * DirectX::XM_PI;
		//}

		//angle += slerp_angle / 60;

		angle = atan2f(player.pos.x - pos.x, player.pos.z - pos.z);					//	��]
		pos.x += sinf(angle) * speed / 2;		//	���i
		pos.z += cosf(angle) * speed / 2;		//

		p.x += sinf(angle) * OFS_FRONT;
		p.z += cosf(angle) * OFS_FRONT;
		p.y += OFS_HEIGHT;

		if (timer % 40 == 0)
			shotManager.Set(p, angle + (rand() % 12 - 6) * DirectX::XM_PI / 180, SHOT_SPEED, 0.3f, DirectX::XMFLOAT3(1.0f, 0.3f, 0.3f));

		timer--;
		if (timer < 0)		state = WAIT_INIT;	//	�s���I����
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
		p.y += OFS_HEIGHT;

		if (timer % 20 == 0)
			shotManager.Set(p, angle, SHOT_SPEED, 0.3f, DirectX::XMFLOAT3(1.0f, 0.3f, 0.3f));

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
		if (timer < 0)		state = WAIT_INIT;	//	�s���I����
		break;

	case BOSS_NORMAL_INIT:
		timer = 5 * FRAME_RATE;
		state++;
	case BOSS_NORMAL_PROC:
		angle = atan2f(player.pos.x - pos.x, player.pos.z - pos.z);					//	��]
		pos.x += sinf(angle) * speed / 3;		//	���i
		pos.z += cosf(angle) * speed / 3;		//

		p.x += sinf(angle) * OFS_FRONT;
		p.z += cosf(angle) * OFS_FRONT;
		p.y += OFS_HEIGHT;

		if (timer % 10 == 0)
			shotManager.Set(p, angle + (rand() % 12 - 6) * DirectX::XM_PI / 180, SHOT_SPEED * 1.5, 0.3f, DirectX::XMFLOAT3(1.0f, 0.3f, 0.3f));

		timer--;
		if (timer < 0)		state = WAIT_INIT;	//	�s���I����
		break;

	case BOSS_FAN_INIT:
		timer = 7 * FRAME_RATE;
		angle = atan2f(player.pos.x - pos.x, player.pos.z - pos.z);					//	��]
		this->tank_rotate = 1.0f;
		state++;
	case BOSS_FAN_PROC:
		p = pos;

		p.x += sinf(angle) * OFS_FRONT;
		p.z += cosf(angle) * OFS_FRONT;
		p.y += OFS_HEIGHT;

		if (timer % 5 == 0)
			shotManager.Set(p, angle, SHOT_SPEED * 1.5, 0.3f, DirectX::XMFLOAT3(1.0f, 0.3f, 0.3f));

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
		case 330:
			tank_rotate *= -1;
			break;
		case 390:
			tank_rotate *= -1;
			break;
		}
		if (timer < 0)		state = WAIT_INIT;	//	�s���I����
		break;

	case BOSS_TORNADO_INIT:
		timer = 3 * FRAME_RATE;
		state++;
	case BOSS_TORNADO_PROC:
		angle += 6;
		timer--;
		if (timer % 1 == 0)
		{
			shotManager.Set(p, angle, SHOT_SPEED, 0.3f, DirectX::XMFLOAT3(1.0f, 0.3f, 0.3f));
		}
		if (timer < 0)		state = WAIT_INIT;	//	�s���I����
		break;
	}
}

void	Enemy::Render(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir)
{
	obj.pos = this->pos;
	obj.angle.y = this->angle;
	obj.color = this->color;

	obj.Render(view, projection, light_dir);
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

void	EnemyManager::Update()
{
	for (auto& d : data)
	{
		if (d.exist)
		{
			d.Move();
		}
	}
}

void	EnemyManager::Render(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir)
{
	for (auto& d : data)
	{
		if (d.exist)	d.Render(view, projection, light_dir);
	}
}

Enemy* EnemyManager::Set(const char* filename, DirectX::XMFLOAT3 pos, float angle, DirectX::XMFLOAT4 color, int hp)
{
	for (auto& d : data)
	{
		if (d.exist)	continue;

		d.Initialize(filename);
		d.pos = pos;
		d.angle = angle;
		d.color = color;
		d.hp = hp;
		d.exist = true;
		return	&d;
	}

	return	nullptr;
}

Enemy* EnemyManager::Set(MyMesh& mesh, DirectX::XMFLOAT3 pos, float angle, DirectX::XMFLOAT4 color, int hp)
{
	for (auto& d : data)
	{
		if (d.exist)	continue;

		//d.Initialize(filename);
		d.obj.Initialize();
		d.obj.SetMesh(mesh);
		d.pos = pos;
		d.angle = angle;
		d.color = color;
		d.hp = hp;
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