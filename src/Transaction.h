#ifndef _TRANSACTION_H_INCLUDED_
#define _TRANSACTION_H_INCLUDED_
#include "Vector.h"
#include "cmdline.h"
#include "Utilities.h"
#include "sha256.h"

#include <cstring>
#include <ostream>
#include <string>
#include <string.h>
#include <string_view>
#include <iostream>
#include <istream>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <cstdlib>

#define HASH_LENGTH 64

using namespace std;

struct input_t {

	string tx_id;
	string idx;
	string addr; 
};

struct output_t {

	string value;
	string addr; 
};


class Transaction 
{
private:
	string n_tx_in;
	string n_tx_out;

	Vector <input_t> inputs;
	Vector <output_t> outputs;

public:
	Transaction();
	~Transaction();
	void load_transaction_for_mempool(Vector <string> &, float , float, string, int);
	void load_Transaction(istream *, float &);
	string get_n_tx_in()const{return n_tx_in;};
	string get_n_tx_out()const{return n_tx_out;};
	input_t get_input(int)const;
	output_t get_output(int)const;

	void set_n_tx_in(string value){n_tx_in = value;};
	void set_n_tx_out(string value){n_tx_out = value;};
	void set_inputs(Vector <input_t> value){inputs=value;};
	void set_outputs(Vector <output_t> value){outputs=value;};
	void set_input(string txid_, string idx_, string addr_, int pos){inputs[pos].tx_id = txid_; inputs[pos].idx = idx_ ;inputs[pos].addr = addr_;};
	void set_output(string val, string addr_, int pos){outputs[pos].value = val ; outputs[pos].addr = addr_ ; };
	string hash_transaction();
	
	friend ostream & operator<<( ostream &, const Transaction &txn);
	friend istream & operator>>( istream &, Transaction &);
};

#include <unordered_map> 
extern unordered_map < string, Vector<string> > wallet;

#endif
