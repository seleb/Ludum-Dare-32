#pragma once

#include "Node.h"

#include <vector>

class NodeChild;
class SceneRoot;

class NodeParent abstract : public virtual Node{
public:
	virtual ~NodeParent() = 0;

	// Recursively delete all the children of a node, then delete the node itself
	static void deleteRecursively(NodeParent * _j);

	// Loops through the node's children and removes the first instance of _child
	// Pushes _child onto the node's children stack
	// Sets _child->parent = this node
	// Returns whether or not the child was successfully added
	virtual bool addChild(NodeChild * _child);

	// Inserts _child into this node's list of children at _index
	// Note: does not make any checks, so cyclical references and what not are possible
	virtual void addChildAtIndex(NodeChild * _child, int _index);

	// Erases the node at _index from the list of children
	virtual void removeChildAtIndex(int _index);
	
	// Loops through the node's children and removes the first instance of _child and returns the index
	// Does NOT change node->parent
	// Returns (unsigned long int)(-1) if _child is not a child of this node
	virtual unsigned long int removeChild(NodeChild * _child);


	/** List of references to child entities */
	std::vector<NodeChild *> children;


private:

};