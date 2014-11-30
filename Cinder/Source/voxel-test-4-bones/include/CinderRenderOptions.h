#pragma once 

#include "RenderOptions.h"
#include <cinder\gl\GlslProg.h>

#include <vector>
//#include "GL/glew.h"
#include <glm/glm.hpp>

#include <cinder\Camera.h>

class Shader;
class Light;

class CinderRenderOptions : public RenderOptions{
	
public:

	CinderRenderOptions(Shader * _shader, std::vector<Light *> * _lights, Shader * _overrideShader = nullptr);

	~CinderRenderOptions();

	const ci::Camera * ciCam;
	ci::gl::GlslProg * ciShader;
};