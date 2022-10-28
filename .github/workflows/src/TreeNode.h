#ifndef TREENODE_INCLUDED
#define TREENODE_INCLUDED

#include "MerkleTree.h"
#include "Vector.h"

using namespace std;

class TreeNode
{
	string hash;
	TreeNode * left;
	TreeNode * rigth;

public:
	TreeNode(string data){hash = data;};
	~TreeNode(){};

	string get_hash(){return hash;};
	TreeNode * get_left(){return left;};
	TreeNode * get_rigth(){return rigth;};
	void set_left(TreeNode * left_){left = left_;};
	void set_rigth(TreeNode * rigth_){rigth = rigth_;};
	
};

#endif
