
// Guard against multiple declarations

#ifndef SYMBOLTABLE_HEADER
#define SYMBOLTABLE_HEADER

#include<iostream>
#include<string>

#include "ScopeTable.hpp"

using namespace std;

// SymbolTable implements a list of Scopetables

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
		if(verbose) cout<<"\tScopeTable# "<<current_scopetable->get_id()<<" created"<<endl;
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
			if(verbose) cout<<"'"<<name<<"' already exists in the current ScopeTable# "<<current_scopetable->get_id()<<endl;
		}
		return ret;
	}
	bool remove_from_current_scope(string name, bool verbose = false)
	{
		bool ret = current_scopetable->remove(name, verbose);
		if(ret == false)
		{
			if(verbose) cout<<"Not found in the current ScopeTable# "<<current_scopetable->get_id()<<endl;
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
		if(verbose) cout<<"'"<<name<<"' not found in any of the ScopeTables"<<endl;
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


#endif // SYMBOLTABLE_HEADER