#pragma once

#include <PuppetCharacter.h>

class PuppetTexturePack;

class PuppetCharacterArcher : public PuppetCharacter{
public:

	PuppetCharacterArcher(bool _ai, unsigned long int _id, Box2DWorld * _world, int16 _categoryBits, int16 _maskBits = -1, int16 _groupIndex = 0);
	~PuppetCharacterArcher();
};