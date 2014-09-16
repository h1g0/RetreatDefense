#include <DxLib.h>
#include "Fps.h"
#include "GameManager.h"
#include<fstream>
#include <sstream>
#include <string>

GameManager gameManager;
int GameMain(){
	//Esc�L�[�ŏI��
	if(( GetJoypadInputState( DX_INPUT_KEY_PAD1 ) & PAD_INPUT_START ) != 0 ){return 1;}
	//===================�����ɃQ�[���{�̂̏���======================================
	gameManager.Move();
	gameManager.Draw();
	//==============================================================================
	return 0;
}

DATEDATA Date;
FpsManager gFPS;

int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow ){
	ChangeWindowMode(TRUE);
	gFPS.SetDefaultFPS(60);
	if( DxLib_Init() == -1 ){
		 return -1;	// �G���[���N�����璼���ɏI��
	}
	ChangeFontType(DX_FONTTYPE_ANTIALIASING_EDGE);
	SetMouseDispFlag(TRUE);
	gameManager.SetStage(gameManager.Load);
	for(;;){ //���C�����[�v
		ClearDrawScreen();
		if(ProcessMessage()==-1 || GameMain()==1){break;} //�Q�[���{�̂����s
		//FPS�`��
		int NowFPS = gFPS.Get();
		int Col = (int)(255 * NowFPS / gFPS.GetDefaultFPS());
		//DrawFormatString(500,450,GetColor(255,Col,Col),"FPS: %d",NowFPS);
		ScreenFlip();
		if((GetJoypadInputState( DX_INPUT_KEY_PAD1 ) & PAD_INPUT_10) !=0){
			GetDateTime( &Date );
			std::stringstream fname;
			fname <<"img" << Date.Year << Date.Mon << Date.Day <<Date.Hour <<Date.Min <<GetNowCount() << ".png";
			SaveDrawScreenToPNG( 0 , 0 , 480 , 480 ,fname.str().c_str()) ;
		}
		gFPS.Fix();
	}

	DxLib_End() ;		// �c�w���C�u�����g�p�̏I������

	return 0 ;		// �\�t�g�̏I��
}