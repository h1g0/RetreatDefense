#include "GameManager.h"
#include<DxLib.h>
#include<cmath>
#include<boost/lexical_cast.hpp>
#include<boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
int GameManager::_LoadMapChip(std::string filePath){
	int result = 0;
	int file = FileRead_open(filePath.c_str());
	for (int i = 0; i < mapChipNumY; i++){
		result = result + FileRead_gets(_mapChip[i], mapChipNumX+3, file);
	}
	FileRead_close(file);
	return result;
}

int GameManager::_LoadMapPath(std::string filePath){
	int result = 0;
	int file = FileRead_open(filePath.c_str());
	for (int i = 0; i < mapChipNumY; i++){
		result = result + FileRead_gets(_mapPath[i], mapChipNumX + 3, file);
	}
	FileRead_close(file);
	return result;
}

void GameManager::_LoadMapInfo(std::string filePath){
	boost::property_tree::ptree t;
	read_xml(filePath, t);
	//スタート地点
	int repeat = t.get<int>("start.num", 1);
	startPoint.clear();

	for (int i = 0; i < repeat; i++){
		std::string s = boost::lexical_cast<std::string>(i);
		startPoint.push_back(Vec2D(t.get<int>("start." + s + ".x", 0), t.get<int>("start." + s + ".y", 0)));
	}

	//ゴール地点
	goalPoint.x = t.get<int>("goal.x", mapChipNumX - 1);
	goalPoint.y = t.get<int>("goal.y", mapChipNumY - 1);

	//砲台
	repeat = t.get<int>("base.num", 0);
	base.Init();
	for (int i = 0; i < repeat; i++){
		Base b;
		std::string s = boost::lexical_cast<std::string>(i);
		if (t.get<bool>("base." + s + ".attack",false)){
			b.Create(t.get<int>("base." + s + ".x", 0),
				t.get<int>("base." + s + ".y", 0),
				1,
				b.Stop,
				0,
				4,
				0, true, t.get<int>("base." + s + ".people", 0));
		}
		else{
			b.Create(t.get<int>("base." + s + ".x", 0),
				t.get<int>("base." + s + ".y", 0),
				100,
				b.Stop,
				0,
				1,
				5, false,t.get<int>("base." + s + ".people", 0));
		}
		base.unit.push_back(b);
	}
	mapInfo = t;
	_tlPhase = 0;
	_tlWait = mapInfo.get<int>("timeline.0.wait", 0);
}

void GameManager::_MoveLoad(){
	imgTitle = LoadGraph("img\\title.png");
	imgGOver = LoadGraph("img\\title_end.png");

	LoadDivGraph("img\\mapchipall.png", mapChipImgNum, 10, 10, 48, 48, _mapChipImg);
	imgGoal = LoadGraph("img\\exit.png");

	imgAttacker[0] = LoadGraph("img\\kougeki1.png");
	imgAttacker[1] = LoadGraph("img\\kougeki2.png");
	imgAttacker[2] = LoadGraph("img\\kougeki3.png");
	imgDefender[0] = LoadGraph("img\\boei1.png");
	imgDefender[1] = LoadGraph("img\\boei2.png");
	imgDefender[2] = LoadGraph("img\\boei3.png");
	LoadDivGraph("img\\boei_anim.png", 10, 10, 1, 48, 48, imgDefenderAnim);
	LoadDivGraph("img\\kougeki_anim.png", 10, 10, 1, 48, 48, imgAttackerAnim);

	for (int i = 0; i < 14; i++){
		std::string path = "img\\serihu" + boost::lexical_cast<std::string>(i+1)+".png";
		imgSpeak[i] = LoadGraph(path.c_str());
	}
	imgWindow = LoadGraph("img\\window.png");
	//敵パターン及び画像読み込み
	enmPtn.clear();
	boost::property_tree::ptree t;
	read_xml("map\\enemy.xml", t);
	for (int i = 0; i < t.get<int>("pattern.num", 0); i++){
		std::string s = boost::lexical_cast<std::string>(i);
		EnemyPattern e;
		e.atk = t.get<int>("pattern." + s + ".attack");
		e.hp = t.get<int>("pattern." + s + ".hp");
		e.img = LoadGraph(std::string("img\\"+t.get<std::string>("pattern." + s + ".image","error.png")).c_str());
		LoadDivGraph(std::string("img\\" + t.get<std::string>("pattern." + s + ".attack_image", "error.png")).c_str(),10,10,1,48,48,e.imgAtk);
		enmPtn.push_back(e);
	}
	sndTitle=LoadSoundMem("sound\\title.wav");

	sndMain = LoadSoundMem("sound\\Hamlet.wav");
	SetStage(Title);
}
void GameManager::_MoveTitle(){
	if (_frame == 1){ PlaySoundMem(sndTitle, DX_PLAYTYPE_LOOP); }
	if (_LClick == 1){ StopSoundMem(sndTitle);
	SetStage(StageSelect); }
}
void GameManager::_MoveStageSelect(){
	int stg = 0;
	enemy.Init();
	base.Init();

	std::string s = boost::lexical_cast<std::string>(stg);
	_LoadMapChip("map\\stg"+s+"_map.txt");
	_LoadMapPath("map\\stg"+s+".txt");
	_LoadMapInfo("map\\stg" + s + "_info.xml");
	_lostPoint = 0;
	_escPoint = 0;
	_selectedBase = -1;
	_selectedPeople = 0;
	SetStage(Main);
}
void GameManager::_MoveMain(){
	if (_frame == 1){ PlaySoundMem(sndMain, DX_PLAYTYPE_LOOP); }

	//ゲームクリア判定
	if (_escPoint >= mapInfo.get<int>("exit", 3) && _frame >= mapInfo.get<int>("time", 0)){ SetStage(GameClear); }
	//敵生成
	if (_tlWait == 0){
		if (_tlPhase < mapInfo.get<int>("timeline.num", 0)){ _tlPhase++; }
		std::string s = boost::lexical_cast<std::string>(_tlPhase);
		_tlWait = mapInfo.get<int>("timeline." + s + ".wait", -1);
		int start = mapInfo.get<int>("timeline." + s + ".spawn", 0);
		int ptn = mapInfo.get<int>("timeline." + s + ".pattern", 0);

		Enemy e;
		e.Create(startPoint[start].x, startPoint[start].y, enmPtn[ptn].hp, e.Left, 2, ptn, enmPtn[ptn].atk, 0);
		enemy.unit.push_back(e);
	}
	else{
		_tlWait--;
	}

	//敵移動前に停めている敵がいるかのフラグを立てる
	for (int j = 0; j < base.unit.size(); j++){
		if (base.unit[j].status() != base.unit[j].Dead){
			base.unit[j].SetCatched(0);
			for (int i = 0; i < enemy.unit.size(); i++){
				if (std::abs(enemy.unit[i].x() - base.unit[j].x()) + std::abs(enemy.unit[i].y() - base.unit[j].y()) <= 1){
					base.unit[j].AddCatched(enemy.unit[i].catched());
				}
			}

			for (int i = 0; i < enemy.unit.size(); i++){
				int dist2 = (enemy.unit[i].x() - base.unit[j].x())*(enemy.unit[i].x() - base.unit[j].x()) 
					      + (enemy.unit[i].y() - base.unit[j].y())*(enemy.unit[i].y() - base.unit[j].y());
				if (dist2 <= base.unit[j].attackRange()*base.unit[j].attackRange()){
					if (base.unit[j].people()>0 && base.unit[j].status() != base.unit[j].Attacking){
						base.unit[j].SetStatus(base.unit[j].Attacking);
						base.unit[j].SetAttackFrame((base.unit[j].isAttacker()) ? 120 : 60);
						base.unit[j].SetAnime(10, 5);
					}
					else{
						base.unit[j].MoveAttackFrame();
						if (base.unit[j].attackFlag()){
							base.unit[j].SetAttackFlag(false);
							if (base.unit[j].attackPower() >= enemy.unit[i].hp()){
								enemy.unit[i].SetHp(0);
								base.unit[j].SetStatus(base.unit[j].Moving);
							}
							else{
								enemy.unit[i].AddHp(base.unit[j].attackPower()*-1);
							}
						}
					}
				}
			}
			//マウス選択処理
			if (_LClick==1&& std::abs(base.unit[j].x()*mapChipWidth + mapChipWidth/2 - _mouseX) <= mapChipWidth / 2 && std::abs(base.unit[j].y()*mapChipHeight + mapChipHeight/2 - _mouseY) <= mapChipHeight / 2){
				if (_selectedBase == j){
					_selectedBase = -1;
					_selectedPeople = 0;
				}
				else if (_selectedBase > -1 && _selectedPeople > 0){//送り先に選ばれた
					if (base.unit[_selectedBase].people() >= _selectedPeople){
						base.unit[_selectedBase].AddPeople(-_selectedPeople);
						base.unit[j].AddPeople(_selectedPeople);
					}
					_selectedBase = -1;
					_selectedPeople = 0;
				}
				else{
					_selectedBase = j;
					_selectedPeople = 0;
				}
			}
		}
	}
	//Exitを選択できるのは送り先のみ
	if (_LClick==1&& std::abs(goalPoint.x*mapChipWidth + mapChipWidth / 2 - _mouseX) <= mapChipWidth / 2 && std::abs(goalPoint.y*mapChipHeight + mapChipHeight/2 - _mouseY) <= mapChipHeight / 2){
		if (_selectedBase > -1 && _selectedPeople > 0){//送り先に選ばれた
			if (base.unit[_selectedBase].people() >= _selectedPeople){
				base.unit[_selectedBase].AddPeople(-_selectedPeople);
				_escPoint += _selectedPeople;
			}
			_selectedBase = -1;
			_selectedPeople = 0;
		}
		else{
			_selectedBase = -1;
			_selectedPeople = 0;
		}
	}
	//右クリック処理
	if (_RClick==1){
		_selectedBase = -1;
		_selectedPeople = 0;
	}

	//マウスホイール処理
	if (_selectedBase>-1 && _mouseWheel > 0){
		if (_selectedPeople + _mouseWheel <= base.unit[_selectedBase].people()){
			_selectedPeople = _selectedPeople + _mouseWheel;
		}
		else{
			_selectedPeople = base.unit[_selectedBase].people();
		}
	}
	else if (_selectedBase > -1 && _mouseWheel < 0){
		if (_selectedPeople + _mouseWheel >= 0){
			_selectedPeople = _selectedPeople + _mouseWheel;
		}
		else{
			_selectedPeople = 0;
		}
	}

	//敵移動
	for (int i = 0; i < enemy.unit.size(); i++){

		enemy.unit[i].Move();
		if (enemy.unit[i].movedFlag()){
			int x = enemy.unit[i].x();
			int y = enemy.unit[i].y();
			switch (_mapPath[y][x]){
			case 'l':
				enemy.unit[i].SetVector(enemy.unit[i].Left);
				break;
			case 'r':
				enemy.unit[i].SetVector(enemy.unit[i].Right);
				break;
			case 'u':
				enemy.unit[i].SetVector(enemy.unit[i].Up);
				break;
			case 'd':
				enemy.unit[i].SetVector(enemy.unit[i].Down);
				break;
			}

			//防衛拠点と隣接したら戦闘状態に
			for (int j = 0; j < base.unit.size(); j++){
				int catchedLimit = 50;

				if (base.unit[j].status() != base.unit[j].Dead && base.unit[j].catched() == 0){
					if (std::abs(x - base.unit[j].x()) + std::abs(y - base.unit[j].y()) <= 1){
						enemy.unit[i].SetStatus(enemy.unit[i].Attacking);
						enemy.unit[i].AddCatched(catchedLimit);
						enemy.unit[i].SetAnime(10, 20);
						enemy.unit[i].SetAttackId(j);
						enemy.unit[i].SetAttackFrame(60);
					}
				}
			}

			//ゴールに接触したら消す
			if (x == goalPoint.x && y == goalPoint.y){
				_lostPoint ++;
				enemy.unit[i].SetHp(0);

				if (_lostPoint >= mapInfo.get<int>("lose", 3)){ SetStage(GameOver); }
			}
			
			//敵同志で重なったらの処理
			for (int j = 0; j < enemy.unit.size(); j++){
				if (i!=j && enemy.unit[i].x() == enemy.unit[j].x() &&
					enemy.unit[i].y() == enemy.unit[j].y()){
					if (enemy.unit[i].hp() <= 0 || enemy.unit[j].hp() <= 0){ break; }
					if (enemy.unit[j].catched()>0){
						if (enemy.unit[j].hp() < enemy.unit[j].catched()){ //止まってる側が重なったものを吸収
							if (enemy.unit[j].hp() + enemy.unit[i].hp() >= enemy.unit[j].catched()){ //溢れる
								int overed = enemy.unit[j].catched() - enemy.unit[j].hp();
								enemy.unit[j].AddHp(overed);
								enemy.unit[i].AddHp(-overed);
							}
							else{
								enemy.unit[j].AddHp(enemy.unit[i].hp());
								enemy.unit[i].SetHp(0);
							}

						}
					//if (enemy.unit[i].attackPow() < enemy.unit[j].attackPow()){ //jのほうが攻撃力が高いなら
					}else{
						if (!enemy.unit[j].movedFlag()){ break; }
						enemy.unit[j].AddHp(enemy.unit[i].hp());
						enemy.unit[i].SetHp(0);
					}
				}
			}

			enemy.unit[i].SetMovedFlag(false);
		}
		else{ //MovedFlagが立っていない
			if (enemy.unit[i].status() == enemy.unit[i].Attacking){ //攻撃処理
				enemy.unit[i].MoveAttackFrame();
				if (enemy.unit[i].attackFlag()){
					enemy.unit[i].SetAttackFlag(false);
					int j = enemy.unit[i].attackId();
					int atkTimes = static_cast<int>((enemy.unit[i].hp() - 1) / 10) + 1;
					int atkPow = enemy.unit[i].attackPow() - base.unit[j].defencePower();
					if (atkPow < 0){ atkPow = 0; }
					if (base.unit[j].hp() - atkPow*atkTimes <= 0){ //基地が破壊された
						base.unit[j].SetHp(0);
						enemy.unit[i].SetAttackId(-1);
						enemy.unit[i].SetCatched(0);
						enemy.unit[i].SetStatus(enemy.unit[i].Moving);
					}
					else{
						base.unit[j].AddHp(-atkPow*atkTimes);
					}
				}
			}
		}
	}

	//敵消去(最後に)
	for (std::vector<Enemy>::iterator i = enemy.unit.begin(); i != enemy.unit.end();){
		if (i->hp() <= 0){
			i = enemy.unit.erase(i);
		}
		else{
			i++;
		}
	}
}
void GameManager::_MoveGameOver(){
	if (_frame == 1){ StopSoundMem(sndMain); }
	if (_frame >= 180 ){ SetStage(Title); }
}
void GameManager::_MoveGameClear(){
	if (_frame == 1){ StopSoundMem(sndMain); }
	if (_frame>=180 ){ SetStage(Title); }
}

void GameManager::_DrawGameClear(){
	//SetStage(Title)
	//DrawGraph(0, 0, imgGOver, FALSE);
	DrawString(60, 240, "The rest is silence.", GetColor(255, 255, 255));
	DrawString(120, 260, " げーむくりあ。おめでとう", GetColor(255, 255, 255));

}
void GameManager::_DrawGameOver(){
	//SetStage(Title)
	DrawGraph(0, 0, imgGOver, FALSE);
	DrawString(60, 240, "There are more things in heaven and earth, Horatio,", GetColor(255, 255, 255));
	DrawString(120, 260, " Than are dreamt of in your philosophy.", GetColor(255, 255, 255));

}

void GameManager::_DrawLoad(){
	DrawString(0, 0, "ろーど", GetColor(255, 255, 255));
}
void GameManager::_DrawTitle(){
	DrawGraph(0, 0,imgTitle,FALSE);
	DrawString(450, 30, "クリックすると始まる", GetColor(255, 255, 255));
}
void GameManager::_DrawStageSelect(){
	DrawString(0, 0, "すてーじせれくと", GetColor(255, 255, 255));
}

void GameManager::_DrawMain(){
	//マップ描画
	int img = 0;
	for (int i = 0; i < mapChipNumY; i++){
		for (int j = 0; j < mapChipNumX; j++){
			img = _mapChipChar2Idx(_mapChip[i][j]);
			DrawGraph(j*mapChipWidth, i*mapChipHeight, img, TRUE);
			//DrawFormatString(j*mapChipWidth, i*mapChipHeight, GetColor(255, 255, 255), "%c", _mapPath[i][j]);
		}
	}
	//ゴール地点
	DrawGraph(goalPoint.x*mapChipWidth, goalPoint.y*mapChipHeight, imgGoal, TRUE);

	//拠点
	for (int i = 0; i < base.unit.size(); i++){
		int x = base.unit[i].x()*mapChipWidth;
		int y = base.unit[i].y()*mapChipHeight;
		bool atk = base.unit[i].isAttacker();
		int img = 0;
		if (base.unit[i].hp()>0){
			if (base.unit[i].status() == base.unit[i].Attacking){
				base.unit[i].MoveAnime();
				img = (atk) ? imgAttackerAnim[base.unit[i].animeNum()] : imgDefenderAnim[base.unit[i].animeNum()];
			}
			else{
				if (base.unit[i].people() > 0){
					if (atk){
						img = imgAttacker[1];
					}
					else{
						img = imgDefender[1];
					}
				}
				else{
					if (atk){
						img = imgAttacker[0];
					}
					else{
						img = imgDefender[0];
					}
				}
			}
		}
		else{
			if (atk){
				img = imgAttacker[2];
			}
			else{
				img = imgDefender[2];
			}
		}

		DrawGraph(x, y, img, TRUE);
		SetDrawBlendMode(DX_BLENDMODE_ALPHA, 192);
		if (base.unit[i].hp()>0){ DrawFormatString(x + 24, y-16, GetColor(0, 255, 0), "x%d", base.unit[i].people()); }
		DrawLine(x, y + 49, x + 48, y + 49, GetColor(0, 0, 0), 3);
		int w = static_cast<int>(static_cast<double>(base.unit[i].hp()) / static_cast<double>(base.unit[i].maxHp())*48);
		DrawLine(x, y + 49, x + w, y + 49, GetColor(0, 255, 0), 3);
		SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

		//敵描画
		for (int i = 0; i < enemy.unit.size(); i++){
			int x = 0;
			int y = 0;

			switch (enemy.unit[i].vector()){
			case Enemy::Left:
				x = -enemy.unit[i].moveFrame();
				break;
			case Enemy::Right:
				x = enemy.unit[i].moveFrame();
				break;
			case Enemy::Up:
				y = -enemy.unit[i].moveFrame();
				break;
			case Enemy::Down:
				y = enemy.unit[i].moveFrame();
				break;
			}

			switch (enemy.unit[i].status())
			{
			case Enemy::Moving:
				DrawGraph(enemy.unit[i].x()*mapChipWidth + x, enemy.unit[i].y()*mapChipHeight + y, enmPtn[enemy.unit[i].id()].img, TRUE);
				break;
			case Enemy::Attacking:
				enemy.unit[i].MoveAnime();
				DrawGraph(enemy.unit[i].x()*mapChipWidth + x, enemy.unit[i].y()*mapChipHeight + y, enmPtn[enemy.unit[i].id()].imgAtk[enemy.unit[i].animeNum()], TRUE);
				break;
			}
			if (enemy.unit[i].MoveSpeak()){
				DrawGraph(enemy.unit[i].x()*mapChipWidth + x - 24, enemy.unit[i].y()*mapChipHeight + y - 48, imgSpeak[enemy.unit[i].speakId()], TRUE);
			}
			SetDrawBlendMode(DX_BLENDMODE_ALPHA, 192);
			DrawFormatString(enemy.unit[i].x()*mapChipWidth + x + 24, enemy.unit[i].y()*mapChipHeight + y - 16, GetColor(255, 0, 0), "x%d", enemy.unit[i].hp());
			SetDrawBlendMode(DX_BLENDMODE_NOBLEND, 0);

			//ウィンドウ描画
			if (_selectedBase > -1){
				int x = base.unit[_selectedBase].x()*mapChipWidth + 24;
				int y = base.unit[_selectedBase].y()*mapChipHeight + 24;
				DrawGraph(x, y, imgWindow, TRUE);
				DrawFormatString(x + 8, y + 8, GetColor(0, 0, 0), "%d", _selectedPeople);
			}

			//情報描画
			DrawFormatString(0, 0, GetColor(255, 255, 255), "逃がす目標: %d/%d", _escPoint, mapInfo.get<int>("exit", 0));
			DrawFormatString(0, 20, GetColor(255, 255, 255), "逃した敵の数: %d/%d", _lostPoint, mapInfo.get<int>("lose", 3));
			DrawFormatString(400, 0, (_frame < mapInfo.get<int>("time", 0)) ? GetColor(255, 255, 255) : GetColor(255, 0, 0), "時間: %d秒/%d秒", static_cast<int>(_frame / 60), static_cast<int>(mapInfo.get<int>("time", 0) / 60));

		}
	}
}


int GameManager::_mapChipChar2Idx(char c){
	int img=0;
	switch (c){
	case 'A':
		img = _mapChipImg[0];
		break;
	case 'B':
		img = _mapChipImg[1];
		break;
	case 'C':
		img = _mapChipImg[2];
		break;
	case 'D':
		img = _mapChipImg[3];
		break;
	case 'E':
		img = _mapChipImg[4];
		break;
	case 'F':
		img = _mapChipImg[5];
		break;
	case 'G':
		img = _mapChipImg[6];
		break;
	case 'H':
		img = _mapChipImg[7];
		break;
	case 'I':
		img = _mapChipImg[8];
		break;
	case 'J':
		img = _mapChipImg[9];
		break;
	case 'K':
		img = _mapChipImg[10];
		break;
	case 'L':
		img = _mapChipImg[11];
		break;
	case 'M':
		img = _mapChipImg[12];
		break;
	case 'N':
		img = _mapChipImg[13];
		break;
	case 'O':
		img = _mapChipImg[14];
		break;
	case 'P':
		img = _mapChipImg[15];
		break;
	case 'Q':
		img = _mapChipImg[16];
		break;
	case 'R':
		img = _mapChipImg[17];
		break;
	case 'S':
		img = _mapChipImg[18];
		break;
	case 'T':
		img = _mapChipImg[19];
		break;
	case 'U':
		img = _mapChipImg[20];
		break;
	case 'V':
		img = _mapChipImg[21];
		break;
	case 'W':
		img = _mapChipImg[22];
		break;
	case 'X':
		img = _mapChipImg[23];
		break;
	case 'Y':
		img = _mapChipImg[24];
		break;
	case 'Z':
		img = _mapChipImg[25];
		break;
	case 'a':
		img = _mapChipImg[26];
		break;
	case 'b':
		img = _mapChipImg[27];
		break;
	case 'c':
		img = _mapChipImg[28];
		break;
	case 'd':
		img = _mapChipImg[29];
		break;
	case 'e':
		img = _mapChipImg[30];
		break;
	case 'f':
		img = _mapChipImg[31];
		break;
	case 'g':
		img = _mapChipImg[32];
		break;
	case 'h':
		img = _mapChipImg[33];
		break;
	case 'i':
		img = _mapChipImg[34];
		break;
	case 'j':
		img = _mapChipImg[35];
		break;
	case 'k':
		img = _mapChipImg[36];
		break;
	case 'l':
		img = _mapChipImg[37];
		break;
	case 'm':
		img = _mapChipImg[38];
		break;
	case 'n':
		img = _mapChipImg[39];
		break;
	case 'o':
		img = _mapChipImg[40];
		break;
	case 'p':
		img = _mapChipImg[41];
		break;
	case 'q':
		img = _mapChipImg[42];
		break;
	case 'r':
		img = _mapChipImg[43];
		break;
	case 's':
		img = _mapChipImg[44];
		break;
	case 't':
		img = _mapChipImg[45];
		break;
	case 'u':
		img = _mapChipImg[46];
		break;
	case 'v':
		img = _mapChipImg[47];
		break;
	case 'w':
		img = _mapChipImg[48];
		break;
	case 'x':
		img = _mapChipImg[49];
		break;
	case 'y':
		img = _mapChipImg[50];
		break;
	case 'z':
		img = _mapChipImg[51];
		break;
	case '0':
		img = _mapChipImg[52];
		break;
	case '1':
		img = _mapChipImg[53];
		break;
	case '2':
		img = _mapChipImg[54];
		break;
	case '3':
		img = _mapChipImg[55];
		break;
	case '4':
		img = _mapChipImg[56];
		break;
	case '5':
		img = _mapChipImg[57];
		break;
	case '6':
		img = _mapChipImg[58];
		break;
	case '7':
		img = _mapChipImg[59];
		break;
	case '8':
		img = _mapChipImg[60];
		break;
	case '9':
		img = _mapChipImg[61];
		break;
	default:
		img = _mapChipImg[0];
		break;
	}
	return img;
}
