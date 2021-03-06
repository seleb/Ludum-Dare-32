#pragma once

#include "MeshEntity.h"
#include <Box2D/Box2D.h>
#include <node/NodeResource.h>

class BitmapFont;
class Scene;
class LayeredScene;
class Shader;
class Box2DWorld;
class Box2DSprite;
class CharacterComponent;
class Texture;
class TextureSampler;

class Box2DSuperSprite : public MeshEntity {
public:
	
	static int16 gGroupIndex;
	int16 groupIndex;
	int16 categoryBits; // Box2D entity category
	int16 maskBits;		// Box2D collides only with
	float componentScale;
	
	Box2DWorld * world;
	Scene * scene;
	signed long int sceneLayer;

	std::vector<Box2DSprite **> components;

	// Component to which others are relative (ex: character torso, item handle)
	Box2DSprite * rootComponent;

	explicit Box2DSuperSprite(Box2DWorld * _world, int16 _categoryBits, int16 _maskBits = -1, int16 _groupIndex = 0);
	
	~Box2DSuperSprite();

	virtual void render(vox::MatrixStack* _matrixStack, RenderOptions* _renderOptions) override;
	virtual void update(Step* _step) override;

	virtual void setShader(Shader * _shader, bool _configureDefaultVertexAttributes) override;
	
	virtual void addToScene(Scene * _scene);

	virtual void addToLayeredScene(LayeredScene * _scene, unsigned long int _layer);
	
	virtual void translateComponents(glm::vec3 _translateVector);
	virtual void translateComponents(float _x, float _y, float _z);
	void setUserData(void * _data);

	// Sets the group index on all components
	void setGroupIndex(int16 _groupIndex);

	virtual void addComponent(Box2DSprite * _component);

	// Can't use this with the way we have things set up, things like the catapult arm and puppet hand don't store their actual world position since they're positioned with joints
	void snapComponents(Box2DSprite * _sprite);
	
	virtual void load() override;
	virtual void unload() override;
};