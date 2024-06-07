
// Guard against multiple declarations

#ifndef SYMBOLINFO_HEADER
#define SYMBOLINFO_HEADER

#include<iostream>
#include<string>

#include "2005001_ParseTree.hpp"

using namespace std;

// SymbolInfo contains the name and type of a single symbol
// It also includes a pointer to another SymbolInfo to implement
// chaining for preventing hash collisions

#define IS_LEAF 0
#define IS_FUNCTION 1
#define IS_ARRAY 2

class SymbolInfo
{
    string name, type;
    bool verbose_destructor;
    unsigned char boolean_properties; // open for extension  // stores boolean properties in individual bits
    TreeNode* parse_tree_node;

    void update_parse_tree_node(int s, int e)
    {
        parse_tree_node->content = type + string(" : ") + name;
        parse_tree_node->start_line_count = s;
        parse_tree_node->end_line_count = e;
    }

public:
    SymbolInfo *next;
    SymbolInfo(string name, string type, SymbolInfo* next = NULL, bool verbose_destructor = false)
    {
        this->name = name;
        this->type = type;
        this->next = next;
        this->verbose_destructor = verbose_destructor;
        this->parse_tree_node = new TreeNode();
        boolean_properties = 0;
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

    //functions regarding additional info
    void set_line_counts(int s, int e)   // updates tree node automatically
    {
        update_parse_tree_node(s,e);
    }
    TreeNode* get_parse_tree_node()
    {
        return parse_tree_node;
    }
    void set_property(int bit)
    {
        boolean_properties |= (1<<bit);
    }
    void unset_property(int bit)
    {
        boolean_properties &= ~(1<<bit);
    }
    bool check_property(int bit)
    {
        return (boolean_properties & (1<<bit));
    }

    void set_children_tree_nodes(size_t numChildren, ...) {
        va_list args;
        va_start(args, numChildren);

        for (size_t i = 0; i < numChildren; ++i) {
            SymbolInfo* childSymbol = va_arg(args, SymbolInfo*);
            parse_tree_node->childrenNodes.push_back(childSymbol->get_parse_tree_node());
        }

        va_end(args);
    }
    
};

#endif  // SYMBOLINFO_HEADER
