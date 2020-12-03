#pragma once

#include	<DirectXMath.h>

#include	"my_mesh.h"

/*******************************************************************************
TODO:08 敵の生存数の取得
説明    WAVE管理をする上で絶対に必要になる情報として、
		敵の生存数を取得する方法を作成する
--------------------------------------------------------------------------------
手順    敵管理クラスから「現在生存(活動)している敵の数」を
		知る方法を作成せよ
		今回、具体的な手段は指示しないが、可能な限り
		低負担/高速になるように意識する事
*******************************************************************************/

/*******************************************************************************
	「敵」クラス
*******************************************************************************/
class Enemy
{
private:
	//	敵行動管理番号の宣言
	enum	ENEMY_ACT
	{
		INITIALIZE	= 0,		//	「初期化」
		SELECT,					//	「行動選択」
		WAIT_INIT,				//	「待機_初期化」
		WAIT_PROC,				//	「待機_処理」
		STRAIGHT_INIT,			//	「直進_初期化」
		STRAIGHT_PROC,			//	「直進_処理」
		ROTATE_INIT,			//	「旋回_初期化」
		ROTATE_PROC,			//	「旋回_処理」
		TARGET_INIT,			//	「プレイヤーへ直進_初期化」
		TARGET_PROC,			//	「プレイヤーへ直進_処理」
		SHOT_NORMAL_INIT,
		SHOT_NORMAL_PROC,
		SHOT_FAN_INIT,
		SHOT_FAN_PROC
	};

public:
	MyMesh				skinned_obj;		//	「モデルオブジェクト」
	DirectX::XMFLOAT3	pos;		//	「位置(座標)」
	float				angle;		//	「回転角度」

	DirectX::XMFLOAT4	color;		//	「色」
	bool				exist;		//	「存在フラグ」


	//	敵」クラスに情報を追加
	int					state;		//	「状態」
	int					timer;		//	「タイマー」

	float               tank_rotate;

	void	Initialize(ID3D11Device* device, const char* fbx_filename);																							//	初期化関数

	void	Release();																											//	解放関数
	void	Move(ID3D11Device* device);																												//	移動関数
	void	Render(ID3D11DeviceContext* context,const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir );	//	描画関数
};


/*******************************************************************************
	「敵管理」クラス
*******************************************************************************/
class EnemyManager
{
public:
	static const int	MAX = 32;				//	「最大数」	

private:
	Enemy				data[MAX];				//	「敵データ」


public:
	void	Initialize();																										//	初期化関数
	void	Release();																											//	解放関数
	void	Update(ID3D11Device* device);																											//	更新関数
	void	Render(ID3D11DeviceContext* context,const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& projection, const DirectX::XMFLOAT4& light_dir );	//	描画関数

	bool    AP();

	Enemy*	Set(ID3D11Device* device,const char* filename, DirectX::XMFLOAT3 pos, float angle, DirectX::XMFLOAT4 color);								//	敵設定関数
	Enemy* EnemyManager::Set(MyMesh& mesh, DirectX::XMFLOAT3 pos, float angle, DirectX::XMFLOAT4 color);

	Enemy*	Get(int no){
		if ((unsigned int)no >= MAX) return	nullptr;
		return		&data[no];
	}


};

extern	EnemyManager	enemyManager;		//	こっそり追加
