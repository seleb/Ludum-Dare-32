#include "TestGame.h"
#include "BaseScene.h"
#include <string>
#include "TestScene2D.h"
#include "GameJamSceneIndoor.h"
#include "GameJamSceneOutdoor.h"

#include "Character.h"

TestGame::TestGame(bool _running):
	Game(_running)
{
	Character::init();
	scenes.insert(std::make_pair("outdoors", new GameJamSceneOutdoor(this)));
	scenes.insert(std::make_pair("indoors", new GameJamSceneIndoor(this)));
	currentScene = scenes.at("indoors");
}

TestGame::~TestGame(){
}

void TestGame::update(){
	Game::update();
}

void TestGame::draw(){
	Game::draw();
}