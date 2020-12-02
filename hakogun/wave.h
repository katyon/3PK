#pragma once

#include	<directxmath.h>
#include	"my_mesh.h"


/*******************************************************************************
TODO:01 WAVEフラグ込み敵設定構造体を定義する
説明    単なる敵出現情報ではなく、WAVEの何番目に出現するかの情報を持たせる
--------------------------------------------------------------------------------
手順    以下のメンバを持つ構造体を定義せよ
		・int		番号(何WAVE目に出現するかの管理番号)
		・int		モデル番号(どのモデルを使用するかの管理番号)
		・float		座標(X,Zのみ、今回はY値を一律0として管理する予定)
		・float		回転角度
		・XMFLOAT4	色(3色+不透明度)
*******************************************************************************/
class Wave
{
public:
	int wave_num;
	int model_num;
	float pos_x;
	float pos_z;
	float angle;
	DirectX::XMFLOAT4 color = { 0.8f, 0.4f, 0.2f, 1.0f };
};

/*******************************************************************************
TODO:02 WAVE管理クラスを定義する
説明    敵出現(WAVE)管理クラスを作成する
		敵出現データだけでなく、1ステージ内で使用する敵モデル(のオリジナル)も
		全てこのクラスが管理する形で設計する
--------------------------------------------------------------------------------
手順    以下のメンバ、メソッドを持つ構造体を定義せよ
		・const int		モデル最大数(オリジナルモデルの最大数、初期値:10)
		・MyMesh		オリジナルモデル配列(要素数：モデル最大数)
		・敵設定構造体*	敵出現データ(ポインタのみ)
		・int			カウンタ(出現データの何処まで出現しているかを記録する為の変数)
		・int			現WAVE番号(何WAVEまで敵が出ているかを管理)

		・void	初期化関数(文字列配列, 敵設定構造体配列)
		・void	解放関数()
		・bool	生成関数()
		・void	WAVE進行関数()	{ 現WAVE番号を1つ増やす; }
		
	※必要に応じてシングルトン化、その他メンバの追加をしなさい
*******************************************************************************/
class WaveManager
{
public:
	static const int model_max = 10;
private:
	MyMesh myModel[model_max];
	Wave* EnemyData;
	int count;
	int ThisWave;

public:
	void init(char** filename, Wave* ed);
	void release();
	bool create();
	void WaveProceed()
	{
		if (EnemyData[count].wave_num < 0) return;
		ThisWave++;
	}

	static WaveManager* getInstance()
	{
		static WaveManager instance;
		return &instance;
	}
};

#define pWaveManager WaveManager::getInstance()



































