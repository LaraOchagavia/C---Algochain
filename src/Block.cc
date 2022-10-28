#include "Block.h"

Block::Block()
{
	header = new header_t;
    body = new body_t;
}

Block::Block(const Block &bl)
{
	header = new header_t;
	body = new body_t;

	*header = *bl.header;
	*body = *bl.body;
}


Block::~Block()
{
	delete header;
	delete body;
}

bool Block::is_genesis()
{
	if(header->prev_block == PREV_BLOCK_GENESIS)	
	{
		if( (body->txns)[0].get_input(0).tx_id !=PREV_BLOCK_GENESIS)
			return false;

		if( (body->txns)[0].get_input(0).idx != "0")
			return false;

		if( (body->txns)[0].get_input(0).addr != PREV_BLOCK_GENESIS)
			return false;
		
		return true;
	}
	return false;
}


bool Block::is_valid_hash_header(int dif)
{
	string hash_header;
	hash_header = header->prev_block + "\n"+ header->txns_hash + "\n" + std::to_string(header->bits)+ "\n" + std::to_string(header->nonce) + "\n";
	string h = sha256(sha256(hash_header));

	//Parte de division:
	int position=dif/4;		// Nro de caracteres -----> Valor de dividendo
	int rest=dif%4;

	srand(time(NULL));	//Se usa para generar una semilla para generar numeros aleatorios

	string test(h);	//Genera una cadena auxiliar con el valor de h

	test.resize(position+1); 	//Genera una cadena de n bits a testear donde cada caracter determinado por position son 4 bits 

	test=Hex2Bin(test ); //Queda guardado el numero en binario

	string	string_zeros(position*4+rest, '0');	/*Forma una cadena de "0" igual a la cantidad de bits cero que necesita
													  Position*4 porque position lo que cuenta son 4 bits
													  Rest son bits*/

	int comparation=(test.compare(0, string_zeros.length(), string_zeros));
	
	if(comparation != 0)
		return false;
	else
		return true;
}

void Block::check_hash_header(int dif)
{
	string hash_header;
	hash_header = header->prev_block + "\n"+ header->txns_hash + "\n" + std::to_string(header->bits)+ "\n" + std::to_string(header->nonce)+ "\n";
	string h = sha256(sha256(hash_header));

	//Parte de division:
	int position=dif/4;		// Nro de caracteres -----> Valor de dividendo
	int rest=dif%4;

	srand(time(NULL));	//Se usa para generar una semilla para generar numeros aleatorios

	std::string test(h);	//Genera una cadena auxiliar con el valor de h

	test.resize(position+1); 	//Genera una cadena de n bits a testear donde cada caracter determinado por position son 4 bits 

	test=Hex2Bin(test ); //Queda guardado el numero en binario

	std::string	string_zeros(position*4+rest, '0');	/*Forma una cadena de "0" igual a la cantidad de bits cero que necesita
													  Position*4 porque position lo que cuenta son 4 bits
													  Rest son bits*/

	int comparation=(test.compare(0, string_zeros.length(), string_zeros));

	while( comparation != 0)
	{				
		header->nonce = rand() % NONCE_MAX + NONCE_MIN;		//Genera un nonce aleatorio

		hash_header = header->prev_block + "\n"+ header->txns_hash+ "\n"+ std::to_string(header->bits)+ "\n" + std::to_string(header->nonce)+ "\n";
		h = sha256(sha256(hash_header));

		string test(h);
		test.resize(position+1);
			
		test=Hex2Bin(test);

		comparation=(test.compare(0, string_zeros.length(), string_zeros));
	}
}

void Block::load_block(string dif)
{
	//Carga el body:
	body->txns_count = mempool->size();
	list<Transaction>::iterator mem = mempool->first();

	Vector <TreeNode *> txns_hashes(body->txns_count); //Vector de hojas del arbol de Merkle
	TreeNode *p = NULL;
	//Se itera sobre cada transaccion de la mempool
	for (int i = 0 ; i < body->txns_count ; i++)
	{
		//Se duplica memoria del vector transacciones si no hay mas espacio
		if (body->txns_count == (body->txns).get_size())
		{
			Vector <Transaction> aux(2*((body->txns).get_size()), body->txns);
			body->txns = aux;
		}

		//Se incluye la transaccion en el vector de transacciones
		(body->txns)[i] = (mem.data());

		txns_hashes[i] = new TreeNode((body->txns)[i].hash_transaction());
		txns_hashes[i]->set_left(p);
		txns_hashes[i]->set_rigth(p);

		mem.go_forward();
	}

	header->prev_block = ((Algochain->last()).data()).hash_Block(); 

	MerkleTree *Tree = new MerkleTree(txns_hashes);
	header->txns_hash = (Tree->get_root())->get_hash();

	delete Tree;
	
	header->bits = stoi(dif);

	header->nonce = NONCE_MIN;

	check_hash_header(stoi(dif));
}

void Block::load_Genesis(string & user, float value, int dif)
{
	body->txns_count = 1;
	(body->txns)[0].set_n_tx_in("1");
	(body->txns)[0].set_n_tx_out("1");

	(body->txns)[0].set_input(PREV_BLOCK_GENESIS, "0", PREV_BLOCK_GENESIS, 0);
	(body->txns)[0].set_output(to_string(value), sha256(sha256(user)), 0);

	header->prev_block = PREV_BLOCK_GENESIS;
	
	Vector <TreeNode*> txns_hashes(body->txns_count); //Vector de hojas del arbol de Merkle
	txns_hashes[0] = new TreeNode((body->txns)[0].hash_transaction());//Carga las hojas

	TreeNode * p = NULL;
	txns_hashes[0]->set_left(p);
	txns_hashes[0]->set_rigth(p);

	
	MerkleTree * hashTree = new MerkleTree(txns_hashes);
	header->txns_hash = (hashTree->get_root())->get_hash();

	delete hashTree;
		
	header->bits = dif;
	header->nonce = NONCE_MIN;

	check_hash_header(dif);

	//Se carga el Vector<string> de la wallet con el value, hash transferencia y el idx 
	Vector<string> aux(3);
	aux[0] = to_string(value);	
	aux[1] = (body->txns)[0].hash_transaction();
	aux[2] = "0";

	wallet.insert( {sha256(sha256(user)), aux} );
}


string Block::hash_Block()
{
	string hash_block = "";
	string header_txnshash = "";

	for (int i = 0 ; i < body->txns_count ; i++)
	{
		header_txnshash += (body->txns)[i].get_n_tx_in() + "\n"; 

		for(int j = 0; j < (stoi((body->txns)[i].get_n_tx_in())); j++)//Cant de inputs
		{	
			header_txnshash += ((body->txns)[i]).get_input(j).tx_id + " ";
			header_txnshash += ((body->txns)[i]).get_input(j).idx + " ";
			header_txnshash += ((body->txns)[i]).get_input(j).addr + "\n";
		}
	
		header_txnshash += (body->txns)[i].get_n_tx_out() + "\n";
	
		for(int j = 0; j < (stoi((body->txns)[i].get_n_tx_out())); j++)//Cant de outputs
		{	
			header_txnshash += ((body->txns)[i]).get_output(j).value + " ";
			header_txnshash += ((body->txns)[i]).get_output(j).addr + "\n";
		}
	}

	hash_block = header->prev_block +"\n"+ header->txns_hash +"\n"+ to_string(header->bits) +"\n"+ to_string(header->nonce) +"\n"+ to_string(body->txns_count) +"\n"+ header_txnshash +"\n";

	return sha256(sha256(hash_block));
}


void Block::load_wallet(Transaction & txn)
{
    //Al usuario que aparece en el INPUT de la transaccion: se le tiene que poner el valor del ultimo output de esta transaccion

    // Cuando aparece en el input hay que dejarle el vuelto en el value
    (wallet.find( (txn.get_input(0)).addr )->second)[0] = txn.get_output(stoi( txn.get_n_tx_out() ) -1 ).value;
    
    //Actualizar la segunda posicion del vector con el hash de la transaccion actual 
    (wallet.find( (txn.get_input(0)).addr )->second)[1] = txn.hash_transaction();
    
    //Actualizar la ultima posicion del vector con la posicion del ULTIMO output de esta transaccion
    (wallet.find( (txn.get_input(0)).addr )->second)[2] =  to_string( stoi(txn.get_n_tx_out()) - 1 );

    //Cargar en la wallet los fondos que se correspondan con los user que aparecen solo en las outputs:
    //Se itera sobre los outputs de esta transaccion
    //Se recorre hasta el anteultimo output porque el ultimo tiene el vuelto del user que aparece en el input
    float total_value;

    for (int j = 0; j < stoi(txn.get_n_tx_out()) -1 ; j++)
    {
        if( (wallet.count( (txn.get_output(j)).addr ) == 1) )
        {
            //Ya esta en la wallet	
            total_value = stof((wallet.find( (txn.get_output(j)).addr )->second)[0])+ stof( txn.get_output(j).value );
            
            (wallet.find( (txn.get_output(j)).addr )->second)[0] = to_string(total_value);

            (wallet.find( (txn.get_output(j)).addr )->second)[1] = txn.hash_transaction(); 

            (wallet.find( (txn.get_output(j)).addr )->second)[2] = to_string(j);
        }
        else
        {
            //No esta en la wallet
            //Se carga el Vector<string> de la wallet con el value, hash transferencia y el idx 
            Vector<string> aux(3);
            aux[0] = txn.get_output(j).value;	
            aux[1] = txn.hash_transaction(); 
            aux[2] = to_string(j); 

            pair <string, Vector<string> > mywallet (txn.get_output(j).addr , aux);
            wallet.insert(mywallet);  
        }
    }
}

//Operador para imprimir en output
ostream & operator<<( ostream &output,  Block &block)
{
	output<<block.get_prev_block()<<"\n";
	output<<block.get_txns_hash()<<"\n";
	output<<block.get_bits()<<"\n";
	output<<block.get_nonce()<<"\n";
	output<<block.get_txns_count()<<"\n";

	int amount_outputs = block.get_txns_count();
	Vector<Transaction> vector_txns = block.get_txns();

	for(int i = 0; i < amount_outputs; i++)//Cant de Transacciones
	{
		output<<(vector_txns)[i];
	}
	return output;    
}

//Operador para cargar un bloque a partir de un input
istream & operator>>( istream & input, Block & block)
{
	string aux_load;
	// Carga el header del bloque:
	getline(input, aux_load);
	if(input.eof() == true)
		return input;
	if (aux_load.length() != HASH_LENGTH || is_alphanumeric(aux_load) == false || aux_load.empty() == true)
	{
		cerr << "Invalid input information from command load"<< endl;
		exit(1);
	}
	block.set_prev_block(aux_load);

	getline(input, aux_load);
	if (aux_load.length() != HASH_LENGTH || is_alphanumeric(aux_load) == false || aux_load.empty() == true)
	{
		cerr << "Invalid input information from command load"<< endl;
		exit(1);
	}
	block.set_txns_hash(aux_load);
	
	getline(input, aux_load);
	if ( is_int(aux_load) == false || stoi(aux_load) > MAXIMUM_DIFFICULTY || aux_load.empty() == true )
	{
		cerr << "Invalid input information from command load. "<< endl;
		exit(1);
	}
	block.set_bits(stoi(aux_load));

	getline(input,aux_load);
	if ( is_int(aux_load) == false || aux_load.empty() == true) 
	{
		cerr << "Invalid input information from command load. "<< endl;
		exit(1);
	}
	block.set_nonce(stoi(aux_load));

	//Carga el Body:
	getline(input,aux_load); 
	if ( is_int(aux_load) == false || aux_load.empty() == true)
	{
		cerr << "Invalid input information from command load. "<< endl;
		exit(1);
	}
	block.set_txns_count(stoi(aux_load));


	Vector <Transaction> aux_tnxs(block.get_txns_count());
	Vector <TreeNode *> leaves_hashes(block.get_txns_count()); //Vector de hojas del arbol de Merkle
	TreeNode *p = NULL;

	float total_txn_value = 0; 
	for(int i = 0; i < block.get_txns_count(); i++) 
	{
		//Se carga cada transaccion
		aux_tnxs[i].load_Transaction(&input, total_txn_value);

		if(wallet.empty()==false)
		{
			//Se valida que la input de la transccion exista en la wallet
			if( wallet.count(aux_tnxs[i].get_input(0).addr) == 0)
			{
				cerr << "FAIL: Non valid source for transaction: "
					 << aux_tnxs[i].get_input(0).addr
					 << "." 
					 << endl;
				exit(1);
			}
			if(total_txn_value > stof((wallet.find(aux_tnxs[i].get_input(0).addr)->second)[0]) )
			{
				cerr << "Invalid transaction, not sufficient funds: "
			  	   	 <<	stof((wallet.find(aux_tnxs[i].get_input(0).addr)->second)[0])
			  	   	 << ". "
			 	     << endl;
				exit(1);
			}

			block.load_wallet( aux_tnxs[i] );
		}
		else
		{
			Vector <string> aux_genesis(3);
			aux_genesis[0] = (aux_tnxs)[0].get_output(0).value;
			aux_genesis[1] = (aux_tnxs)[0].hash_transaction();
			aux_genesis[2] = "0";

			wallet.insert( {(aux_tnxs)[i].get_output(0).addr, aux_genesis} );
		}

		hashes_transactions->insert_after((aux_tnxs)[i].hash_transaction(),hashes_transactions->last());

		leaves_hashes[i] = new TreeNode((aux_tnxs)[i].hash_transaction());
		leaves_hashes[i]->set_left(p);
		leaves_hashes[i]->set_rigth(p);
	}

	MerkleTree *Tree = new MerkleTree(leaves_hashes);
	string expected_txns_hash = (Tree->get_root())->get_hash();
	delete Tree;

	if(expected_txns_hash != block.get_txns_hash())
	{
		cerr << "Invalid block, txns_hash does not match transaction's hash."
	 	     << endl;
		exit(1);
	}
	block.set_txns(aux_tnxs);

	//Verifica que el hash del header cumpla con la dificultad pasada
	if(block.is_valid_hash_header(block.get_bits()) == false)
	{
		cerr << "FAIL: Difficulty does not verify."
			 << endl;
		exit(1);
	}
	return input; 
}
