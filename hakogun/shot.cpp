#include	"shot.h"

#include	"framework.h"
#include	"player.h"
#include    "enemy.h"

extern	Player player;


/*******************************************************************************
	弾丸クラスの移動関数
*******************************************************************************/
void	Shot::Move()
{
	
	//	最大距離の2乗を事前に計算
	const float	 DIST_SQUARE = MAX_DISTANCE * MAX_DISTANCE;

	//	移動
	pos.x += speed.x;
	pos.y += speed.y;
	pos.z += speed.z;

	// 弾の存在管理
	{
		//	プレイヤーとの距離算出
		DirectX::XMFLOAT3 vec;
		vec.x = pos.x - player.pos.x;
		vec.y = pos.y - player.pos.y;
		vec.z = pos.z - player.pos.z;
		float	d_sq = vec.x * vec.x + vec.y * vec.y + vec.z * vec.z;

		//	距離の比較
		if (d_sq < DIST_SQUARE)	return;

		//	不透明度算出
		alpha = timer / (float)FADE_TIMER;

		//	タイマー管理
		timer--;
		if (timer < 0)
		{
			//	消滅処理
			obj.Release();
			exist = false;
		}
	}

}


/*******************************************************************************
	弾丸管理クラスの初期化関数
*******************************************************************************/
void	ShotManager::Initialize()
{
	for (auto& s : data)
	{
		s.exist = false;
		s.obj.Initialize();
	}
}


/*******************************************************************************
	弾丸管理クラスの解放関数
*******************************************************************************/
void	ShotManager::Release()
{
	for (auto& s : data)
	{
		s.exist = false;
		s.obj.Release();
	}
}


/*******************************************************************************
	弾丸管理クラスの更新関数
*******************************************************************************/
void	ShotManager::Update()
{
	for (auto& s : data)
	{
		if( !s.exist )	continue;
		s.Move();
	}
}


/*******************************************************************************
	弾丸管理クラスの描画関数
*******************************************************************************/
void	ShotManager::Render(ID3D11DeviceContext* context,const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj, const DirectX::XMFLOAT4& light_dir)
{
	for (auto& s : data)
	{
		if( !s.exist )	continue;

		s.obj.pos = s.pos;
		s.obj.angle.y = s.angle;
		s.obj.color.w = s.alpha;

		s.obj.Render(context,view, proj, light_dir);
	}
}


/*******************************************************************************
	弾丸管理クラスの発射関数
*******************************************************************************/
Shot*	ShotManager::Set(ID3D11Device* device,DirectX::XMFLOAT3 pos, float angle, float speed, float scale, DirectX::XMFLOAT3 color)
{
	for (auto& s : data)
	{
		if( s.exist )	continue;

		s.obj.Release();		//	念の為解放
		s.obj.SetPrimitive(new GeometricSphere(device));
		s.obj.scale = DirectX::XMFLOAT3(scale, scale, scale);
		s.obj.color = DirectX::XMFLOAT4(color.x, color.y, color.z, 1.0f );
		s.pos = pos;
		s.angle = angle;
		s.speed = DirectX::XMFLOAT3(sinf(angle)*speed, .0f, cosf(angle)*speed);
		s.timer = Shot::FADE_TIMER;
		s.alpha = 1.0f;
		s.exist = true;

		return	&s;
	}

	return	nullptr;
}


