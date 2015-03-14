#pragma once

#include <PuppetCharacterCastleChampion.h>
#include <PuppetTexturePack.h>
#include <BehaviourPatrol.h>
#include <BehaviourAttack.h>
#include <BehaviourManager.h>
#include <Item.h>
#include <ItemFlail.h>
#include <Box2D\Box2D.h>
#include <Box2DWorld.h>

PuppetCharacterCastleChampion::PuppetCharacterCastleChampion(Box2DWorld * _world, int16 _categoryBits, int16 _maskBits, int16 _groupIndex):
	PuppetCharacter(new PuppetTexturePack(
		RaidTheCastleResourceManager::castleChampionTorso,
		RaidTheCastleResourceManager::castleChampionArm,
		RaidTheCastleResourceManager::castleChampionHelmet
	), true, _world, _categoryBits, _maskBits, _groupIndex),
	NodeTransformable(new Transform()),
	NodeChild(nullptr)
{
	behaviourManager.addBehaviour(new BehaviourPatrol(glm::vec3(50,0,0), glm::vec3(100,0,0), this, 10));
	behaviourManager.addBehaviour(new BehaviourAttack(this, 10, PuppetGame::kPLAYER));
	//behaviourManager.addBehaviour(new BehaviourFollow(this, 10, PuppetGame::kPLAYER));

	itemToPickup = new ItemFlail(_world, PuppetGame::kITEM, PuppetGame::kPLAYER | PuppetGame::kSTRUCTURE | PuppetGame::kGROUND, _groupIndex, 0, 0, -RaidTheCastleResourceManager::itemFlailGrip->height/2.f);
	
}

PuppetCharacterCastleChampion::~PuppetCharacterCastleChampion(){
	//delete texPack;
}

void PuppetCharacterCastleChampion::action(){
	// the castle champion isn't allowed to throw their weapon
}

void PuppetCharacterCastleChampion::update(Step * _step){
	PuppetCharacter::update(_step);

	if(behaviourManager.behaviours.at(1)->active){
		behaviourManager.behaviours.at(0)->active = false;
	}else{
		behaviourManager.behaviours.at(0)->active = true;
	}
}