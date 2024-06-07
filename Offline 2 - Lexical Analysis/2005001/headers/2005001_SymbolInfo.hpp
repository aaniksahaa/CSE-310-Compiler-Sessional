
// Guard against multiple declarations

#ifndef SYMBOLINFO_HEADER
#define SYMBOLINFO_HEADER

#include<iostream>
#include<string>

using namespace std;

// SymbolInfo contains the name and type of a single symbol
// It also includes a pointer to another SymbolInfo to implement
// chaining for preventing hash collisions

class SymbolInfo
{
	string name, type;
	bool verbose_destructor;
public:
	SymbolInfo *next;
	SymbolInfo(string name, string type, SymbolInfo* next = NULL, bool verbose_destructor = false)
	{
		this->name = name;
		this->type = type;
		this->next = next;
		this->verbose_destructor = verbose_destructor;
	}
	~SymbolInfo()   // Destructor
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
		cout<<endl;
		cout<<"Name = "<<name<<endl;
		cout<<"Type = "<<type<<endl;
		cout<<endl;
	}
};

#endif  // SYMBOLINFO_HEADER
