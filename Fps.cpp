//FPS制御
#include "Fps.h"

//=================================================================================================
//コンストラクタ
//=================================================================================================
FpsManager::FpsManager(bool FixFps, bool ShowFps, int FixedFPS){
	IsFixFPS = FixFps;
	IsShowFPS = ShowFps;
	DefaultFPS = FixedFPS;
}

//=================================================================================================
//デストラクタ
//=================================================================================================
FpsManager::~FpsManager(){}

//=================================================================================================
//FPSを強制的に固定する
//=================================================================================================
void FpsManager::Fix(){
	//フラグが立っていなければ抜ける
	if(IsFixFPS == false){return;}

	int nowtime = GetNowCount(); //今回の時間
	static int oldtime = 0; //前回の時間
	static int frametime = 1000 / DefaultFPS;	//規定時間

	//１フレームの時間と現在の経過時間を比較
	int diftime = nowtime - oldtime;	//差分

	for(;;){
		
		if(diftime >= frametime){ //規定時間を超過？
			oldtime = nowtime;	//次の計測のため今回の時間を保存
			return;
		}else{ //待機
			WaitTimer(1);
			nowtime = GetNowCount();
			diftime = nowtime - oldtime;
		}
	}
}

/*===============================================================
FPSを取得する
=================================================================*/
int FpsManager::Get(){
	//FPSを表示しない？
	if(IsShowFPS == false){return -1;}

	static int oldtime = GetNowCount(); //前回の時間
	static int counter = 0;  //カウンタ
	static int fps = 0;
	int nowtime = GetNowCount();
	counter++;
	//1000ms過ぎたか？
	if((nowtime - oldtime) >= 1000){
		int differ = ((nowtime - oldtime) - 1000); //誤差差分
		//前回の時間を更新
		oldtime = nowtime - differ;	//誤差修正
		//FPSを保存
		fps = counter;	
		//カウンタを初期化
		counter = 0;
	}
	return fps;
}