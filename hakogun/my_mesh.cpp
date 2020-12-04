#include	"my_mesh.h"

#include	"framework.h"



/*******************************************************************************
	メッシュ初期化
*******************************************************************************/
void	MyMesh::Initialize()
{
	//	情報初期値
	pos		= DirectX::XMFLOAT3(0, 0, 0);
	angle	= DirectX::XMFLOAT3(0, 0, 0);
	scale	= DirectX::XMFLOAT3(1, 1, 1);


	color	= DirectX::XMFLOAT4(1, 1, 1, 1);

	//	モデル読み込み
	obj = nullptr;
}

/*******************************************************************************
	メッシュモデル読み込み
		引数
			fbx_filename:メッシュファイル名(FBX形式)
*******************************************************************************/
bool	MyMesh::Load(const char* fbx_filename)
{
	//	モデル読み込み
	obj = new StaticMesh(pframework.getDevice(), fbx_filename);
	bLoad = true;

	return	true;
}

/*******************************************************************************
	メッシュモデル設定
		引数
			primitive:メッシュモデル
*******************************************************************************/
void	MyMesh::SetPrimitive(GeometricPrimitive* primitive)
{
	obj = primitive;
	bLoad = true;
}

/*******************************************************************************
	メッシュモデル設定(使い回し型)
		引数
			primitive:メッシュモデル
*******************************************************************************/
void	MyMesh::SetMesh(MyMesh& org)
{
	*this = org;
	bLoad = false;
}

/*******************************************************************************
	メッシュ解放
*******************************************************************************/
void	MyMesh::Release()
{
	if (obj && bLoad)
	{
		delete	obj;
	}
	obj = nullptr;
	bLoad = false;
}

/*******************************************************************************
	メッシュ描画
		引数
			view:ビュー変換行列
			projection:投影変換行列
			light_dir:平行光ベクトル
*******************************************************************************/
void	MyMesh::Render(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir)
{
	//	モデルが無ければ描画処理を一切行わない
	if( !obj )		return;

	//	ワールド変換行列取得
	DirectX::XMMATRIX worldM = GetWorldMatrix();

	//	Matrix -> Float4x4 変換
	DirectX::XMFLOAT4X4 world_view_projection;
	DirectX::XMFLOAT4X4 world;
	DirectX::XMStoreFloat4x4(&world_view_projection, worldM * view * projection);
	DirectX::XMStoreFloat4x4(&world, worldM);

	//	描画
	obj->render(pframework.getContext(), world_view_projection, world, light_dir, color);
}


/*******************************************************************************
	メッシュ情報からワールド変換行列取得
*******************************************************************************/
DirectX::XMMATRIX	MyMesh::GetWorldMatrix()
{
	DirectX::XMMATRIX world;
	DirectX::XMMATRIX S, R, Rx, Ry, Rz, T;

	//	初期化
	world = DirectX::XMMatrixIdentity();

	//	拡大・縮小
	S = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);

	//	回転
	Rx = DirectX::XMMatrixRotationX(angle.x);				//	X軸を基準とした回転行列
	Ry = DirectX::XMMatrixRotationY(angle.y);				//	Y軸を基準とした回転行列
	Rz = DirectX::XMMatrixRotationZ(angle.z);				//	Z軸を基準とした回転行列
	R = Rz * Ry * Rx;

	//	平行移動
	T = DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);

	//	ワールド変換行列
	world = S * R * T;


	return	world;
}
