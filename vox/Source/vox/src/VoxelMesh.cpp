#pragma once

#include "VoxelMesh.h"
#include "node/NodeRenderable.h"

VoxelMesh::VoxelMesh(GLenum _drawMode):
	MeshInterface(GL_POINTS, _drawMode),
	NodeRenderable(),
	NodeResource(true),
	resolution(0.15f)
{
}

VoxelMesh::VoxelMesh(MeshInterface* _mesh):
	MeshInterface(GL_POINTS, _mesh->drawMode),
	NodeResource(true),
	resolution(0.15f){
	indices  = _mesh->indices;
	vertices = _mesh->vertices;
	unload();
	load();
}

VoxelMesh::~VoxelMesh(){
}

void VoxelMesh::pushPoint(GLint _vertex){
	indices.push_back(_vertex);
}
