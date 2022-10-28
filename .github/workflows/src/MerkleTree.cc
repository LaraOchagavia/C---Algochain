#include "MerkleTree.h"

MerkleTree::MerkleTree(Vector <TreeNode *> leaves)
{
	//leaves es el Vector que contiene los punteros a TreeNode que contienen los hashes de las transacciones
	if(leaves.get_size() == 0)
	{
		cerr << "FAIL: Not able to calculate txns_hash."
			 << "\n";
		exit(1);
	}
	TreeNode * p = NULL;
	while(leaves.get_size() != 1)
	{	
		Vector <TreeNode *> nodes(leaves.get_size()/2 + leaves.get_size()%2); //Se crea el nivel de arriba del arbol	
		
		for( int i = 0, n = 0; i < leaves.get_size(); i = i+2, n++)
		{			
			if( i != leaves.get_size() -1 )//Se fija si quedan elementos por analizar en el Vector por si es impar
			{
				nodes[n] = new TreeNode( sha256(sha256( leaves[i]->get_hash() + leaves[i+1]->get_hash() )));
				nodes[n]->set_left(leaves[i]);
				nodes[n]->set_rigth(leaves[i+1]);
			}
			else //En caso de quedar un elemento solo, se hace la suma del hash de ese elemento consigo mismo para cargar al padre
			{
				nodes[n] = new TreeNode(sha256(sha256( leaves[i]->get_hash() + leaves[i]->get_hash())));
				nodes[n]->set_left(leaves[i]);
				nodes[n]->set_rigth(p);
			}
		}
		leaves = nodes;
	}
	root = leaves[0];
}

MerkleTree::~MerkleTree()
{
	
	deleteMerkleTree(root);
}

void MerkleTree::deleteMerkleTree(TreeNode * father)
{
	if (father)//Si es distinto de nulo entra
	{
		deleteMerkleTree(father->get_left());
		deleteMerkleTree(father->get_rigth());
		delete father;
		father = NULL;
	}
}
