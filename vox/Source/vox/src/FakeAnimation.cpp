#pragma once

#include "FakeAnimation.h"
#include "Resource.h"
#include "Entity.h"
#include "MatrixStack.h"
#include "shader/Shader.h"
#include "Step.h"

void FakeAnimation::pushFrame(MeshEntity * _frame){
	_frame->transform = this->transform;
	_frame->setShader(this->getShader(), false);
	_frame->parent = nullptr;
	_frame->load();
	children.push_back(_frame);
}

FakeAnimation::FakeAnimation(Transform * _transform, Shader * _shader) :
	NodeTransformable(_transform),
	NodeChild(nullptr),
	MeshEntity(nullptr, _transform, _shader),
	frame(0),
	delay(3),
	delayCount(0)
{
}

FakeAnimation::~FakeAnimation(){
	for(unsigned long int i = 0; i < children.size(); ++i){
		delete children.at(i);
	}
	children.clear();

	delete transform;
	delete getShader();
}

void FakeAnimation::render(vox::MatrixStack * _matrixStack, RenderOptions * _renderOptions){
	//push transform
	_matrixStack->pushMatrix();
	_matrixStack->applyMatrix(transform->getModelMatrix());

	//for(Entity * child : children){
		dynamic_cast<Entity *>(children.at(frame))->render(_matrixStack, _renderOptions);
	//}

	//pop transform
	_matrixStack->popMatrix();
}

void FakeAnimation::update(Step * _step){
	delayCount += 1;
	if(delayCount >= delay){
		delayCount = 0;
		frame += 1;
	}
	if(frame >= children.size()){
		frame = 0;
	}
}