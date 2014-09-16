#pragma once
#include<string>
#include<vector>
#include<boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

#include "Unit.h"
#define mapChipNumX 14
#define mapChipNumY 10
#define mapChipWidth 48
#define mapChipHeight 48
#define mapChipImgNum 100


class GameManager{
public:
	enum Stage{
		Load,
		Title,
		StageSelect,
		Main,
		GameOver,
		GameClear
	};
	BaseGroup base;
	EnemyGroup enemy;

private:
	Stage _nowStage;
	int _frame;
	int _tlWait; //���Ƃǂꂮ�炢�҂�
	int _tlPhase; //���݂̒i�K
	int _lostPoint; //�˔j���ꂽ�G�̐�
	int _escPoint; //������exit�̐�
	int _selectedBase; //�I������Ă����n�̔ԍ�(-1�Ŕ�I��)
	int _selectedPeople; //�I������Ă���l��
	int _LClick;
	int _RClick;
	int _keyUp;
	int _keyDown;
	int _mouseWheel;

	int _mouseX;
	int _mouseY;

	//�}�b�v�`�b�v�����i�[����z��B������̃^�[�~�l�[�^�[(CRLF\0)���i�[���邽�߂�X��3����
	char _mapChip[mapChipNumY][mapChipNumX+3]; 
	//�}�b�v�̐i�s�������i�[����z��B������̃^�[�~�l�[�^�[���i�[���邽�߂�X��3����
	char _mapPath[mapChipNumY][mapChipNumX + 3];
	int _mapChipImg[mapChipImgNum];

	int _mapChipChar2Idx(char c);

	int _LoadMapChip(std::string filePath); //�}�b�v�`�b�v����ǂݍ��݁B�Ԃ�l�͓ǂݍ��񂾃}�b�v�`�b�v��
	int _LoadMapPath(std::string filePath); //�}�b�v�̐i�s������ǂݍ��݁B�Ԃ�l�͓ǂݍ��񂾃}�b�v�`�b�v��
	
	
	void _LoadMapInfo(std::string filePath); //�}�b�v�̖C����
	boost::property_tree::ptree mapInfo;

	void _MoveLoad();
	void _MoveTitle();
	void _MoveStageSelect();
	void _MoveMain();
	void _MoveGameOver();
	void _MoveGameClear();
	void _DrawLoad();
	void _DrawTitle();
	void _DrawStageSelect();
	void _DrawMain();
	void _DrawGameOver();
	void _DrawGameClear();

public:
	class Vec2D{
	public:
		int x;
		int y;
		Vec2D():x(0),y(0){ }
		Vec2D(int X, int Y){
			x = X;
			y = Y;
		}
	};
	int imgTitle;
	int imgGOver;
	int imgStart;
	int imgGoal;
	int imgSpeak[14];
	int imgWindow;
	int sndTitle;
	int sndMain;
	std::vector<Vec2D> startPoint;
	Vec2D goalPoint;

	int imgAttacker[3]; //0: �N�����Ȃ� 1: �l������: 3: �j�󂳂ꂽ
	int imgAttackerAnim[10];
	int imgDefender[3];
	int imgDefenderAnim[10];
	int imgPeople[10];
	std::vector<EnemyPattern> enmPtn;

	void Move(){
		GetMousePoint(&_mouseX, &_mouseY);
		if ((GetMouseInput() & MOUSE_INPUT_LEFT) != 0){
			 _LClick ++; 
		}else
		{
			_LClick = 0;
		}
		if ((GetMouseInput() & MOUSE_INPUT_RIGHT) != 0){
			 _RClick ++; 
		}
		else
		{
			_RClick = 0;
		}

		_mouseWheel = GetMouseWheelRotVol();
		if (CheckHitKey(KEY_INPUT_UP) != 0){
			 _keyUp++; if (_keyUp == 1){ _mouseWheel++; } 
		}
		else
		{
			_keyUp = 0;
		}

		if (CheckHitKey(KEY_INPUT_DOWN) != 0){
			 _keyDown++; if (_keyDown == 1){ _mouseWheel--; } 
		}
		else
		{
			_keyDown = 0;
		}

		switch (_nowStage){
		case Load:
			_MoveLoad();
			break;
		case Title:
			_MoveTitle();
			break;
		case StageSelect:
			_MoveStageSelect();
			break;
		case Main:
			_MoveMain();
			break;
		case GameOver:
			_MoveGameOver();
			break;
		case GameClear:
			_MoveGameClear();
			break;
		default:
			_MoveLoad();
			break;
		}
		_frame++;
	}
	void Draw(){
		switch (_nowStage){
		case Load:
			_DrawLoad();
			break;
		case Title:
			_DrawTitle();
			break;
		case StageSelect:
			_DrawStageSelect();
			break;
		case Main:
			_DrawMain();
			break;
		case GameOver:
			_DrawGameOver();
			break;
		case GameClear:
			_DrawGameClear();
			break;
		default:
			_MoveLoad();
			break;
		}
	}
	void SetStage(Stage stage){
		_nowStage = stage;
		_frame = 0;
	}
	GameManager(){
		SetStage(Load);

	}
};