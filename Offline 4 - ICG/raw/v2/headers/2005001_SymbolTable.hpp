
// Guard against multiple declarations

#ifndef SYMBOLTABLE_HEADER
#define SYMBOLTABLE_HEADER

#include<iostream>
#include<string>

#include "2005001_ScopeTable.hpp"

using namespace std;

// SymbolTable implements a list of Scopetables

class SymbolTable
{
	int number_of_buckets;             // number of buckets in scope tables
	ScopeTable* current_scopetable;
	ostream& out;
	bool verbose_destructor;
	int number_of_scope_tables;

public:

	SymbolTable(int n, bool verbose = false, ostream& outfile = cout, bool verbose_destructor = false): out(outfile)
	{
		this->number_of_buckets = n;
		this->verbose_destructor = verbose_destructor;
		current_scopetable = NULL;
		number_of_scope_tables = 0;
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
		// if(current_scopetable != NULL)
		// {
		// 	current_scopetable->increment_number_of_children();
		// }
		//ScopeTable* new_scopetable = new ScopeTable(number_of_buckets, current_scopetable, out, verbose_destructor);
		
		// changed for new instruction
		ScopeTable* new_scopetable = new ScopeTable(number_of_buckets, current_scopetable, out, verbose_destructor, ++number_of_scope_tables);
		

		current_scopetable = new_scopetable;
		if(verbose) out<<"\tScopeTable# "<<current_scopetable->get_id()<<" created"<<endl;
	}
	void exit_scope(bool verbose = false, bool decrement_count = false)
	{
		// cout<<"Debugging...\n\n\n";
		// cout<<current_scopetable->get_id()<<"\n";

		if(current_scopetable->parent_scope == NULL)
		{
			// because the first scope (created due to main)
			// can not be exited
			if(verbose) out<<"\tScopeTable# 1 cannot be deleted"<<endl;
			return;
		}
		ScopeTable* temp = current_scopetable;
		current_scopetable = current_scopetable->parent_scope;
		temp->parent_scope = NULL;
		delete temp;

		// turn it on for dummy scope creations
		if(decrement_count && number_of_scope_tables)
		{
			number_of_scope_tables--;
		}

		// cout<<current_scopetable->get_id()<<"\n";
		// cout<<"\n\n\n";
	}
	bool insert_into_current_scope(string name, string type, bool verbose = false, bool propagate_verbose = true)
	{
		bool ret = current_scopetable->insert(name,type,verbose && propagate_verbose);
		if(ret == false)
		{
			if(verbose) out<<"'"<<name<<"' already exists in the current ScopeTable# "<<current_scopetable->get_id()<<endl;
		}
		return ret;
	}
	bool remove_from_current_scope(string name, bool verbose = false, bool propagate_verbose = true)
	{
		bool ret = current_scopetable->remove(name, verbose && propagate_verbose);
		if(ret == false)
		{
			if(verbose) out<<"Not found in the current ScopeTable# "<<current_scopetable->get_id()<<endl;
		}
		return ret;
	}
	SymbolInfo* lookup(string name, bool verbose = false, bool propagate_verbose = true)
	{
		ScopeTable* curr = current_scopetable;
		while(curr != NULL)
		{
			SymbolInfo* existing_entry = curr->lookup(name,verbose && propagate_verbose);
			if(existing_entry != NULL)
			{
				//so, found in this scopetable
				return existing_entry;
			}
			curr = curr->parent_scope;
		}
		if(verbose) out<<"'"<<name<<"' not found in any of the ScopeTables"<<endl;
		return NULL;
	}
	// should be called only upon successful prior lookup
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
	string stringify_current_scope_table()
	{
		string ret = "";
		ret += current_scopetable->stringify_scope_table();
		return ret;
	}
	string stringify_all_scope_tables()
	{
		string ret = "";
		ScopeTable* curr = current_scopetable;
		while(curr != NULL)
		{
			ret += curr->stringify_scope_table();
			curr = curr->parent_scope;
		}
		return ret;
	}
};


#endif // SYMBOLTABLE_HEADER