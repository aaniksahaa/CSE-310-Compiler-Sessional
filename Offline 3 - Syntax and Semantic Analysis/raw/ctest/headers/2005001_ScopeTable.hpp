
// Guard against multiple declarations

#ifndef SCOPETABLE_HEADER
#define SCOPETABLE_HEADER

#include<iostream>
#include<string>

#include "2005001_Hash.hpp"
#include "2005001_SymbolInfo.hpp"

using namespace std;

// ScopeTable implements a hash table for managing symbols
// It uses separate chaining for hash collision
// It additionally includes an id which is derived from the id of its parent scope

class ScopeTable
{
	int number_of_buckets, number_of_children;    // M = number of buckets
	string id;
	SymbolInfo** hash_table;
	ostream& out;
	bool verbose_destructor;

public:

	ScopeTable* parent_scope;

	ScopeTable(int n, ScopeTable* parent_scope = NULL, ostream& outfile = cout, bool verbose_destructor = false): out(outfile)
	{
		this->number_of_children = 0;
		this->verbose_destructor = verbose_destructor;
		if(parent_scope == NULL)
		{
			this->id = "1";     // for the first scope 
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
	~ScopeTable()  // Destructor
	{
		for(int i=0; i<number_of_buckets; i++)
		{
			if(hash_table[i] != NULL) delete hash_table[i];
		}
		delete hash_table;
		if(verbose_destructor) out<<"\tScopeTable# "<<id<<" deleted"<<endl;
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
				if(verbose) out<<"'"<<name<<"' found at position <"<<index+1<<", "<<index_in_chain+1<<"> of ScopeTable# "<<id<<endl;
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
			SymbolInfo* new_symbol = new SymbolInfo(name, type, NULL, verbose_destructor);

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
			if(verbose) out<<"Inserted  at position <"<<index+1<<", "<<index_in_chain+1<<"> of ScopeTable# "<<id<<endl;
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
			if(verbose) out<<"Deleted '"<<name<<"' from position <"<<index+1<<", "<<index_in_chain+1<<"> of ScopeTable# "<<id<<endl;
			return true;
		}
		else
		{
			return false;
		}
	}
	void print_scope_table()
	{
		out<<"\t";
		out<<"ScopeTable# "<<this->id<<endl;
		for(int i=0; i<number_of_buckets; i++)
		{
			out<<"\t";
			out<<i+1;
			SymbolInfo* curr = hash_table[i];
			while(curr != NULL)
			{
				out<<" --> "<<"("<<curr->get_name()<<","<<curr->get_type()<<")";
				curr = curr->next;
			}
			out<<endl;
		}
	}
};

#endif // SCOPETABLE_HEADER