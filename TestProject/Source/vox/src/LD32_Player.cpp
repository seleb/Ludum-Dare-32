#pragma once

#include <LD32_Player.h>
#include <MeshInterface.h>
#include <Box2DWorld.h>
#include <Material.h>
#include <MeshFactory.h>
LD32_Player::LD32_Player(Box2DWorld * _world) :
	Box2DMeshEntity(_world, MeshFactory::getCubeMesh(), b2_dynamicBody, false),
	NodeChild(nullptr),
	NodeTransformable(new Transform()),
	playerMat(new Material(15, glm::vec3(1,1,1), true))
{
	b2Filter sf;
	sf.groupIndex = -1;
	transform->scale(1.f, 1.f, 1.f);
	mesh->pushMaterial(playerMat);
	mesh->dirty = true;
	world->addToWorld(this);
	b2Fixture * f = createFixture();
	f->SetFilterData(sf);

	for (unsigned long int i = 0; i < 4; ++i){

	Box2DMeshEntity * thing1 = new Box2DMeshEntity(world, MeshFactory::getCubeMesh(), b2_dynamicBody, false);
	thing1->transform->scale(1.f, 1.f, 1.f);
	thing1->mesh->pushMaterial(playerMat);
	thing1->mesh->dirty = true;
	world->addToWorld(thing1);
	f = thing1->createFixture();
	f->SetDensity(0.1f);
	f->SetFilterData(sf);
	things.push_back(thing1);

	b2PrismaticJointDef j;
	j.bodyA = body;
	j.bodyB = thing1->body;
	j.collideConnected = false;
	j.enableLimit = true;
	j.enableMotor = true;
	switch (i){
	case 0:
		j.localAxisA = b2Vec2(0, 1);
		break;
	case 1:
		j.localAxisA = b2Vec2(1, 0);
		break;
	case 2:
		j.localAxisA = b2Vec2(0, -1);
		break;
	case 3:
		j.localAxisA = b2Vec2(-1, 0);
		break;
	}
	j.lowerTranslation = 0;
	j.upperTranslation = 1;
	j.motorSpeed = -1;
	j.maxMotorForce = 10000;
	joints.push_back(dynamic_cast<b2PrismaticJoint *>(world->b2world->CreateJoint(&j)));
	}
}

void LD32_Player::update(Step * _step){
	Box2DMeshEntity::update(_step);
	for (auto t : things){
		t->update(_step);
	}
}

void LD32_Player::render(vox::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	Box2DMeshEntity::render(_matrixStack, _renderOptions);
	for (auto t : things){
		t->render(_matrixStack, _renderOptions);
	}
}

void LD32_Player::setShader(Shader * _shader, bool _configureDefaultAttributes){
	Box2DMeshEntity::setShader(_shader, _configureDefaultAttributes);
	for (auto t : things){
		t->setShader(_shader, _configureDefaultAttributes);
	}
}