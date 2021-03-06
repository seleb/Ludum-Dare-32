#pragma once

#include <string>
#include <node/Node.h>

class NodeRenderable;

namespace vox{
	class MatrixStack;
}

class RenderOptions;
class Shader;

/********************************************************
*
* An abstract class used to define a shader component
* These components are used with BaseComponentShader
*
* Each component should #define its type so that conditional
* shader compilation can be done depending on whcih components
* are present in BaseComponentShader's component vector
*
*********************************************************/
class ShaderComponent abstract : public Node{
public:

	Shader * shader;

	ShaderComponent(Shader * _shader);
	virtual ~ShaderComponent();

	/**
	* @returns The string containing vertex shader variables
	*/
	virtual std::string getVertexVariablesString() = 0;
	
	/**
	* @returns The String containing the fragment shader variables
	*/
	virtual std::string getFragmentVariablesString() = 0;
	
	/**
	* @returns The String containing the vertex shader body
	*/
	virtual std::string getVertexBodyString() = 0;
	
	/**
	* @returns The String containing the fragment shader body
	*/
	virtual std::string getFragmentBodyString() = 0;
	
	/**
	* @returns The String containing the modification to the 
	* fragment shaders out color. This is GL_OUT_OUT_COLOR in ShaderVariables.h
	*/
	virtual std::string getOutColorMod() = 0;

	virtual void configureUniforms(vox::MatrixStack * _matrixStack, RenderOptions * _renderOption,  NodeRenderable* _nodeRenderable) = 0;
	virtual void clean(vox::MatrixStack * _matrixStack, RenderOptions * _renderOption,  NodeRenderable* _nodeRenderable);
	void makeDirty();
private:
	bool dirty;
};