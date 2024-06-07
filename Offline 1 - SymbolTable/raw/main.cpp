
#include <bits/stdc++.h>
using namespace std;

/*** Definitions ***/

#define gap " "
#define br "\n"
#define fo(i,n) for(int i=0;i<n;i++)
#define pii pair<int,int>
#define double long double
#define debug(x) cout<< #x <<" = "<< x<<endl

/*** Constants ***/

const int inf = INT_MAX;

/*** Random Number Genertor ***/

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

void initialize_random_seed(int seed)
{
	rng.seed(seed);
}

int random(int l,int r) {
	return uniform_int_distribution<int>(l, r)(rng);
}

// Hash Class

class Hash
{
public:
	// here int is actually long long int, so it can fit unsigned long as per original definition
	static uint64_t get_sdbm_hash(string key_string)
	{
		uint64_t hash = 0;
		int len = key_string.length();
		for(int i=0; i<len; i++)
		{
			char ch = key_string[i];
			int c = ch;               // casting into int
			hash = c + (hash<<6) + (hash<<16) - hash;
		}
		return hash;
	}

};

// SymbolInfo

class SymbolInfo
{
	string name, type;
public:
	SymbolInfo *next;
	SymbolInfo(string name, string type, SymbolInfo* next = NULL)
	{
		this->name = name;
		this->type = type;
		this->next = next;
	}
	~SymbolInfo()
	{
		if(next != NULL) delete next;  //so it recursively calls the destructor of the next SymbolInfo objects
	}
	// Getters and Setters
	string get_name()
	{
		return name;
	}
	void set_name(string name)
	{
		this->name = name;
	}
	string get_type()
	{
		return type;
	}
	void set_type(string type)
	{
		this->type = type;
	}
	// For debugging
	void show()
	{
		cout<<br;
		cout<<"Name = "<<name<<br;
		cout<<"Type = "<<type<<br;
		cout<<br;
	}
};

// ScopeTable

class ScopeTable
{
	int number_of_buckets, number_of_children;    // M = number of buckets
	string id;
	SymbolInfo** hash_table;

public:

	ScopeTable* parent_scope;

	ScopeTable(int n, ScopeTable* parent_scope = NULL)
	{
		this->number_of_children = 0;
		if(parent_scope == NULL)
		{
			this->id = "1";
		}
		else
		{
			this->id = parent_scope->get_id() + "." + to_string(parent_scope->get_number_of_children());
		}
		this->parent_scope = parent_scope;
		number_of_buckets = n;
		hash_table = new SymbolInfo*[number_of_buckets];

		for(int i=0; i<number_of_buckets; i++)
		{
			hash_table[i] = NULL;  // initializing with NULL
		}
	}
	~ScopeTable()
	{
		for(int i=0; i<number_of_buckets; i++)
		{
			if(hash_table[i] != NULL) delete hash_table[i];
		}
		delete hash_table;
		cout<<"\tScopeTable# "<<id<<" deleted"<<br;
		if(parent_scope != NULL) delete parent_scope; //so it recursively calls the destructor of the next ScopeTable objects
	}
	string get_id()
	{
		return id;
	}
	int get_number_of_children()
	{
		return number_of_children;
	}
	int get_bucket_index(string name)
	{
		uint64_t hash = Hash::get_sdbm_hash(name);
		int index = hash % number_of_buckets;
		return index;
	}
	int get_index_in_chain(string name)
	{
		int found = -1;
		int index = get_bucket_index(name);
		SymbolInfo* curr = hash_table[index];
		int i = 0;
		while(curr != NULL)
		{
			if(curr->get_name() == name)
			{
				found = i;
				return found;
			}
			curr = curr->next;
			i++;
		}
		return found;
	}
	void increment_number_of_children()
	{
		number_of_children++;
	}
	SymbolInfo* lookup(string name, bool verbose = false)
	{
		int index = get_bucket_index(name);
		SymbolInfo* curr = hash_table[index];
		// searching for the symbol
		int index_in_chain = 0;
		while(curr != NULL)
		{
			if(curr->get_name() == name)  // just match by name as there can't be'two same named symbols in same scope
			{
				if(verbose) cout<<"'"<<name<<"' found at position <"<<index+1<<", "<<index_in_chain+1<<"> of ScopeTable# "<<id<<br;
				return curr;
			}
			curr = curr->next;
			index_in_chain++;
		}
		return NULL;
	}
	bool insert(string name, string type, bool verbose = false)
	{
		SymbolInfo* existing_entry = lookup(name);
		if(existing_entry == NULL)  // does not already exist
		{
			int index = get_bucket_index(name);

			// creating a new SymbolInfo object
			SymbolInfo* new_symbol = new SymbolInfo(name, type, NULL);

			SymbolInfo* curr = hash_table[index];
			int index_in_chain = 0;
			if(curr == NULL)  // first entry on this bucket
			{
				hash_table[index] = new_symbol;
			}
			else
			{
				index_in_chain = 1;
				while(curr->next != NULL)
				{
					curr = curr->next;
					index_in_chain++;
				}
				curr->next = new_symbol;
			}
			if(verbose) cout<<"Inserted  at position <"<<index+1<<", "<<index_in_chain+1<<"> of ScopeTable# "<<id<<br;
			return true;
		}
		else // The same symbol already exists
		{
			return false;
		}
	}
	bool remove(string name, bool verbose = false)
	{
		SymbolInfo* existing_entry = lookup(name);
		if(existing_entry != NULL)
		{
			int index = get_bucket_index(name);

			SymbolInfo* curr = hash_table[index];
			SymbolInfo* temp;  // entry to be deleted
			int index_in_chain = 0;

			if(curr == existing_entry)
			{
				hash_table[index] = curr->next;
				temp = curr;
			}
			else
			{
				index_in_chain = 1;
				while(curr->next != existing_entry)
				{
					curr = curr->next;
					index_in_chain++;
				}
				temp = curr->next;
				curr->next = curr->next->next;
			}
			// to avoid recursive deletion
			temp->next = NULL;
			delete temp;
			if(verbose) cout<<"Deleted '"<<name<<"' from position <"<<index+1<<", "<<index_in_chain+1<<"> of ScopeTable# "<<id<<br;
			return true;
		}
		else
		{
			return false;
		}
	}
	void print_scope_table()
	{
		cout<<"\t";
		cout<<"ScopeTable# "<<this->id<<br;
		for(int i=0; i<number_of_buckets; i++)
		{
			cout<<"\t";
			cout<<i+1;
			SymbolInfo* curr = hash_table[i];
			while(curr != NULL)
			{
				cout<<" --> "<<"("<<curr->get_name()<<","<<curr->get_type()<<")";
				curr = curr->next;
			}
			cout<<br;
		}
	}
};

// SymbolTable

class SymbolTable
{
	int number_of_buckets;             // number of buckets in scope tables
	ScopeTable* current_scopetable;

public:

	SymbolTable(int n, bool verbose = false)
	{
		this->number_of_buckets = n;
		current_scopetable = NULL;
		enter_scope(verbose);
	}
	~SymbolTable()
	{
		delete current_scopetable;
	}
	ScopeTable* get_current_scopetable()
	{
		return current_scopetable;
	}
	void enter_scope(bool verbose = false)
	{
		if(current_scopetable != NULL)
		{
			current_scopetable->increment_number_of_children();
		}
		ScopeTable* new_scopetable = new ScopeTable(number_of_buckets, current_scopetable);
		current_scopetable = new_scopetable;
		if(verbose) cout<<"\tScopeTable# "<<current_scopetable->get_id()<<" created"<<br;
	}
	void exit_scope()
	{
		if(current_scopetable->parent_scope == NULL)
		{
			// because the first scope (created due to main)
			// can't be exited
			return;
		}
		ScopeTable* temp = current_scopetable;
		current_scopetable = current_scopetable->parent_scope;
		temp->parent_scope = NULL;
		delete temp;
	}
	bool insert_into_current_scope(string name, string type, bool verbose = false)
	{
		bool ret = current_scopetable->insert(name,type,verbose);
		if(ret == false)
		{
			if(verbose) cout<<"'"<<name<<"' already exists in the current ScopeTable# "<<current_scopetable->get_id()<<br;
		}
		return ret;
	}
	bool remove_from_current_scope(string name, bool verbose = false)
	{
		bool ret = current_scopetable->remove(name, verbose);
		if(ret == false)
		{
			if(verbose) cout<<"Not found in the current ScopeTable# "<<current_scopetable->get_id()<<br;
		}
		return ret;
	}
	SymbolInfo* lookup(string name, bool verbose = false)
	{
		ScopeTable* curr = current_scopetable;
		while(curr != NULL)
		{
			SymbolInfo* existing_entry = curr->lookup(name,verbose);
			if(existing_entry != NULL)
			{
				//so, found in this scopetable
				return existing_entry;
			}
			curr = curr->parent_scope;
		}
		if(verbose) cout<<"'"<<name<<"' not found in any of the ScopeTables"<<br;
		return NULL;
	}
	// should be called upon successful prior lookup
	ScopeTable* get_scope_table_of_lookup(string name)
	{
		ScopeTable* curr = current_scopetable;
		while(curr != NULL)
		{
			SymbolInfo* existing_entry = curr->lookup(name);
			if(existing_entry != NULL)
			{
				return curr;
			}
			curr = curr->parent_scope;
		}
		return NULL;
	}
	void print_current_scope_table()
	{
		current_scopetable->print_scope_table();
	}
	void print_all_scope_tables()
	{
		ScopeTable* curr = current_scopetable;
		while(curr != NULL)
		{
			curr->print_scope_table();
			curr = curr->parent_scope;
		}
	}
};

int32_t main()
{
    //freopen("input.txt", "r", stdin);
    //freopen("output.txt", "w", stdout);

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
        cout<<"Cmd "<<cmd_cnt<<": "<<line<<br;

        if(command == "I" || command == "L" || command == "D")
        {
        	cout<<"\t";
        }

        if(command == "I")
        {
        	expected_arg_cnt = 2;

        	if(arg_cnt != expected_arg_cnt)
        	{
        	 	cout<<error_arg_cnt<<command<<br;
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
        	 	cout<<error_arg_cnt<<command<<br;
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
        	 	cout<<error_arg_cnt<<command<<br;
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
        	 	cout<<error_arg_cnt<<command<<br;
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
        		cout<<br;
        	}
        }
        else if(command == "S")
        {
        	expected_arg_cnt = 0;

        	if(arg_cnt != expected_arg_cnt)
        	{
        	 	cout<<error_arg_cnt<<command<<br;
        	 	continue;
        	}

        	ST->enter_scope(true);
        }
        else if(command == "E")
        {
        	expected_arg_cnt = 0;

        	if(arg_cnt != expected_arg_cnt)
        	{
        	 	cout<<error_arg_cnt<<command<<br;
        	 	continue;
        	}

        	if(current_scopetable_id == "1")
        	{
        		cout<<"\tScopeTable# 1 cannot be deleted"<<br;
        		continue;
        	}
        	ST->exit_scope();
        }
        else if(command == "Q")
        {
        	expected_arg_cnt = 0;

        	if(arg_cnt != expected_arg_cnt)
        	{
        	 	cout<<error_arg_cnt<<command<<br;
        	 	continue;
        	}
        	ST->~SymbolTable();
        	break;
        }
    }

}
