#ifndef _BLOCK_H_INCLUDED_
#define _BLOCK_H_INCLUDED_

#include "Transaction.h"
#include "Vector.h"
#include "sha256.h"
#include "cmdline.h"
#include "Utilities.h"
#include "List.h"
#include "main.h"

#include "MerkleTree.h"

#include <cstddef>
#include <bitset>
#include <time.h>
#include <string>
#include <string.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>
#include <cstring>
#include <cmath>    
#include <unordered_map>   

using namespace std;

#define PREV_BLOCK_GENESIS "0000000000000000000000000000000000000000000000000000000000000000"
#define NONCE_MIN 10000
#define NONCE_MAX 2000000
#define HASH_LENGTH 64

struct header_t
{
	string prev_block;
	string txns_hash;
	int bits; // Dificultad
	int nonce; // Para el hash
};

struct body_t
{
	int txns_count;
	Vector<Transaction> txns;
};

class Block
{
private:
	header_t * header;
	body_t * body;

public:
	Block();
	Block(const Block &);
	~Block();
	void load_block(string);
	void load_Genesis(string &,float, int);
	bool is_genesis();

	string get_prev_block()const { return header->prev_block;};
	string get_txns_hash()const {return header->txns_hash;};
	int get_bits()const {return header->bits;}; 
	int get_nonce()const {return header->nonce;};
	int get_txns_count()const {return body->txns_count;};
	Vector<Transaction> get_txns()const {return body->txns;};

	void set_prev_block(string value){header->prev_block=value;};
	void set_txns_hash(string value){header->txns_hash=value;};
	void set_bits(int value){header->bits=value;};
	void set_nonce(int value){header->nonce=value;};

	void set_txns_count(int value){body->txns_count=value;};
	void set_txns(Vector<Transaction> value){body->txns=value;};

	bool is_valid_hash_header(int dif);
	void check_hash_header(int dif);
	string hash_Block();
	void load_wallet(Transaction &);

	friend ostream & operator<<( ostream &, Block &);
	friend istream & operator>>( istream &, Block &);
};

extern list<Block> *Algochain;
extern list<Transaction> *mempool;
extern list<string> *hashes_transactions;
extern list<string> *hashes_blocks;

extern istream *iss;	// Input Stream (clase para manejo de los flujos de entrada)
extern ostream *oss;	// Output Stream (clase para manejo de los flujos de salida)
extern fstream ifs; 	// Input File Stream (derivada de la clase ifstream que deriva de istream para el manejo de archivos)
extern fstream ofs;		// Output File Stream (derivada de la clase ofstream que deriva de ostream para el manejo de archivos)

#endif



