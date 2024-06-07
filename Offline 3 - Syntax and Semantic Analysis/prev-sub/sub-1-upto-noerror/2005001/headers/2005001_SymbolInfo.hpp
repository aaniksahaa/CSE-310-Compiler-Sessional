
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
#define ERROR_OCCURED 3
#define IS_DECLARED_FUNCTION 4
#define IS_DEFINED_FUNCTION 5

class SymbolInfo
{
    string name, type;

    bool verbose_destructor;

    int boolean_properties;     // open for extension  // stores boolean properties in individual bits
    TreeNode* parse_tree_node;

public:

    // evaluated type is stored for all SymbolInfo Objects
    string evaluated_type;                  // return type for functions and type for expressions - what type the evaluated expression will be...
    vector<SymbolInfo*>child_identifiers;   // parameters for functions
    vector<string>parameter_types;
    int arr_size;

    SymbolInfo *next;

    void update_parse_tree_node(int s, int e)
    {
        parse_tree_node->content = type + string(" : ") + name;
        parse_tree_node->start_line_count = s;
        parse_tree_node->end_line_count = e;
    }

    SymbolInfo(string name, string type, SymbolInfo* next = NULL, bool verbose_destructor = false)
    {
        this->name = name;
        this->type = type;
        this->next = next;

        this->verbose_destructor = verbose_destructor;

        this->parse_tree_node = new TreeNode();
        boolean_properties = 0;
        arr_size = 0;
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
    int get_start_line_count()
    {
    	return parse_tree_node->start_line_count;
    }
    int get_end_line_count()
    {
    	return parse_tree_node->end_line_count;
    }

    string get_evaluated_type()
    {
        return evaluated_type;
    }
    void set_evaluated_type(string type)
    {
        evaluated_type = type;
    }
    vector<SymbolInfo*> get_child_identifiers()
    {
        return child_identifiers;
    }
    void set_child_identifiers(vector<SymbolInfo*> c_ids)
    {
        child_identifiers = c_ids;
    }

    TreeNode* get_parse_tree_node()
    {
        return parse_tree_node;
    }
    void set_boolean_properties(int properties)
    {
        boolean_properties = properties;
    }
    int get_boolean_properties()
    {
        return boolean_properties;
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
    void set_arr_size(int n)
    {
        arr_size = n;
    }
    int get_arr_size()
    {
        return arr_size;
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

    void add_child_identifier(SymbolInfo* child)
    {
        child_identifiers.push_back(child);
    }
    vector<string>get_parameter_types()
    {
        return parameter_types;
    }
    void add_parameter_type(string type)
    {
        parameter_types.push_back(type);
    }
    int get_parameter_count()
    {
        return (int)(parameter_types.size());
    }
    
};

#endif  // SYMBOLINFO_HEADER
