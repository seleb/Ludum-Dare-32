#pragma once

#include "Transform.h"

Transform::Transform():
	translationVector(0.f, 0.f, 0.f),
	scaleVector(1.f, 1.f, 1.f),
	orientation(1.f, 0.f, 0.f, 0.f),
	tDirty(true),
	sDirty(true),
	oDirty(true),
	mDirty(true)
{
}

Transform::~Transform(){
}

void Transform::scale(float _scaleX, float _scaleY, float _scaleZ, bool _relative){
	scale(glm::vec3(_scaleX, _scaleY, _scaleZ), _relative);
}

void Transform::scale(glm::vec3 _scale, bool relative){
	if(relative){
		scaleVector *= _scale;
	}else{
		scaleVector = _scale;
	}
	sDirty = true;
	mDirty = true;
}

void Transform::translate(float _translateX, float _translateY, float _translateZ, bool _relative){
	translate(glm::vec3(_translateX, _translateY, _translateZ), _relative);
}

void Transform::translate(glm::vec3 _translate, bool _relative){
	if(_relative){
		translationVector += _translate;
	}else{
		translationVector = _translate;
	}
	tDirty = true;
	mDirty = true;
}

void Transform::rotate(glm::quat _rotation, CoordinateSpace _space){
	switch(_space){
	case kWORLD:
		setOrientation(_rotation * orientation);
		break;
	case kOBJECT:
		setOrientation(orientation * _rotation);
		break;
	}
}

void Transform::rotate(float _angle, float _x, float _y, float _z, CoordinateSpace _space){
	rotate(glm::quat(glm::angleAxis(_angle, glm::vec3(_x, _y, _z))), _space);
}
void Transform::setOrientation(glm::quat _orientation){
	orientation = _orientation;
	oDirty = true;
	mDirty = true;
}

glm::mat4 Transform::getTranslationMatrix(){
	if(tDirty){
		tMatrix = glm::translate(translationVector);
		tDirty = false;
	}
	return tMatrix;
}

glm::mat4 Transform::getScaleMatrix(){
	if(sDirty){
		sMatrix = glm::scale(scaleVector);
		sDirty = false;
	}
	return sMatrix;
}

glm::mat4 Transform::getOrientationMatrix(){
	if(oDirty){
		oMatrix = glm::toMat4(orientation);
		oDirty = false;
	}
	return oMatrix;
}


glm::mat4 Transform::getModelMatrix(){
	if(mDirty){
		mMatrix = getTranslationMatrix() * getOrientationMatrix() * getScaleMatrix();
		mDirty = false;
	}
	return mMatrix;
}

void Transform::reset(){
	translate(glm::vec3(0.f, 0.f, 0.f), false);
	scale(glm::vec3(1.f, 1.f, 1.f), false);
	setOrientation(glm::quat(1.f, 0.f, 0.f, 0.f));
}

glm::vec3 Transform::getTranslationVector(){
	return translationVector;
}
glm::vec3 Transform::getScaleVector(){
	return scaleVector;
}
glm::quat Transform::getOrientationQuat(){
	return orientation;
}