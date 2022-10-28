#include "main.h"

static void opt_input(string const &);
static void opt_output(string const &);

istream *iss = 0;	// Input Stream (clase para manejo de los flujos de entrada)
ostream *oss = 0;	// Output Stream (clase para manejo de los flujos de salida)
fstream ifs; 		// Input File Stream (derivada de la clase ifstream que deriva de istream para el manejo de archivos)
fstream ofs;		// Output File Stream (derivada de la clase ofstream que deriva de ostream para el manejo de archivos)

void command_init(string &);
void command_transfer(string &);
void command_mine(string &);
void command_balance(string &);
void command_transaction(string &);
void command_block(string &);
void command_load(string &);
void command_save(string &);
void clear_global_variables();

/**************** Elementos globales ******************/
static option_t options[] = 
{
	{1, "o", "output", "-", opt_output, OPT_DEFAULT},
	{1, "i", "input", "-", opt_input, OPT_DEFAULT},
	{0, },
};

static command_t commands[]= 
{
 {1, "init", command_init},
 {1, "balance", command_balance},
 {1, "transfer", command_transfer},
 {1, "mine", command_mine},
 {1, "load", command_load},
 {1, "save", command_save},
 {1, "block", command_block},
 {1, "txn", command_transaction},
 {0, },
}; 

unordered_map < string, Vector<string> > wallet;

list<Block> *Algochain = new list<Block>;
list<Transaction> *mempool = new list<Transaction>;
list<string> *hashes_transactions = new list<string>;
list<string> *hashes_blocks = new list<string>;

/******************************************************/
static void
opt_input(string const &arg)
{
	/* Si el nombre del archivos es "-", usaremos la entrada
	   estandar. De lo contrario, abrimos un archivo en modo
	   de lectura.*/
	if (arg == "-") 
	{
		iss = &cin;
	}

	
	else 
	{
		ifs.open(arg.c_str(), ios::in); /*c_str(): Returns a pointer to an array that contains a null-terminated
										  sequence of characters (i.e., a C-string) representing
										  the current value of the string object.*/
		iss = &ifs;
	}

	// Verificamos que el stream este OK.
	if (!iss->good()) {
		cerr << "cannot open file "
		     << arg
		     << "."
		     << endl;
		exit(1);
	}
}

static void
opt_output(string const &arg)
{
	/* Si el nombre del archivos es "-", usaremos la salida
	   estandar. De lo contrario, abrimos un archivo en modo
	   de escritura.
	*/
	if (arg == "-") {
		oss = &cout;	// Establezco la salida estandar cout como flujo de salida
	} else {
		ofs.open(arg.c_str(), ios::out);
		oss = &ofs;
	}

	// Verificamos que el stream este OK.
	if (!oss->good()) {
		cerr << "cannot open "
		     << arg
		     << "."
		     << endl;
		exit(1);		// EXIT: Terminacion del programa en su totalidad
	}
}

//Genera el primer bloque ( genesis ) de la Algochain 
//Recibe como argumento el nombre del usuario, el monto inicial, y la dificultad de minado
//Imprime el hash del bloque genesis o error terminal en caso de falla
void command_init(string & args)
{
	Vector <string> inits;
	string delim = " ";
	
	inits = split_string(args, delim);

	valid_init_args(inits);

	clear_global_variables();
	
	Block *block_Genesis = new Block;

	Block Gen(*block_Genesis);

	Gen.load_Genesis(inits[0], stof(inits[1]), stoi(inits[2]));

	Algochain->insert_after(Gen, Algochain->last());

	hashes_blocks->insert(Gen.hash_Block());

	*oss << Gen.hash_Block()<<endl;

	delete block_Genesis;
}

//Genera una transaccion a ser insertada en la mempool
//Recibe como argumento el nombre ususario fuente, el nombre del usuario destino y el monto de la transferencia
//Imprime el hash de la transaccion o error en caso de falla
void command_transfer(string & args)
{
	string source_address, src_arg, src_value,output_pos, source_txn, delim = " ";
	Vector <string> transfers;
	Transaction txn;
	float total_value = 0;

	transfers = split_string(args, delim);

	valid_transfer_args_amount(transfers);

	source_address=sha256(sha256(transfers[0]));

	if( wallet.count(source_address) == 1 )
	{	
		src_value =  (( (wallet.find(source_address)) )->second)[0]; 
		source_txn = (( (wallet.find(source_address)) )->second)[1];
		output_pos = (( (wallet.find(source_address)) )->second)[2];
	}
	else
	{
		cerr << "FAIL: This user does not belong to the Algochain "
		     << transfers[0]
		     << "."
		     << endl;
		return;
	}
	
	valid_transfer_args(transfers, total_value, src_value);
	if(total_value > stof(src_value))
		return;
	
	// Inserto la transferencia en la mempool
 	txn.load_transaction_for_mempool(transfers, total_value,  stof(src_value), source_txn, stoi(output_pos) );
	
	Block bloque_aux;
	bloque_aux.load_wallet(txn);
	
	mempool->insert_after(txn, mempool->last());
	
	hashes_transactions->insert_after(txn.hash_transaction(),hashes_transactions->last());
	*oss << txn.hash_transaction() << endl;
}

//Genera un bloque con todas las transacciones que se encuentren en la mempool
//Recibe como argumento la dificultad de minado del bloque
//Imprime el hash del bloque o error en caso de falla
void command_mine(string & args)
{
    //Validacion contra dificultad negativa, que sea un int y un maximo de 256.
    if (is_int(args) == false || stoi(args) > MAXIMUM_DIFFICULTY)
    {
        cerr << "non-valid difficulty: "
             << args
             << ". It must be a positive integer between 0 and 256."
             << endl;
        exit(1);
    }
    //Chequeamos si la mempool esta vacia, porque necesitamos los transferencias para armar el bloque
    if (mempool->empty() == true)
    {
        cerr << "FAIL: Cannot mine with no transactions. "
             << endl;
        return;
    }       

    //Verifico que exista un bloque genesis como primer elemento de la Algochain:
	if( Algochain->empty() == true || Algochain->first().data().is_genesis() == false)
	{
		cerr << "FAIL: Cannot mine with no genesis block. "
		     << endl;
		return;
	}

	//Cargo el bloque con todas las transacciones de la mempool
	Block *block = new Block;
	block->load_block(args);
	
	Block bl(*block);

	//Cargo el bloque en la Algochain:
	Algochain->insert_after(bl, Algochain->last());

	hashes_blocks->insert_after(bl.hash_Block(), hashes_blocks->last());

	delete mempool;
	mempool = new list<Transaction>;

	*oss << bl.hash_Block() << endl;
	delete block;
}

//Verifica el balance de un usuario en la WALLET
//Recibe como argumento el nombre del usuario
//Imprime el balance total del usuario o 0 si no se encuentra en la WALLET
void command_balance(string & args)
{
	//Chequeo el formato para el nombre del usuario: 
	if (is_alphabetic(args) == false)
	{
		cerr << "non-valid user: "
		     << args
		     << "." 
		     << endl;
		exit(1);
	}

	if( wallet.count( sha256(sha256(args)) )==1 )
	{
		*oss<< (wallet.find(sha256(sha256(args)))->second)[0]<<endl;
	}
	else
	{
		*oss<<"0"<<endl;
	}
}

//Consulta la informacion contenida en la transferencia
//Recibe el hash de la transaccion
//Imprime los campos de la transaccion o FAIL por hash invalido.
void command_transaction(string &args)
{	
	//Chequeo el formato para el hash de la transaccion: 
	if (is_alphanumeric(args) == false || args.length() != HASH_LENGTH )
	{
		cerr << "non-valid transaction: "
		     << args
		     << "." 
		     << endl;
		exit(1);
	}
	//Chequeo si existe la transaccion del hash: 
	if(hashes_transactions->contains(args) == false ) 
	{
		cerr << "FAIL:transaction not found. "
		     << endl;
		return;
	}
	//Busco en la Algochain
	for(list<Block>::iterator iter_list(Algochain->first());  iter_list.end()==false; iter_list.go_forward())
	{
		for(int i=0; i< (iter_list.data()).get_txns_count(); i++)
		{
			if( ((iter_list.data().get_txns())[i]).hash_transaction() == args )
			{
				*oss<<((iter_list.data().get_txns())[i])<<endl;
				return;	
			}
		}
	}//Busco en la Mempool:
	for(list<Transaction>::iterator iter_list (mempool->first());  iter_list.end()==false; iter_list.go_forward())
	{
		if( ((iter_list).data()).hash_transaction() == args )
		{
			*oss<<((iter_list).data())<<endl;
			return;	
		}
	}

	cerr << "FAIL:transaction not found "<< args<< "." << endl;
	return;
}

//Consulta la informacion contenida en un bloque
//Recibe el hash del bloque
//Imprime todos los campos del bloque o FAIL por hash invalido.
void command_block(string & args)
{
	//Chequeo el formato para el hash del block: 
	if (is_alphanumeric(args) == false || args.length() != HASH_LENGTH )
	{
		cerr << "non-valid argument for command block: "
		     << args
		     << "." 
		     << endl;
		exit(1);
	}
	//Chequeo si existe el hash del bloque: 
	if( hashes_blocks->contains(args) == false ) 
	{
		cerr << "FAIL:Block not found "
		     << args
		     << "." 
		     << endl;
		return;
	}
	
	for(list<Block>::iterator iter_list(Algochain->first());  iter_list.end()==false; iter_list.go_forward())
	{
		if(  !(args.compare( ((iter_list).data()).hash_Block()) ) )
		{
			*oss<<((iter_list).data());
			return;	
		}
	}
}

//Lee la Algochain serializada en el archivo pasado por parametro.
//Recibe un archivo que incluye los bloques de la Algochain
//Imprime el hash del ultimo bloque de la cadena o FAIL por bloque o/y transaccion invalido.
void command_load(string & args)
{
	static istream *input_stream = 0;
	static fstream input_file; 
	
	//Chequeo el nombre del archivo
	if (is_alphanumeric_point(args) == false )
	{
		cerr << "non-valid file name: "
		     << args
		     << "." 
		     << endl;
		exit(1);
	}
	// Se abre un archivo en forma de lectura que contiene bloques
	input_file.open(args.c_str(), ios::in);
	input_stream = &input_file;
	
	// Verificamos que el stream este OK.
    if (!input_stream->good()) {
        cerr << "FAIL:cannot open file "
		     << args
		     << "."
		     << endl;
		return;		
	}

	//Validar si el archivo esta vacio
	if( input_stream->peek() == EOF)
	{
		cerr << "Empty file detected, no information to load from "
		     << args
		     << "." 
		     << endl;
		exit(1);
	}	

	//Borrar algochain anterior
	clear_global_variables();
	
	// Cargamos el primer bloque en la Algochain
	Block *block = new Block;
	
	//Cargo el bloque con la informacion del archivo input_stream
	*input_stream>>*block;

	Block bl(*block);
	
	Algochain->insert_after(bl, Algochain->last());

	hashes_blocks->insert_after(bl.hash_Block(), hashes_blocks->last());

	// Chequeamos que el primer bloque sea el genesis:
	if( Algochain->first().data().is_genesis() == false )
	{
		cerr << "FAIL: First block must be genesis."<< endl;
		exit(1);
	}

	// Seguimos cargando los bloques hasta el final del archivo:
	while(input_stream->eof() == false)
	{
		//Cargo el bloque con la informacion del archivo input_stream
		*input_stream>>*block;
		Block bl(*block);

		Algochain->insert_after(bl, Algochain->last());
		hashes_blocks->insert_after(bl.hash_Block(), hashes_blocks->last());
	}
	*oss<< hashes_blocks->last().data()<<endl;
	delete block;
}

//Lee la Algochain serializada e imprime en el archivo pasado por parametro
//Recibe un archivo
//Imprime OK en exito o FAIL en FALLA.
void command_save(string & args)
{
	fstream output_file;
	ostream *output_stream;

	//Chequeo el nombre del archivo
	if (is_alphanumeric_point(args) == false )
	{
		cerr << "non-valid file name: "
		     << args
		     << "." 
		     << endl;
		exit(1);
	}

	//Abro el archivo en modo escritura:
	output_file.open(args.c_str(), ios::out);
	output_stream=&output_file;

	// Verificamos que el stream este OK.
	if (!output_stream->good()){
		cerr << "FAIL:cannot open file "
		     << args
		     << "."
		     << endl;
		return;		
	}
	
	for(list<Block>::iterator iter_list(Algochain->first());  iter_list.end()==false ; iter_list.go_forward())
	{
		output_file<<((iter_list).data());
	}
	*oss<<"OK"<<endl;
}

//Funcion para borrar la informacion de las variables globales
void clear_global_variables()
{
	//Borrar algochain anterior
    if (Algochain->empty() == false)
    {
        //borra los bloques de la algochain
        delete Algochain;
        Algochain = new list<Block>;
        
        //borra la wallet
        wallet.clear();

        //borra los bloques de la hashes_txn
        delete hashes_transactions;
        hashes_transactions = new list<string>;
        
        //borra los bloques de la hashes_block
        delete hashes_blocks;
        hashes_blocks = new list<string>;
        
        //borra los bloques de la mempool
        delete mempool;
        mempool = new list<Transaction>;
    }
}

int
main(int argc, char * const argv[])
{
	string line;

	cmdline cmdl(options);	// Objeto con parametro tipo option_t (struct) declarado globalmente.
	cmdl.parse(argc, argv); // Metodo de parseo de la clase cmdline

	Command command(commands);

	if (iss == &cin)
	{
		while(1)
		{
			if(iss->eof() == true)
				return -1;
			command.parse(iss);
		}
	}
	else
	{
		command.parse(iss);	
	}

	delete Algochain;
	delete mempool;
	delete hashes_transactions;
	delete hashes_blocks;
}
