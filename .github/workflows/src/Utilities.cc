#include "Utilities.h"

string Hex2Bin(const string& s)//transforma de hexa a binario una string
{
	string bin_number="";

	for(size_t i=0; i< s.length(); i++)
    {
		stringstream ss;
		unsigned n;
		ss << hex << s[i];
    	ss >> n;
		
    	bitset<4> b(n);
		bin_number.append( b.to_string() );
	}
	
	return bin_number;
}

bool is_float_or_double(const string &str)
{

    return str.find_first_not_of("0123456789.") == string::npos;
}

bool is_alphanumeric(const string &str)
{

    return str.find_first_not_of("0123456789qwertyuiopasdfghjklzxcvbnm") == string::npos;
}

bool is_alphabetic(const string &str)
{

    return str.find_first_not_of("qwertyuiopasdfghjklzxcvbnm") == string::npos;
}

bool is_alphanumeric_point(const string &str)
{

    return str.find_first_not_of("0123456789qwertyuiopasdfghjklzxcvbnm._-") == string::npos;
}

bool is_int(const string &str)
{

    return str.find_first_not_of("0123456789") == string::npos;
}

bool is_point(const string &str)
{

    return str.find_first_not_of(".") == string::npos;
}

int count_points(string s)
{
    int count = 0;

    for (size_t i = 0; i < s.length(); i++)
    {
        if (s[i] == '.')
            count++;
    }

  return count;
}

void valid_init_args(Vector<string> & inits)
{
    if (inits[3]!=END_OF_ARGS) //Los argumentos son <user> <value> <bits>
    {
        cerr << "Invalid amount of arguments for command init. "
             << endl;
        exit(1);
    }

    if (is_alphabetic(inits[0]) == false) //inits[0] = user
    {
        cerr << "non-valid user: "
             << inits[0] 
             << "."
             << endl;
        exit(1);
    }

    if (is_float_or_double(inits[1]) == false) //inits[1] = value
    {
        cerr << "non-valid value: "
             << inits[1] 
             << "."
             << endl;
        exit(1);
    }

    if (is_int(inits[2]) == false) //inits[2] = dificultad; // no se admite dificultad negativa
    {
        cerr << "non-valid difficulty: "
             << inits[2] 
             << "."
             << endl;
        exit(1);
    }
}

void valid_transfer_args_amount(Vector <string> & transfers)
{
    // Validacion contra impar y contra minima cantidad de arguemntos
    if ( transfers.position(END_OF_ARGS) % 2 == 0 || transfers.position(END_OF_ARGS) < MIN_AMOUNT_TRANSFER_ARGS)
    {
        cerr << "Invalid amount of arguments for transfers."
             << endl;
        exit(1);
    }
    if (is_alphabetic(transfers[0]) == false)
    {
        cerr << "non-valid source : "
             << transfers[0]
             << "."
             << endl;
        exit(1);
    }
}

void valid_transfer_args( Vector <string> transfers, float & total_value, string & src_value)
{
    //Se chequea DESTINO y VALUE, ademas se tomam la suma de los values para despues ver si tiene saldo suficiente
    //Se valida el formato de los argumentos, la doble inclusion de un usuario en una misma transferencia 
    // y que el saldo de la transferencia no supere al balance total.
    for (int i = 1; i < transfers.position(END_OF_ARGS) ; i = i + 2)
    {
        // Validacion ante doble-inclusion
        if (transfers.search(transfers[i]) != 1)
        {
            cerr << "FAIL: Repeated destination"
                 << transfers[i]
                 << "."
                 << endl;
            return;
        }

        if (is_alphabetic(transfers[i]) == false)
        {
            cerr << "non-valid destination : "
                 << transfers[i]
                 << "."
                 << endl;
            exit(1);
        }

        if (is_float_or_double(transfers[i+1]) == false || is_point(transfers[i+1]) == true  || count_points(transfers[i+1]) > 1)
        {
            cout << "non-valid value: "
                 << transfers[i+1] 
                 << endl;
            exit(1);            
        }

        total_value+=stof(transfers[i+1]);

        if(total_value > stof(src_value))
        {
            cerr << "FAIL: not sufficient funds. "
                 << endl;
            return;         
        }
    }
}   

Vector <string> split_string(string & args, string & delimiter)
{
    size_t pos = 0;
    int i = 0;
    string token;
    Vector <string> transfers;
   
    while ((pos = args.find(delimiter)) != string::npos)
    {
        token = args.substr(0, pos);
        if (i == transfers.get_size())
        {
            Vector <string> aux(2*(transfers.get_size()) + 1, transfers);
            transfers = aux;
        }
        transfers[i] = token;
        i++;
        args.erase(0, pos + delimiter.length());
    }
    transfers[i] = args;
    transfers[i+1] = END_OF_ARGS;

    return transfers;
}
