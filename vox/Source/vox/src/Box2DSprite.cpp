#pragma once

#include "Box2DSprite.h"
#include "Box2DWorld.h"
#include "Texture.h"
#include <TextureSampler.h>
#include "MeshInterface.h"

Box2DSprite::Box2DSprite(Box2DWorld * _world, TextureSampler * _textureSampler, b2BodyType _bodyType, bool _defaultFixture, Shader* _shader, Transform* _transform, float _componentScale) :
	NodeTransformable(_transform),
	NodeChild(nullptr),
	NodeBox2DBody(_world, _bodyType, _defaultFixture, _transform),
	width(_textureSampler->width),
	height(_textureSampler->height),
	scale(_componentScale),
	u(_textureSampler->u),
	v(_textureSampler->v)
{
	if(_textureSampler->texture != nullptr){
		mesh->pushTexture2D(_textureSampler->texture);
	}

	body->SetUserData(this);
}
Box2DSprite::Box2DSprite(Box2DWorld * _world, b2BodyType _bodyType, bool _defaultFixture, Shader* _shader, Transform* _transform, Texture * _texture, float _width, float _height, float _u, float _v, float _componentScale):
	NodeTransformable(_transform),
	NodeChild(nullptr),
	NodeBox2DBody(_world, _bodyType, _defaultFixture, _transform),
	width(_width),
	height(_height),
	scale(_componentScale),
	u(_u),
	v(_v)
{
	if(_texture != nullptr){
		mesh->pushTexture2D(_texture);
	}

	body->SetUserData(this);
}

float Box2DSprite::getCorrectedHeight(){
	return height*std::abs(transform->scaleVector.y)*scale*2.f;
}
float Box2DSprite::getCorrectedWidth(){
	return width*std::abs(transform->scaleVector.x)*scale*2.f;
}

void Box2DSprite::createFixture(b2Filter _filter, b2Vec2 _offset, void * _userData){
	b2PolygonShape tShape;
	tShape.SetAsBox(width*std::abs(transform->scaleVector.x)*scale*2.f, std::abs(height*transform->scaleVector.y)*scale*2.f, _offset, 0.0f);

	b2FixtureDef fd;
	fd.shape = &tShape;
	fd.restitution = 0;
	fd.friction = 0.5;
	fd.isSensor = false;
	fd.density = 1;
	fd.userData = _userData;
	fd.filter = _filter;

	b2Fixture * f = body->CreateFixture(&fd);

	// physical fixture
	//b2Fixture * f = body->CreateFixture(&tShape, 1); // physical fixture
	//..f->SetFilterData(_filter);
	//f->SetUserData(_userData);

	b2Vec2 v1 = tShape.GetVertex(0);
	b2Vec2 v2 = tShape.GetVertex(1);
	b2Vec2 v3 = tShape.GetVertex(2);
	b2Vec2 v4 = tShape.GetVertex(3);

	mesh->vertices.at(0).x = v1.x;
	mesh->vertices.at(0).y = v1.y;
	mesh->vertices.at(1).x = v2.x;
	mesh->vertices.at(1).y = v2.y;
	mesh->vertices.at(2).x = v3.x;
	mesh->vertices.at(2).y = v3.y;
	mesh->vertices.at(3).x = v4.x;
	mesh->vertices.at(3).y = v4.y;
	
	float mag = std::max(mesh->textures.at(0)->width, mesh->textures.at(0)->height);
	mesh->vertices.at(3).u = u/mag;
	mesh->vertices.at(3).v = v/mag;
	mesh->vertices.at(2).u = (u + width)/mag;
	mesh->vertices.at(2).v = v/mag;
	mesh->vertices.at(1).u = (u + width)/mag;
	mesh->vertices.at(1).v = (v + height)/mag;
	mesh->vertices.at(0).u = u/mag;
	mesh->vertices.at(0).v = (v + height)/mag;
}


// shouldn't this dereference the fixture and just return its shape?
b2PolygonShape Box2DSprite::getFixtureShape(){
	b2PolygonShape tShape;
	tShape.SetAsBox(width*std::abs(transform->scaleVector.x)*scale*2.f, std::abs(height*transform->scaleVector.y)*scale*2.f);
	return tShape;
}

Box2DSprite::~Box2DSprite(){
	if(world != nullptr && body != nullptr) {
		world->b2world->DestroyBody(body);
		body = nullptr;
	}
}

void Box2DSprite::update(Step * _step){
	NodeBox2DBody::update(_step);
	Sprite::update(_step);
}

void Box2DSprite::setGroupIndex(int16 _groupIndex){
	b2Fixture * f = body->GetFixtureList();
	while(f != nullptr){
		b2Filter bf = f->GetFilterData();
		bf.groupIndex = _groupIndex;
		f->SetFilterData(bf);
		f->Refilter(); // is this necessary?
		f = f->GetNext();
	}
}