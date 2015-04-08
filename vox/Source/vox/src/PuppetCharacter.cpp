#pragma once

#include "PuppetCharacter.h"
#include <Box2DSprite.h>
#include <Texture.h>
#include <GameJamCharacter.h>
#include "Box2DWorld.h"
#include "Item.h"
#include "Behaviour.h"
#include <PuppetTexturePack.h>
#include <shader\ShaderComponentHsv.h>
#include <shader\ShaderComponentTint.h>
#include <shader\BaseComponentShader.h>
#include <shader\Shader.h>
#include <RenderOptions.h>
#include <SoundManager.h>
#include <RaidTheCastleResourceManager.h>
#include <TextureSampler.h>
#include <shader\ShaderComponentAlpha.h>
#include <PuppetScene.h>
#include <ParticleSystem.h>
#include <Particle.h>
#include <NumberUtils.h>
#include <BehaviourManager.h>
#include <Sprite.h>

bool PuppetCharacter::compareByScore(PuppetCharacter * _a, PuppetCharacter * _b){
	return (_a->score < _b->score);
}

PuppetCharacter::PuppetCharacter(PuppetTexturePack * _texturePack, float _ghostPosition, bool _ai, Box2DWorld* _world, int16 _categoryBits, int16 _maskBits, int16 _groupIndex):
	Box2DSuperSprite(_world, _categoryBits, _maskBits, _groupIndex),
	NodeTransformable(new Transform()),
	NodeChild(nullptr),
	NodeRenderable(),
	texPack(_texturePack),
	ai(_ai),
	canJump(false),
	dead(false),
	deathPending(false),
	targetRoll(0),
	health(50.0f),
	itemToPickup(nullptr),
	heldItem(nullptr),
	itemJoint(nullptr),
	behaviourManager(new BehaviourManager(this)),
	score(0.f),
	lastUpdateScore(0.f),
	control(1.f),
	id(-1),
	actionThrottle(0.333),
	lastActionTime(0.0),
	ghostPosition(_ghostPosition),
	contactDelegate(nullptr),
	justTookDamage(false),
	indicator(nullptr)
{
	init();
}

PuppetCharacter * PuppetCharacter::clone(Box2DWorld * _world){
	PuppetCharacter * res = new PuppetCharacter(texPack, ghostPosition, ai, _world, categoryBits, maskBits, groupIndex);
	res->id = id;
	res->score = score;
	return res;
}

PuppetCharacter::~PuppetCharacter(){
	delete behaviourManager;
	//delete texPack;
}

void PuppetCharacter::init(){
	bool defaultTex = false;
	if(texPack == nullptr){
		defaultTex = true;
		texPack = new PuppetTexturePack(
			RaidTheCastleResourceManager::knightTorso,
			RaidTheCastleResourceManager::knightArm,
			RaidTheCastleResourceManager::knightHelmet
		);
	}

	head = new Box2DSprite(world, texPack->headTex, b2_dynamicBody, false, nullptr, new Transform(), componentScale*texPack->scale);
	face = new Box2DSprite(world, texPack->faceTex, b2_dynamicBody, false, nullptr, new Transform(), componentScale*texPack->scale*0.5f);
	handLeft = new Box2DSprite(world, texPack->handTex, b2_dynamicBody, false, nullptr, new Transform(), componentScale*texPack->scale);
	handRight = new Box2DSprite(world, texPack->handTex, b2_dynamicBody, false, nullptr, new Transform(), componentScale*texPack->scale);

	torso = new Box2DSprite(world, texPack->torsoTex, b2_dynamicBody, false, nullptr, new Transform(), componentScale*texPack->scale);
	armLeft = new Box2DSprite(world, texPack->armTex, b2_dynamicBody, false, nullptr, new Transform(), componentScale*texPack->scale);
	armRight = itemHolder = new Box2DSprite(world, texPack->armTex, b2_dynamicBody, false, nullptr, new Transform(), componentScale*texPack->scale);
	headgear = new Box2DSprite(world, texPack->headgearTex, b2_dynamicBody, false, nullptr, new Transform(), componentScale*texPack->scale);

	components.push_back(&armLeft);
	components.push_back(&armRight);
	components.push_back(&handLeft);
	components.push_back(&handRight);
	components.push_back(&torso);
	components.push_back(&head);
	components.push_back(&face);
	components.push_back(&headgear);

	rootComponent = torso;

	b2Filter sf;
	sf.categoryBits = categoryBits;
	if(maskBits != static_cast<int16>(-1)){
		sf.maskBits = maskBits;
	}
	sf.groupIndex = groupIndex;

	b2CircleShape testshape;
	testshape.m_radius = 1.f;
	testshape.m_p = b2Vec2(0, -2.25f);
	b2FixtureDef test;
	test.shape = &testshape;
	test.density = 150.f;
	test.userData = this;
	test.friction = 1;
	test.filter = sf;
	b2Fixture * testf = torso->body->CreateFixture(&test);
	sf.groupIndex = groupIndex;

	torso->createFixture	 (sf, b2Vec2(0.0f, -1.f), this);
	armLeft->createFixture	 (sf, b2Vec2(0.0f, 0.0f), this);
	armRight->createFixture	 (sf, b2Vec2(0.0f, 0.0f), this);
	handLeft->createFixture  (sf, b2Vec2(0.0f, 0.0f), this);
	handRight->createFixture (sf, b2Vec2(0.0f, 0.0f), this);
	face->createFixture		 (sf, b2Vec2(0.0f, 0.0f), this);
	headgear->createFixture	 (sf, b2Vec2(0.0f, 0.0f), this);
	head->createFixture		 (sf, b2Vec2(0.0f, 0.0f), this);

	b2RevoluteJointDef jth;
	jth.bodyA = torso->body;
	jth.bodyB = head->body;
	jth.localAnchorA.Set(0, 0.4f * torso->getCorrectedHeight());
	jth.localAnchorB.Set(0, -0.9f * head->getCorrectedHeight());
	jth.collideConnected = false;
	jth.enableLimit = true;
	jth.enableMotor = true;
	jth.maxMotorTorque = 0;
	jth.motorSpeed = 0;
	jth.referenceAngle = 0;
	jth.lowerAngle = glm::radians(-15.f);
	jth.upperAngle = glm::radians(15.f);
	world->b2world->CreateJoint(&jth);

	// face
	b2RevoluteJointDef jhf;
	jhf.bodyA = head->body;
	jhf.bodyB = face->body;
	jhf.localAnchorA.Set(0, 0);
	jhf.localAnchorB.Set(0, 0);
	jhf.collideConnected = false;
	jhf.enableLimit = true;
	jhf.referenceAngle = 0;
	world->b2world->CreateJoint(&jhf);
	
	// headgear
	b2WeldJointDef jhh;
	//b2RevoluteJointDef jhh;
	jhh.bodyA = head->body;
	jhh.bodyB = headgear->body;
	jhh.localAnchorA.Set(0, 0.5f * head->getCorrectedHeight());
	jhh.localAnchorB.Set(0, -0.1f * headgear->getCorrectedHeight());
	jhh.collideConnected = false;
	//jhh.enableLimit = true;
	jhh.referenceAngle = 0;
	world->b2world->CreateJoint(&jhh);

	// right arm
	b2RevoluteJointDef jtar;
	jtar.bodyA = torso->body;
	jtar.bodyB = armRight->body;

	jtar.localAnchorA.Set(0.9f * torso->getCorrectedWidth(), 0.3f * torso->getCorrectedHeight());
	jtar.localAnchorB.Set(0, 0.6f * armRight->getCorrectedHeight());

	jtar.collideConnected = false;
	jtar.enableLimit = true;
	jtar.enableMotor = true;
	jtar.maxMotorTorque = 0;
	jtar.motorSpeed = 0;
	jtar.referenceAngle = glm::radians(90.f);
	jtar.lowerAngle = glm::radians(-10.f);
	jtar.upperAngle = glm::radians(10.f);
	world->b2world->CreateJoint(&jtar);

	// left arm
	b2RevoluteJointDef jtal;
	jtal.bodyA = torso->body;
	jtal.bodyB = armLeft->body;

	jtal.localAnchorA.Set(-0.9f * torso->getCorrectedWidth(), 0.3f * torso->getCorrectedHeight());
	jtal.localAnchorB.Set(0, 0.6f * armLeft->getCorrectedHeight());

	jtal.collideConnected = false;
	jtal.enableLimit = true;
	jtal.enableMotor = true;
	jtal.maxMotorTorque = 0;
	jtal.motorSpeed = 0;
	jtal.referenceAngle = glm::radians(-90.f);
	jtal.lowerAngle = glm::radians(-10.f);
	jtal.upperAngle = glm::radians(10.f);
	world->b2world->CreateJoint(&jtal);

	// right hand
	b2WeldJointDef rhrej;
	rhrej.bodyA = armRight->body;
	rhrej.bodyB = handRight->body;
	rhrej.localAnchorA.Set(0.f, -0.9f * armRight->getCorrectedHeight());
	rhrej.localAnchorB.Set(0.f, 0.f);
	rhrej.collideConnected = false;
	rhrej.referenceAngle = glm::radians(0.f);
	world->b2world->CreateJoint(&rhrej);

	// left hand
	b2WeldJointDef lhlej;
	lhlej.bodyA = armLeft->body;
	lhlej.bodyB = handLeft->body;
	lhlej.localAnchorA.Set(0.f, -0.9f * armLeft->getCorrectedHeight());
	lhlej.localAnchorB.Set(0.f, 0.f);
	lhlej.collideConnected = false;
	lhlej.referenceAngle = glm::radians(0.f);
	world->b2world->CreateJoint(&lhlej);

	// flip left side
	armLeft->transform->scale(-1, 1, 1);
	handLeft->transform->scale(-1, 1, 1);
}

void PuppetCharacter::createIndicator(unsigned long _id){
	// headgear

	TextureSampler * tex = PuppetResourceManager::indicators.at(_id);

	indicator = new Box2DSprite(world, tex, b2_dynamicBody, false, nullptr, new Transform(), componentScale);
	
	b2Filter sf;
	sf.categoryBits = 0;
	sf.maskBits = 0;
	sf.groupIndex = groupIndex;
	b2Fixture * f = indicator->createFixture(sf, b2Vec2(0.0f, 0.0f), this);
	f->SetSensor(true);

	/*b2WeldJointDef jhi;
	//b2RevoluteJointDef jhi;
	jhi.bodyA = torso->body;
	jhi.bodyB = indicator->body;
	jhi.localAnchorA.Set(0, -0.f * torso->getCorrectedHeight());
	jhi.localAnchorB.Set(0, 0);
	jhi.collideConnected = false;
	//jhi.enableLimit = true;
	jhi.referenceAngle = 0;
	world->b2world->CreateJoint(&jhi);*/

	indicator->setShader(getShader(), true);
}


void PuppetCharacter::render(vox::MatrixStack* _matrixStack, RenderOptions* _renderOptions){
	// save the current shader settings
	float hue = static_cast<ShaderComponentHsv *>(static_cast<BaseComponentShader *>(_renderOptions->shader)->components.at(1))->getHue();
	float sat = static_cast<ShaderComponentHsv *>(static_cast<BaseComponentShader *>(_renderOptions->shader)->components.at(1))->getSaturation();
	float red = static_cast<ShaderComponentTint *>(static_cast<BaseComponentShader *>(_renderOptions->shader)->components.at(2))->getRed();
	float green = static_cast<ShaderComponentTint *>(static_cast<BaseComponentShader *>(_renderOptions->shader)->components.at(2))->getGreen();
	float blue = static_cast<ShaderComponentTint *>(static_cast<BaseComponentShader *>(_renderOptions->shader)->components.at(2))->getBlue();
	float alpha = static_cast<ShaderComponentAlpha *>(static_cast<BaseComponentShader *>(_renderOptions->shader)->components.at(3))->getAlpha();

	float newHue = hue, newSat = sat;
	if(id == 0){
		newSat = sat + 0.8f;
		newHue = 0.125f;
	}else if(id == 1){
		newHue = 0.3056f;
	}else if(id == 2){
		newHue = 0.64;
		newSat = sat +0.55f;
	}else if(id == 3){
		newHue = 0;
	}
	if(ai){
		newSat = 0.f;
	}

	static_cast<ShaderComponentTint *>(static_cast<BaseComponentShader *>(_renderOptions->shader)->components.at(2))->setRed(red + (1 - control) * 3);
	static_cast<ShaderComponentTint *>(static_cast<BaseComponentShader *>(_renderOptions->shader)->components.at(2))->setGreen(green - (1 - control) * 3);
	static_cast<ShaderComponentTint *>(static_cast<BaseComponentShader *>(_renderOptions->shader)->components.at(2))->setBlue(blue - (1 - control) * 3);


	// change the shader settings based on current damage and player id
	static_cast<ShaderComponentHsv *>(static_cast<BaseComponentShader *>(_renderOptions->shader)->components.at(1))->setHue(newHue);
	static_cast<ShaderComponentHsv *>(static_cast<BaseComponentShader *>(_renderOptions->shader)->components.at(1))->setSaturation(newSat);
	
	if(dead){
		static_cast<ShaderComponentAlpha *>(static_cast<BaseComponentShader *>(_renderOptions->shader)->components.at(3))->setAlpha(0.5f);
	}

	armLeft->render(_matrixStack, _renderOptions);
	armRight->render(_matrixStack, _renderOptions);
	torso->render(_matrixStack, _renderOptions);
	//Box2DSuperSprite::render(_matrixStack, _renderOptions);

	// revert the shader settings
	static_cast<ShaderComponentHsv *>(static_cast<BaseComponentShader *>(_renderOptions->shader)->components.at(1))->setHue(hue);
	static_cast<ShaderComponentHsv *>(static_cast<BaseComponentShader *>(_renderOptions->shader)->components.at(1))->setSaturation(sat);

	head->render(_matrixStack, _renderOptions);
	face->render(_matrixStack, _renderOptions);
	handLeft->render(_matrixStack, _renderOptions);
	handRight->render(_matrixStack, _renderOptions);
	
	// change the shader settings based on current damage and player id
	static_cast<ShaderComponentHsv *>(static_cast<BaseComponentShader *>(_renderOptions->shader)->components.at(1))->setHue(newHue);
	static_cast<ShaderComponentHsv *>(static_cast<BaseComponentShader *>(_renderOptions->shader)->components.at(1))->setSaturation(newSat);

	if(dead){
		static_cast<ShaderComponentAlpha *>(static_cast<BaseComponentShader *>(_renderOptions->shader)->components.at(3))->setAlpha(0.5f);
	}
	headgear->render(_matrixStack, _renderOptions);

	if(indicator != nullptr){
		indicator->render(_matrixStack, _renderOptions);
	}
	// revert the shader settings
	static_cast<ShaderComponentHsv *>(static_cast<BaseComponentShader *>(_renderOptions->shader)->components.at(1))->setHue(hue);
	static_cast<ShaderComponentHsv *>(static_cast<BaseComponentShader *>(_renderOptions->shader)->components.at(1))->setSaturation(sat);
	
	static_cast<ShaderComponentTint *>(static_cast<BaseComponentShader *>(_renderOptions->shader)->components.at(2))->setRed(red);
	static_cast<ShaderComponentTint *>(static_cast<BaseComponentShader *>(_renderOptions->shader)->components.at(2))->setGreen(green);
	static_cast<ShaderComponentTint *>(static_cast<BaseComponentShader *>(_renderOptions->shader)->components.at(2))->setBlue(blue);
	static_cast<ShaderComponentTint *>(static_cast<BaseComponentShader *>(_renderOptions->shader)->components.at(2))->setBlue(blue);
	static_cast<ShaderComponentAlpha *>(static_cast<BaseComponentShader *>(_renderOptions->shader)->components.at(3))->setAlpha(alpha);
}

void PuppetCharacter::update(Step* _step){
	if(canJump){
		for(Box2DSprite ** c : components){
			if(*c != nullptr){
				(*c)->maxVelocity = b2Vec2(10, 10);
			}
		}
	}else{
		for(Box2DSprite ** c : components){
			if(*c != nullptr){
				(*c)->maxVelocity = b2Vec2(10, NO_VELOCITY_LIMIT);
			}
		}
	}
	Box2DSuperSprite::update(_step);
	for(Box2DSprite ** c : components){
		if(*c != nullptr){
			(*c)->update(_step);
		}
	}
    if (control < 0.5f){
        targetRoll = glm::radians(90.f);
        action(true);
    }

	float bodAngle = (rootComponent)->body->GetAngle();
	rootComponent->body->SetTransform(rootComponent->body->GetPosition(), bodAngle);
	float angularVel = -((bodAngle + targetRoll) * control) * 10;// + glm::radians(90.f);
    

	rootComponent->body->SetAngularVelocity(angularVel);

	if(!dead){
		//body
		if((rootComponent->body->GetPosition().y < 0)){
			rootComponent->applyLinearImpulseUp(250);
		}
		if(itemToPickup != nullptr){
			pickupItem(itemToPickup);
		}
	}else{
		if(rootComponent->body->GetPosition().y < ghostPosition) {
			rootComponent->applyForceUp(500.f);
		}else{
			rootComponent->applyForceDown(500.f);
		}
	//	rootComponent->setTranslationPhysical(rootComponent->body->GetPosition().x, ghostPosition, rootComponent->transform->getTranslationVector().z);
		rootComponent->body->ApplyForce(b2Vec2(-bodAngle * 50.0f, 0), rootComponent->body->GetWorldCenter(), true);
	}
	behaviourManager->update(_step);

	control = std::min(1.f, control+0.01f);

	if(health <= 0.0f || deathPending) {
		die();
	}
	if(control >= 0.995f){
		for(Box2DSprite ** c : components){
			if(*c != nullptr){
				b2Filter b = (*c)->body->GetFixtureList()->GetFilterData();
				b.groupIndex = groupIndex;
				(*c)->body->GetFixtureList()->SetFilterData(b);
			}
		}
	}

	if(contactDelegate != nullptr) {
		contactDelegate(this);
		contactDelegate = nullptr;
	}

	// spray some particles to show hits more
	PuppetScene * ps = dynamic_cast<PuppetScene *>(scene);
	if (justTookDamage){
		if (ps != nullptr){
			Particle * p = ps->particleSystem->addParticle(rootComponent->getPos(false));
			p->applyLinearImpulse(vox::NumberUtils::randomFloat(-750, 750), vox::NumberUtils::randomFloat(1000, 1500), p->body->GetPosition().x, p->body->GetPosition().y);
		}
		justTookDamage = false;
	}
	
	// spray some particles to show score
	if(lastUpdateScore < score){
		lastUpdateScore += 1;
		Particle * p = ps->particleSystem->addParticle(rootComponent->getPos(false), PuppetResourceManager::getRandomScoreParticles());
		p->applyLinearImpulse(vox::NumberUtils::randomFloat(-250, 250), vox::NumberUtils::randomFloat(500, 750), p->body->GetPosition().x, p->body->GetPosition().y);
	}

	if(indicator != nullptr){
		indicator->setPos(rootComponent->getPos(false) + glm::vec3(0.f, 3.f, 0.f));
		//indicator->transform->setOrientation(glm::quat(1,0,0,0));
	}
}

void PuppetCharacter::jump(){
	if(canJump){
		//PuppetResourceManager::jumpSounds->playRandomSound();
		//PuppetResourceManager::jumpSounds->playRandomSound();
		float t = rootComponent->body->GetAngle();
		b2Vec2 p = rootComponent->body->GetWorldPoint(b2Vec2(0, 1));
		rootComponent->applyLinearImpulseUp(6000.f *(cos(t)*0.5f + 0.5f));
		if(rootComponent->body->GetAngle() > 0){
			rootComponent->body->SetLinearVelocity(b2Vec2(-80*(1-cos(t)), rootComponent->body->GetLinearVelocity().y));
		}else{
			rootComponent->body->SetLinearVelocity(b2Vec2(80*(1-cos(t)), rootComponent->body->GetLinearVelocity().y));
		}
	}
	//canJump = false;
}

void PuppetCharacter::action(bool _forceDrop){
	if(vox::step.time - lastActionTime > actionThrottle){
		lastActionTime = vox::step.time;
		if(heldItem != nullptr){
			if(itemJoint != nullptr){
                Item * projectile = heldItem->getProjectile(_forceDrop);
				if(projectile == heldItem){
					heldItem = nullptr;
					itemJoint = nullptr;
					itemToPickup = nullptr;
				}
				float t = rootComponent->body->GetAngle();
				projectile->rootComponent->body->SetTransform(projectile->rootComponent->body->GetPosition(), t);
				projectile->rootComponent->applyLinearImpulseUp(50);
				if(rootComponent->body->GetAngle() > 0){
					projectile->rootComponent->applyLinearImpulseLeft(50*(1-cos(t)));
				}else{
					projectile->rootComponent->applyLinearImpulseRight(50*(1-cos(t)));
				}
			}
		}
	}
}

void PuppetCharacter::removeCollision(PuppetGame::BOX2D_CATEGORY _category){
	int idx = -1;
	for(unsigned long int i = 0; i < collisionTypes.size(); ++i) {
		if(collisionTypes.at(i) == _category) {
			idx = i;
			break;
		}
	}
	if(idx >= 0) {
		collisionTypes.erase(collisionTypes.begin() + idx);
	}
}

void PuppetCharacter::addCollision(PuppetGame::BOX2D_CATEGORY _category){
	//if(!isCollidingWith(_category)){
		collisionTypes.push_back(_category);
	//}
}

bool PuppetCharacter::isCollidingWith(PuppetGame::BOX2D_CATEGORY _category){
	for(unsigned long int i = 0; i < collisionTypes.size(); ++i) {
		if(collisionTypes.at(i) == _category) {
			return true;
		}
	}
	return false;
}

void PuppetCharacter::die(){
	dead = true;
	deathPending = false;
	action(true);
	for(Box2DSprite ** c : components){
		(*c)->body->SetGravityScale(0.0f);
		for (b2Fixture* f = (*c)->body->GetFixtureList(); f != nullptr; f = f->GetNext()){
			b2Filter sf = f->GetFilterData();
			if((sf.categoryBits & PuppetGame::kPLAYER) != 0){
				sf.maskBits = PuppetGame::kBOUNDARY;
				f->SetFilterData(sf);
			}
		}
	}
}

void PuppetCharacter::takeDamage(float _damage){
	if(control > 0.75f){
		PuppetResourceManager::hitSounds->playRandomSound();
	}//rootComponent->applyLinearImpulseUp(500);
	health -= _damage;
    control = std::max(0.f, control - 0.05f);

	justTookDamage = true;
}

void PuppetCharacter::unload(){
	Box2DSuperSprite::unload();
}

void PuppetCharacter::load(){
	Box2DSuperSprite::load();
}

void PuppetCharacter::delegateToContactComplete(std::function<void(PuppetCharacter*)> _func){
		contactDelegate = _func;
}

void PuppetCharacter::pickupItem(Item * _item){
	if(_item != heldItem){
		if(heldItem != nullptr){
			action();
		}
		// set the item's group index to match character's so that they won't collide anymore (doesn't work?)
		_item->setGroupIndex(this->groupIndex);

		b2WeldJointDef jd;
		jd.bodyA = itemHolder->body;
		jd.bodyB = (*_item->components.at(0))->body;
		jd.localAnchorA.Set(0.f, -0.9f * itemHolder->getCorrectedHeight());
		jd.localAnchorB.Set(_item->handleX * componentScale, _item->handleY * componentScale);
		jd.collideConnected = false;
		jd.referenceAngle = glm::radians(-90.f);
		jd.dampingRatio = 0;
		itemJoint = _item->playerJoint = (b2WeldJoint *)world->b2world->CreateJoint(&jd);
		heldItem = _item;
		itemToPickup = nullptr;
		_item->held = true;
		_item->owner = this;
	}
}

void PuppetCharacter::setShader(Shader * _shader, bool _configureDefaultAttributes){
	Box2DSuperSprite::setShader(_shader, _configureDefaultAttributes);
	if(itemToPickup != nullptr){
		itemToPickup->setShader(_shader, _configureDefaultAttributes);
	}
}


void PuppetCharacter::translateComponents(glm::vec3 _translationVector){
	Box2DSuperSprite::translateComponents(_translationVector);
	if(heldItem != nullptr){
		heldItem->translateComponents(_translationVector);
	}if(itemToPickup != nullptr){
		itemToPickup->translateComponents(_translationVector);
	}
}