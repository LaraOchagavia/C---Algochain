#include "Command.h"

Command::Command(command_t * comm):command_table(comm)
{	
}

void Command::parse(istream *is)
{
	string commands_names[] = {"init", "transfer", "mine", "balance", "block", "txn", "load", "save"};
	string line, line_name, line_args, delim = " ";
	int found=0;

	while (getline(*is,line).eof() == false)
	{
		if (line.empty() == true)
		{
			cout << "Empty line in commands \n";
			continue;
		}			

		line_name = line.substr(0,line.find(delim));
		found = 0;
		for (int i = 0; i < AMOUNT_COMMANDS ; i++)
		{
			if (line_name == commands_names[i])
				found++;
		}

		if(found == 0)
		{
			cerr << "Command not found: "
			   	 << "--"
				 << line_name
				 << "\n";
			exit(1);
		}
	
		line_args = line.erase(0, line.find(delim) + DELIM_LENGTH);
		
		for(command_t *com = command_table; com->name !=0; ++com)
		{
			if (line_name == string(com->name))
			{
				if (line_args.length() == 0 || line_args == line_name)
				{
					cerr << "Command requires argument: "
				    	 << "--"
				    	 << com->name
				    	 << "\n";
					exit(1);
				}
				com->parse(line_args);//Pasaje mediante puntero a funcion	
				break; 		
			}

		}			
	}

	if (line.empty() == false)
	{

		line_name = line.substr(0,line.find(delim));
		
		for (int i = 0; i < AMOUNT_COMMANDS ; i++)
		{
			if (line_name == commands_names[i])
				found++;
		}

		if(found == 0)
		{
			cerr << "Command not found: "
			   	 << "--"
				 << line_name
				 << "\n";
			exit(1);
		}

		line_args = line.erase(0, line.find(delim) + DELIM_LENGTH);

		for(command_t *com = command_table; com->name !=0; ++com)
		{
			if (line_name == string(com->name))
			{
				if (line_args.length() == 0)
				{
					cerr << "Command requires argument: "
				    	 << "--"
				    	 << com->name
				    	 << "\n";
					exit(1);
				}
				com->parse(line_args);//Pasaje mediante puntero a funcion	
				break; 		
			}

		}
	}			

	return;
}
