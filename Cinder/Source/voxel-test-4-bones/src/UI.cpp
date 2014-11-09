#pragma once

#include "UI.h"
<<<<<<< HEAD
#include "Easing.h"
#include "Joint.h"

std::vector<Node *> UI::selectedNodes;

float UI::time;
Easing::Type UI::interpolation;

ci::Vec3d UI::handlePos = ci::Vec3d(0, 0, 0);

void UI::updateHandlePos(){
	handlePos.set(0,0,0);
	for(unsigned long int i = 0; i < UI::selectedNodes.size(); ++i){
		handlePos += (dynamic_cast<Joint *>(UI::selectedNodes.at(i)))->getPos(false);
	}
	handlePos /= UI::selectedNodes.size();
}
