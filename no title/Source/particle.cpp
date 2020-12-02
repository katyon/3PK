#include	"particle.h"

#include	"framework.h"


/*******************************************************************************
	パーティクル管理クラスの更新関数
*******************************************************************************/
void	ParticleManager::Update()
{
	for (auto& pd : data)
	{
		if (!pd.exist)	continue;

		pd.speed.x	+= pd.accel.x;		pd.speed.y	+= pd.accel.y;		pd.speed.z	+= pd.accel.z;
		pd.pos.x	+= pd.speed.x;		pd.pos.y	+= pd.speed.y;		pd.pos.z	+= pd.speed.z;

		pd.dispTimer--;
		if (pd.dispTimer < 0)	pd.exist = false;
	}
}


/*******************************************************************************
	パーティクル管理クラスの描画関数
*******************************************************************************/
void	ParticleManager::Render(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection)
{
	DirectX::XMFLOAT4X4	view2;
	DirectX::XMMATRIX	inv_view2;

	//	共通の行列算出(改変view逆行列、ViewProjection行列)
	DirectX::XMStoreFloat4x4(&view2, view);						//	一旦4x4に変換
	view2._41 = 0.0f; view2._42 = 0.0f; 						//	viewの位置情報だけを削除
	view2._43 = 0.0f; view2._44 = 1.0f;							//	
	inv_view2 = DirectX::XMLoadFloat4x4(&view2);				//	Matrixへ再変換
	inv_view2 = DirectX::XMMatrixInverse(nullptr, inv_view2);	//	view行列の逆行列

	DirectX::XMMATRIX	VP;
	VP	= view * projection;									//	投影行列合成
	
	//	描画準備→描画
	DirectX::XMFLOAT4 light_dir(0, 0, 1, 0);					//	明かり情報を事実上無効化

	blender::Set(blender::ADD);

	DirectX::XMMATRIX	S, T, W;
	DirectX::XMFLOAT4X4	world, wvp;
	for ( int n = 0; n < MAX; n++ ){
		Particle&	pd	= data[n];
		if (!pd.exist)		continue;

		//	ワールド行列合成(回転行列不要)
		S = DirectX::XMMatrixScaling(pd.scale, pd.scale, pd.scale);
		T = DirectX::XMMatrixTranslation(pd.pos.x, pd.pos.y, pd.pos.z);
		W = S * T;

		//	描画用行列(4x4)生成
		DirectX::XMStoreFloat4x4(&world, W);					//	ワールド変換行列を4x4に変換
		W = inv_view2 * W * VP;									//	WorldViewProjection行列合成(view逆行列による打消し)
		DirectX::XMStoreFloat4x4(&wvp, W);						//	WorldViewProjection行列を4x4に変換
		polygon->render( FRAMEWORK.getContext(), wvp, world, light_dir, pd.color ); 		//	描画
	}

	blender::Set(blender::ALPHA);
}


/*******************************************************************************
	パーティクル管理クラスの設置関数
*******************************************************************************/
bool	ParticleManager::Set(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 speed, DirectX::XMFLOAT3 accel,
							float scale, DirectX::XMFLOAT4 color, int timer)
{
	Particle	d;
	d.pos	= pos;
	d.scale	= scale;
	d.color	= color;

	d.speed = speed;
	d.accel = accel;
	d.dispMaxTimer	= d.dispTimer	= timer;

	return	Set(d);
}
bool	ParticleManager::Set(const Particle& d)
{
	for (int n = 0; n < MAX; n++){
		Particle&	pd = data[n];
		if (pd.exist)		continue;

		pd			= d;
		pd.exist	= true;
		return	true;
	}

	return	false;
}
