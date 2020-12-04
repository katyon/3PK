#include	"camera.h"

#include	"framework.h"
#include	"player.h"
#include	"Util.h"

extern	Player	player;

/*******************************************************************************
	「カメラ」クラスのコンストラクタ
*******************************************************************************/
Camera::Camera()
{
	pos = DirectX::XMFLOAT3(.0f, 10.0f, -15.0f);
	target = DirectX::XMFLOAT3(.0f, .0f, .0f);

	float	fov = DirectX::XMConvertToRadians(30.0f);
	float	aspect = (float)SCREEN_WIDTH / (float)SCREEN_HEIGHT;
	SetPerspectiveMatrix(fov, aspect, 0.1f, 1000.0f);

	mode = MODE::WATCH;
}


/*******************************************************************************
	平行投影行列を算出する関数
*******************************************************************************/
DirectX::XMMATRIX	Camera::SetOrthographicMatrix(float w, float h, float znear, float zfar)
{
	projection = DirectX::XMMatrixOrthographicLH(w, h, znear, zfar);
	return	projection;
}


/*******************************************************************************
	透視投影行列を算出する関数
*******************************************************************************/
DirectX::XMMATRIX	Camera::SetPerspectiveMatrix(float fov, float aspect, float znear, float zfar)
{
	projection = DirectX::XMMatrixPerspectiveFovLH(fov, aspect, znear, zfar);
	return	projection;
}


/*******************************************************************************
	ビュー行列を取得する関数
*******************************************************************************/
DirectX::XMMATRIX	Camera::GetViewMatrix()
{
	DirectX::XMVECTOR	p = DirectX::XMVectorSet(pos.x, pos.y, pos.z, 1.0f);
	DirectX::XMVECTOR	t = DirectX::XMVectorSet(target.x, target.y, target.z, 1.0f);
	DirectX::XMVECTOR	up = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

	return	DirectX::XMMatrixLookAtLH(p, t, up);
}


/*******************************************************************************
	カメラ用更新関数の実装
*******************************************************************************/
void	Camera::Update()
{
	//	カメラの更新
	switch (mode)
	{
	case WATCH:			Watch();		break;		//	「監視カメラ」
	case DIRECTIONAL:	Directional();	break;		//	「相対位置固定カメラ」
	case CHASE:			Chase();		break;		//	「追跡カメラ」
	case TPS:			Tps();			break;		//	「3人称カメラ(TPS)」
	case FPS:			Fps();			break;		//	「1人称カメラ(FPS)」
	}

	mode = DIRECTIONAL;

	//	カメラの切り替え
	//if (GetAsyncKeyState('C') & 1)
	//{
	//	mode = (mode + 1) % MODE::MAX;		//	1行でこう書ける
	//	//	複数行ならこれが王道?
	//	//mode++;
	//	//if (mode <= MODE::MAX)	mode = WATCH;
	//}


	////	カメラモード確認用デバッグ
	//char	str[64];
	//sprintf_s(str, 64, "CameraMode:%d", mode);
	//SetText(str, 5, 5);
}


/*******************************************************************************
	「監視カメラ」の実装
*******************************************************************************/
void	Camera::Watch()
{
	pos = DirectX::XMFLOAT3(.0f, 10.0f, -15.0f);
	target = player.pos;
}
/*******************************************************************************
	「相対位置固定カメラ」の実装
*******************************************************************************/
void	Camera::Directional()
{
	pos.x = player.pos.x + 17.0f;
	pos.y = player.pos.y + 17.0f;
	pos.z = player.pos.z - 17.0f;

	target = player.pos;
}
/*******************************************************************************
	「3人称カメラ」の実装
*******************************************************************************/
void	Camera::Tps()
{
	const float	Distance = 12.0f;

	float	dx = sinf(player.angle);
	float	dz = cosf(player.angle);

	pos.x = player.pos.x - Distance * dx;
	pos.z = player.pos.z - Distance * dz;

	target = player.pos;
}
/*******************************************************************************
	「1人称カメラ」の実装
*******************************************************************************/
void	Camera::Fps()
{
	const float	Height = 0.75f;

	float	dx = sinf(player.angle);
	float	dz = cosf(player.angle);

	pos.x = player.pos.x;
	pos.y = player.pos.y + Height;
	pos.z = player.pos.z;

	target.x = pos.x + dx;
	target.y = pos.y;
	target.z = pos.z + dz;
}
/*******************************************************************************
	「追跡カメラ」の実装
*******************************************************************************/
void	Camera::Chase()
{
	const float	minDistance = 5.0f;
	const float	maxDistance = 15.0f;

	float	dx = player.pos.x - pos.x;
	float	dz = player.pos.z - pos.z;
	float	distance = sqrtf(dx * dx + dz * dz);

	if (distance < minDistance)
	{
		dx /= distance;		//	ベクトルの正規化
		dz /= distance;		//	
		pos.x = player.pos.x - minDistance * dx;
		pos.z = player.pos.z - minDistance * dz;
	}
	if (distance > maxDistance)
	{
		dx /= distance;		//	ベクトルの正規化
		dz /= distance;		//	
		pos.x = player.pos.x - maxDistance * dx;
		pos.z = player.pos.z - maxDistance * dz;
	}

	target = player.pos;
}
