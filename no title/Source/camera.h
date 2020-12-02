#pragma once

#include	<DirectXMath.h>


/*******************************************************************************
	「カメラ」クラス
*******************************************************************************/
class Camera
{
public:
	/*******************************************************************************
		カメラ状態管理番号の宣言
	*******************************************************************************/
	enum MODE
	{
		WATCH,				//	「監視カメラ」
		DIRECTIONAL,		//	「相対位置固定カメラ」
		CHASE,				//	「追跡カメラ」
		TPS,				//	「3人称カメラ(TPS)」
		FPS,				//	「1人称カメラ(FPS)」
		MAX					//	「カメラ管理番号最大値」	
	};

private:
	/*******************************************************************************
		カメラ状態保存用変数の定義
	*******************************************************************************/
	int		mode;

public:
	DirectX::XMFLOAT3	pos;				//	「位置(座標)」
	DirectX::XMFLOAT3	target;				//	「注視点」
	DirectX::XMMATRIX	projection;			//	「投影行列」

	Camera();
	/*******************************************************************************
		カメラ更新関数の宣言
	*******************************************************************************/
	void				Update();					//	更新関数

private:		//	追加メンバ関数(外部から呼び出す予定が無いのでprivate化)(汚い組み方だねぇ…)
	void				Watch();		//	監視カメラ関数
	void				Directional();	//	相対位置固定カメラ関数
	void				Chase();		//	追跡カメラ関数
	void				Tps();			//	3人称カメラ(TPS)関数
	void				Fps();			//	1人称カメラ(FPS)関数
public:

	DirectX::XMMATRIX	SetOrthographicMatrix( float w, float h, float znear, float zfar );			//	平行投影行列設定関数
	DirectX::XMMATRIX	SetPerspectiveMatrix( float fov, float aspect, float znear, float zfar );	//	透視投影行列設定関数
	DirectX::XMMATRIX	GetViewMatrix();															//	ビュー行列取得関数
	DirectX::XMMATRIX	GetProjectionMatrix(){ return	projection; }								//	投影行列取得関数

};