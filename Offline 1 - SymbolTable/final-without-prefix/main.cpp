
#include <bits/stdc++.h>

#include "SymbolTable.hpp"

using namespace std;

/*** Definitions ***/

#define gap " "
#define fo(i,n) for(int i=0;i<n;i++)
#define pii pair<int,int>
#define double long double
#define debug(x) cout<< #x <<" = "<< x<<endl

/*** Constants ***/

const int inf = INT_MAX;

/*** Random Number Generator ***/

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

void initialize_random_seed(int seed)
{
	rng.seed(seed);
}

int random(int l,int r) {
	return uniform_int_distribution<int>(l, r)(rng);
}

/*** Helper Functions ***/

bool fileExists(const string& filename) {
    ifstream file(filename);
    return file.good();
}

/*** Main Function ***/

int main()
{
    string input_filename = "input.txt";
    string output_filename = "output.txt";

    if(!fileExists(input_filename))
    {
        cout<<"Sorry! Input file does not exist.";
        return 0;
    }

    freopen(input_filename.c_str(), "r", stdin);
    freopen(output_filename.c_str(), "w", stdout);

	int number_of_buckets_in_scopetables;

	cin>>number_of_buckets_in_scopetables;

	SymbolTable* ST = new SymbolTable(number_of_buckets_in_scopetables, true);

	string choice;
	string line;

	string error_arg_cnt = "Wrong number of arugments for the command ";

	int cmd_cnt = 0;

    while (getline(cin, line)) {

    	if(line == "") continue;

        istringstream string_stream(line);
        string command;
        string arg[3];

        string_stream >> command;

        string temp;
        int arg_cnt = 0;
        while (string_stream >> temp) {
        	arg[arg_cnt] = temp;
        	arg_cnt++;
        }

        int expected_arg_cnt;

        ScopeTable* current_scopetable = ST->get_current_scopetable();

        string current_scopetable_id = current_scopetable->get_id();

        cmd_cnt++;
        cout<<"Cmd "<<cmd_cnt<<": "<<line<<endl;

        if(command == "I" || command == "L" || command == "D")
        {
        	cout<<"\t";
        }

        if(command == "I")
        {
        	expected_arg_cnt = 2;

        	if(arg_cnt != expected_arg_cnt)
        	{
        	 	cout<<error_arg_cnt<<command<<endl;
        	 	continue;
        	}

        	string name = arg[0];
        	string type = arg[1];

        	ST->insert_into_current_scope(name, type, true);

        }
        else if(command == "L")
        {
        	expected_arg_cnt = 1;

        	if(arg_cnt != expected_arg_cnt)
        	{
        	 	cout<<error_arg_cnt<<command<<endl;
        	 	continue;
        	}

        	string name = arg[0];

        	SymbolInfo* ret = ST->lookup(name, true);

        }
        else if(command == "D")
        {
        	expected_arg_cnt = 1;

        	if(arg_cnt != expected_arg_cnt)
        	{
        	 	cout<<error_arg_cnt<<command<<endl;
        	 	continue;
        	}

        	string name = arg[0];

        	ST->remove_from_current_scope(name, true);
        }
        else if(command == "P")
        {
        	expected_arg_cnt = 1;

        	if(arg_cnt != expected_arg_cnt)
        	{
        	 	cout<<error_arg_cnt<<command<<endl;
        	 	continue;
        	}

        	string operation = arg[0];

        	if(operation == "A")
        	{
        		ST->print_all_scope_tables();
        	}
        	else if(operation == "C")
        	{
        		ST->print_current_scope_table();
        	}
        	else
        	{
        		cout<<"\tInvalid argument for the command P";
        		cout<<endl;
        	}
        }
        else if(command == "S")
        {
        	expected_arg_cnt = 0;

        	if(arg_cnt != expected_arg_cnt)
        	{
        	 	cout<<error_arg_cnt<<command<<endl;
        	 	continue;
        	}

        	ST->enter_scope(true);
        }
        else if(command == "E")
        {
        	expected_arg_cnt = 0;

        	if(arg_cnt != expected_arg_cnt)
        	{
        	 	cout<<error_arg_cnt<<command<<endl;
        	 	continue;
        	}

        	if(current_scopetable_id == "1")
        	{
        		cout<<"\tScopeTable# 1 cannot be deleted"<<endl;
        		continue;
        	}
        	ST->exit_scope();
        }
        else if(command == "Q")
        {
        	expected_arg_cnt = 0;

        	if(arg_cnt != expected_arg_cnt)
        	{
        	 	cout<<error_arg_cnt<<command<<endl;
        	 	continue;
        	}
        	ST->~SymbolTable();
        	break;
        }
    }

}
