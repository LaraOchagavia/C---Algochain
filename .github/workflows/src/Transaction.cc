#include "Transaction.h"

Transaction::Transaction()
{
	n_tx_in = "0";
	n_tx_out = "0";
}

Transaction::~Transaction()
{
}

input_t Transaction::get_input(int position) const
{

	return inputs[position];
}

output_t Transaction::get_output(int position) const
{

	return outputs[position];	
}

string Transaction::hash_transaction()
{
	string hash = n_tx_in + "\n";

	for (int i = 0 ; i < stoi (n_tx_in) ; i++)
		hash += inputs[i].tx_id + " " + inputs[i].idx + " " + inputs[i].addr + "\n";

	hash += n_tx_out + "\n";

	int j;
	for (j = 0 ; j < stoi (n_tx_out) -1; j++)
		hash += outputs[j].value + " " + outputs[j].addr + "\n";

	hash += outputs[j].value + " " + outputs[j].addr + "\n";

	return sha256(sha256(hash));	
}

//Recibe un Vector<string> que contiene nombre de destinatario y monto de la transaccion
void Transaction::load_transaction_for_mempool(Vector <string> & transfers, float total_value, float src_value, string tx_id_ , int idx_)
{
 	n_tx_in="1";

 	n_tx_out = to_string(((transfers.position(END_OF_ARGS)-1)/2)+1);
 	inputs[0].tx_id = tx_id_; //cargue tx_id de la transaccion
 	inputs[0].idx = to_string(idx_);
 	inputs[0].addr = sha256(sha256(transfers[0]));

 	int i, j;
 	for (i = 0, j=0 ; j < stoi (n_tx_out)-1 ; i = i + 2, j++) //iterando sobre el vector de outputs
 	{
 		outputs[j].addr = sha256(sha256(transfers[i+1]));
 		outputs[j].value = transfers[i+2];
 	}
	//Para el ultimo Output:
 	outputs[j].addr = inputs[0].addr;
 	outputs[j].value = to_string(src_value-total_value);
}

//Operador para imprimir en output
ostream & operator<<( ostream &output, const Transaction &txn)
{
	output<<txn.n_tx_in<<"\n";
	for (int i = 0; i < std::stoi (txn.n_tx_in); i++)
	{
		output<<txn.inputs[i].tx_id<<" ";
		output<<txn.inputs[i].idx<<" ";
		output<<txn.inputs[i].addr<<"\n";
	}
	output<<txn.n_tx_out<<"\n"; 
	for (int i = 0; i < std::stoi (txn.n_tx_out); i++)
	{
		output<<txn.outputs[i].value<<" ";
		output<<txn.outputs[i].addr<<"\n";
	}
	return output;            
}

//Operador para cargar una transaccion desde input
istream & operator>>( istream &input, Transaction &txn) 
{
	char delim = ' ';
	string aux_str;
	getline(input,aux_str);

	if (is_int(aux_str) == false)
	{
		cerr << "Invalid n_tx_in format: "
		     << aux_str
		     << ". "
		     << endl;
		exit(1);
	}
	txn.set_n_tx_in(aux_str);

	// Carga los inputs:
	Vector <input_t> aux_inputs(stoi(txn.get_n_tx_in()));
	
	for (int i = 0; i < stoi(txn.get_n_tx_in()); i++)
	{
		cout << "Entre al for de cargar inputs en transacciones" << endl;
		getline(input, aux_inputs[i].tx_id, delim);

		if (is_alphanumeric(aux_inputs[i].tx_id) == false || (aux_inputs[i].tx_id).length() != HASH_LENGTH)
		{
			cerr << "Invalid tx_id format: "
		  	   << aux_inputs[i].tx_id
		  	   << ". "
		 	    << endl;
			exit(1);
		}

		getline(input, aux_inputs[i].idx, delim);

		if (is_int(aux_inputs[i].idx) == false)
		{
			cerr << "Invalid idx format: "
		  	   << aux_inputs[i].idx
		  	   << ". "
		 	    << endl;
			exit(1);
		}

		getline(input, aux_inputs[i].addr);

		if (is_alphanumeric(aux_inputs[i].addr) == false || (aux_inputs[i].addr).length() != HASH_LENGTH)
		{
			cerr << "Invalid addr format: "
		  	   << aux_inputs[i].addr
		  	   << ". "
		 	    << endl;
			exit(1);
		}
	}
	txn.set_inputs(aux_inputs);
	// Carga los outputs:
	getline(input,aux_str);
	if (is_int(aux_str) == false)
	{
		cerr << "Invalid n_tx_out format: "
		     << aux_str
		     << ". "
		     << endl;
		exit(1);
	}
	txn.set_n_tx_out(aux_str);
	// Carga el vector de outputs:
	Vector <output_t> aux_outputs(stoi(txn.get_n_tx_out()));

	float total_txn_value=0;
	for (int j = 0; j < stoi (txn.get_n_tx_out()); j++)
	{
		getline(input, aux_outputs[j].value, delim);

		if (is_float_or_double(aux_outputs[j].value) == false)
		{
			cerr << "Invalid value format: "
		    	 << aux_outputs[j].value
		     	<< ". "
		     	<< endl;
			exit(1);
		}
		total_txn_value+=stof(aux_outputs[j].value);

		getline(input, aux_outputs[j].addr);
		if ( is_alphanumeric(aux_outputs[j].addr) == false||(aux_outputs[j].addr).length() != HASH_LENGTH)
		{
			cerr << "Invalid addr format: "
		  		 << aux_outputs[j].addr
		  		 << ". "
		 		 << endl;
			exit(1);
		}
	}
	txn.set_outputs(aux_outputs);

	return input;            
}

void Transaction::load_Transaction(istream * is, float & total_txn_value)
{
	char delim = ' ';

	getline(*is,n_tx_in);//Carga de cantidad de inputs
	if (is_int(n_tx_in) == false)
	{
		cerr << "Invalid n_tx_in format: "
		     << n_tx_in
		     << ". "
		     << endl;
		exit(1);
	}

	Vector <input_t> aux(stoi(n_tx_in));
	inputs = aux;	
	for (int i = 0; i < stoi (n_tx_in); i++)//Carga de los inputs y validacion de formato
	{
		getline(*is, inputs[i].tx_id, delim);
		if (is_alphanumeric(inputs[i].tx_id) == false || (inputs[i].tx_id).length() != HASH_LENGTH)
		{
			cerr << "Invalid tx_id format: "
		  	   << inputs[i].tx_id
		  	   << ". "
		 	    << endl;
			exit(1);
		}

		getline(*is, inputs[i].idx, delim);
		if (is_int(inputs[i].idx) == false)
		{
			cerr << "Invalid idx format: "
		  	   << inputs[i].idx
		  	   << ". "
		 	    << endl;
			exit(1);
		}

		getline(*is, inputs[i].addr);
		if (is_alphanumeric(inputs[i].addr) == false || (inputs[i].addr).length() != HASH_LENGTH)
		{
			cerr << "Invalid addr format: "
		  	   << inputs[i].addr
		  	   << ". "
		 	    << endl;
			exit(1);
		}
	} 
	
	getline(*is,n_tx_out); //Carga de cantidad de outputs
	if (is_int(n_tx_out) == false)
	{
		cerr << "Invalid n_tx_out format: "
		     << n_tx_out
		     << ". "
		     << endl;
		exit(1);
	}
	
	Vector <output_t> aux1(stoi(n_tx_out));
	outputs = aux1;
	
	total_txn_value=0;
	for (int j = 0; j < stoi (n_tx_out); j++)//Carga de los outputs y validacion de formato
	{
		getline(*is, outputs[j].value, delim);
		if (is_float_or_double(outputs[j].value) == false)
		{
			cerr << "Invalid value format: "
		    	 << outputs[j].value
		     	<< ". "
		     	<< endl;
			exit(1);
		}

		total_txn_value+=stof(outputs[j].value);

		getline(*is, outputs[j].addr);
		if ( is_alphanumeric(outputs[j].addr) == false||(outputs[j].addr).length() != HASH_LENGTH)
		{
			cerr << "Invalid addr format: "
		  		 << outputs[j].addr
		  		 << ". "
		 		 << endl;
			exit(1);
		}
	}
}
