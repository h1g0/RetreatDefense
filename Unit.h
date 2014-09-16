#pragma once

#include <list>
#include<vector>
#include<DxLib.h>
class Unit{
public:
	enum Status{
		Dead,
		Moving,
		Attacking,
	};
	enum Vector{
		Left,
		Up,
		Right,
		Down,
		Stop
	};
private:
	int _x;
	int _y;
	int _hp;
	int _maxHp;
	Vector _vect;
	Status _status;
	int _speed;
	int _moveFrame; //初期値(0)から_speed分増やしていき、マップチップの大きさ以上になったら位置を次に進めて初期値に戻す
	int _animeNum; //現在表示中のアニメーション画像番号
	int _animeNumMax; //アニメーション画像番号最大値
	int _animeFrame; //アニメーションフレーム
	int _animeFrameMax; //アニメーションフレーム最大値
	bool _movedFlg; //移動が完了したことを伝えるフラグ
	int _atkFrm;
	int _atkFrmMax;
	bool _atkFlg;
public:
	int x(){ return _x; }
	int y(){ return _y; }
	int hp(){ return _hp; }
	int maxHp(){ return _maxHp; }
	bool movedFlag(){ return _movedFlg; }
	int moveFrame(){ return static_cast<int>(_moveFrame/2); }
	int animeNum(){ return _animeNum; }
	Status status(){ return _status; }
	void SetStatus(Status s){ _status = s; }
	Vector vector(){ return _vect; }
	void SetVector(Vector v){ _vect = v; }
	void SetMovedFlag(bool flg){ _movedFlg = flg; }

	int attackFrame(){ return _atkFrmMax; }
	bool attackFlag(){ return _atkFlg; }
	void SetAttackFlag(bool b){ _atkFlg = b; _atkFrm = 0; }
	void SetAttackFrame(int max){
		_atkFrmMax = max;
		_atkFrm = 0;
		_atkFlg = false;
	}
	bool MoveAttackFrame(){
		if (_atkFlg){ return true; }
		if (_atkFrm >= _atkFrmMax){
			_atkFlg = true;
			_atkFrm = 0;
		}
		else{
			_atkFrm++;
		}
		return _atkFlg;
	}
	void SetHp(int hp){
		if (hp > 0){ _hp = hp; }
		else{ _hp = 0; _status = Dead; }
	}
	int AddHp(int hp){
		SetHp(hp + _hp);
		return _hp;
	}
	void Create(int x, int y, int hp, Vector vector, int speed){
		_x = x;
		_y = y;
		SetHp(hp);
		_maxHp = _hp;
		SetMovedFlag(true);
		_moveFrame = 0;
		_speed = speed;
	}
	void Move(){
		if (movedFlag()){ return; } //移動完了が伝達されていなかったら移動しない
		if (_status == Moving){
			_moveFrame += _speed;
			if (_moveFrame >= 96){
				SetMovedFlag(true);
				_moveFrame = 0;
				switch (_vect){
				case Left:
					_x--;
					break;
				case Up:
					_y--;
					break;
				case Right:
					_x++;
					break;
				case Down:
					_y++;
					break;
				}
			}
		}

	}
	//void Draw();
	void SetAnime(int animeNum, int animeFrame){
		_animeFrame = 0;
		_animeFrameMax = animeFrame;
		_animeNum = 0;
		_animeNumMax = animeNum;
	}
	void MoveAnime(){
		if (_animeFrame+1 >= _animeFrameMax){
			_animeFrame = 0;
			if (_animeNum+1 >= _animeNumMax){
				_animeNum = 0;
			}
			else{
				_animeNum++;
			}
		}
		else{
			_animeFrame++;
		}
	}

};

class Base :public Unit{
private:
	int _atkRange;
	int _dfPow;
	bool _attacker;
	int _people;
	int _catched; //捉えられる上限
public:
	int catched(){ return _catched; }
	void SetCatched(int i){
		_catched = i;
		if (_catched < 0){ _catched = 0; }
	}
	void AddCatched(int i){
		SetCatched(i + _catched);
	}

	int people(){
		return _people;
	}
	void SetPeople(int i){
		if (i < 0){ _people = 0; }
		else{
			_people = i;
		}
	}
	void AddPeople(int i){
		SetPeople(i + _people);
	}
	int attackRange(){
		return _atkRange;
	}
	int defencePower(){
		if (_attacker){ return 0; }
		if (_people <= 0){ return 0; }
		else if (_people <= 1 || _people == 2){ return 5; }
		else if (_people == 3 || _people == 4){ return 6; }
		else if (_people == 5 || _people == 6){ return 7; }
		else if (_people == 7 || _people == 8){ return 8; }
		else if (_people == 9 || _people == 10){ return 9; }
		else if (_people >= 11){ return 10; }
	}
	int attackPower(){
		if (_attacker){
			if (_people <= 0){ return 0; }
			if (_people == 1){ return 2; }
			else if (_people == 2){ return 4; }
			else if (_people == 3){ return 6; }
			else if (_people == 4){ return 8; }
			else if (_people == 5){ return 10; }
			else if (_people == 6){ return 14; }
			else if (_people == 7){ return 18; }
			else if (_people == 8){ return 22; }
			else if (_people == 9){ return 26; }
			else if (_people >= 10){ return 30; }
		}
		else{
			if (_people <= 0){ return 0; }
			else if (_people == 1){ return 0; }
			else if (_people == 2){ return 0; }
			else if (_people == 3){ return 1; }
			else if (_people == 4){ return 2; }
			else if (_people == 5){ return 4; }
			else if (_people == 6){ return 6; }
			else if (_people == 7){ return 9; }
			else if (_people == 8){ return 12; }
			else if (_people == 9){ return 16; }
			else if (_people >= 10){ return 20; }
		}
	}

	

	bool isAttacker(){
		return _attacker;
	}
	void Create(int x, int y, int hp, Vector vector, int speed, int attackRange, int defencePow,bool isAttacker, int people){
		Unit::Create(x, y, hp, vector, speed);
		_atkRange = attackRange;
		_dfPow = defencePow;
		_attacker = isAttacker;
		_people = people;
		_catched = 0;

	}
};


class BaseGroup{
public:
	std::vector<Base> unit;
	void Init(){
		unit.clear();
	}
};

class Enemy: public Unit{
	int _id;
	int _atkPow;
	int _dfPow;
	int _catched; //捉えられる上限
	int _atkId; //攻撃する防衛基地のID

	int _speakId;
	int _speakFrame;
public:
	int speakId(){ return _speakId; }
	void SetSpeak(){
		_speakId = DxLib::GetRand(13);
		_speakFrame = 180 + DxLib::GetRand(600);
	}
	bool MoveSpeak(){
		if (_speakFrame <= 0){
			SetSpeak();
			return false;
		}
		if (_speakFrame <= 180){
			_speakFrame--;
			return true;
		}
		else{
			_speakFrame--;
			return false;
		}
	}
	int id(){ return _id; }
	int attackPow(){ return _atkPow; }
	int defencePow(){ return _dfPow; }
	int catched(){ return _catched; }
	int attackId(){ return _atkId; }
	void SetAttackId(int i){ _atkId = i; }
	void SetCatched(int i){
		_catched = i;
		if (_catched < 0){ _catched = 0; }
	}
	void AddCatched(int i){
		SetCatched(i + _catched);
	}


	void Create(int x, int y, int hp, Vector vector, int speed, int id, int attackPow, int defencePow){
		Unit::Create(x, y, hp, vector, speed);
		_id = id;
		_atkPow = attackPow;
		_dfPow = defencePow;
		SetStatus(Moving);
		_catched = 0;
		_atkId = -1;
		SetSpeak();
	}

};

class EnemyGroup{
public:
	std::vector<Enemy> unit;
	void Init(){
		unit.clear();
	}
};

class EnemyPattern{
public:
	int hp;
	int atk;
	int img;
	int imgAtk[10];
};
