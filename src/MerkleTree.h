#ifndef MERKLE_TREE_INCLUDED
#define MERKLE_TREE_INCLUDED

#include <string>
#include <iostream>

#include "Vector.h"
#include "TreeNode.h"
#include "sha256.h"

using namespace std;

class MerkleTree
{
private:
	TreeNode * root;
	void deleteMerkleTree(TreeNode *);

public:
	MerkleTree(Vector <TreeNode *>);
	~MerkleTree();
	TreeNode *get_root(){return root;};
};

#endif
