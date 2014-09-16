//FPS制御
#ifndef FPS_H
#define FPS_H

/*DxLibを使わず、他のライブラリを使用する場合は
#define FPS_USE_DXLIB
をコメントアウトし、
以下の関数を定義(もしくは書き換え)して下さい。
inline int GetNowCount(); //ミリ秒単位の精度を持つカウンタの現在値を得る
inline int	WaitTimer( int WaitTime ); //指定の時間だけ処理をとめる
*/
#define FPS_USE_DXLIB

#ifdef FPS_USE_DXLIB
	#include "DxLib.h"
#else
	#include "hoge.h"
	inline int GetNowCount(); //ミリ秒単位の精度を持つカウンタの現在値を得る
	inline int	WaitTimer( int WaitTime ) ; //指定の時間だけ処理をとめる
#endif

/*
使い方: 
メインループ開始前に宣言する(まぁ当たり前だね)
↓
FPSを描画するタイミングでGet()を呼ぶ(メインループのなるべく最後の方が好ましい)
↓
Fix()で固定する
*/

//FPS設定
class FpsManager{
	bool IsFixFPS; //FPSを固定するか？
	bool IsShowFPS; //FPSを表示するか？
	int DefaultFPS; //デフォルトのFPS
public:
	/*コンストラクタ
	引数: IsFixFps: FPSを固定するか？ IsShowFps: FPSを表示するか？ FixedFPS: FPSを幾つに固定するか？
	*/
	FpsManager(bool FixFps = true, bool ShowFps = true, int FixedFPS = 60);
	//デストラクタ
	~FpsManager();

	//FPSを固定する
	void Fix();
	//FPSを返す(返り値: FPS)
	int Get();

	//セッタ
	void SetFixFPS(bool FixFps){IsFixFPS = FixFps;}
	void SetShowFPS(bool ShowFps){IsShowFPS = ShowFps;}
	void SetDefaultFPS(int Default){DefaultFPS = Default;}

	//ゲッタ
	bool GetFixFPS(){return IsFixFPS;}
	bool GetShowFPS(){return IsShowFPS;}
	int GetDefaultFPS(){return DefaultFPS;}

};



#endif