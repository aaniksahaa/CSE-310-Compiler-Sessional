
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

#define EMPTY_CONSTANT_TEXT string("#")

#define IS_LEAF 0
#define IS_FUNCTION 1
#define IS_ARRAY 2
#define ERROR_OCCURED 3
#define IS_DECLARED_FUNCTION 4
#define IS_DEFINED_FUNCTION 5
#define SYNTAX_ERROR_OCCURRED 6
#define IS_LOCAL_VARIABLE 7

// determines whether the actual result of an expression is boolean (0/1) or not
#define IS_VALUE_BOOLEAN 8


class SymbolInfo
{
public:
    string name, type;

    bool verbose_destructor;

    int boolean_properties;     // open for extension  // stores boolean properties in individual bits
    TreeNode* parse_tree_node;

    // evaluated type is stored for all SymbolInfo Objects
    string evaluated_type;                  // return type for functions and type for expressions - what type the evaluated expression will be...
    vector<SymbolInfo*>child_identifiers;   // parameters for functions
    vector<string>parameter_types;
    vector<string>argument_types;
    int arr_size;
    string constant_text;    // only for CONST_INT or CONST_FLOAT

    SymbolInfo *next;


    // 

    vector<SymbolInfo*>child_symbols;
    string display_name;
    int stack_starting_position;        // signed position wrt SP

    // for labels in icg

    int true_label_id, false_label_id, next_label_id;

    //

    int return_label_id;

    //

    bool is_boolean_conditional;
    bool is_boolean_conversion_required;
    bool is_boolean_labels_defined;

    //

    int scope_local_space;


    SymbolInfo(string name, string type, SymbolInfo* next = NULL, bool verbose_destructor = false)
    {
        this->name = name;
        this->type = type;
        this->next = next;

        this->verbose_destructor = verbose_destructor;

        this->parse_tree_node = new TreeNode(name,type);  // line numbers not yet known...  //  will be set to 0 as default
        boolean_properties = 0;
        arr_size = 1;
        constant_text = EMPTY_CONSTANT_TEXT;
        evaluated_type = string("DEFAULT");

        //

        stack_starting_position = -1; // default

        //

        true_label_id = false_label_id = next_label_id = -1;

        //

        return_label_id = -1;

        //

        is_boolean_conditional = false;
        is_boolean_conversion_required = true;
        is_boolean_labels_defined = false;

        //

        scope_local_space = 0;
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
        parse_tree_node->name = name;
    }
    string get_type()
    {
        return type;
    }
    void set_type(string type)
    {
        this->type = type;
        parse_tree_node->type = type;
    }

    // For debugging
    void show()
    {
        cout<<endl;
        cout<<"Name = "<<name<<endl;
        cout<<"Type = "<<type<<endl;
        cout<<endl;
    }
    string get_string()
    {
        string ret = string("Name = ") + name + string(" Type = ") + type;
        return ret;
    }

    //functions regarding additional info
    void set_line_counts(int s, int e)   // updates tree node automatically
    {
        parse_tree_node->start_line_count = s;
        parse_tree_node->end_line_count = e;
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
    void set_constant_text(string t)
    {
        constant_text = t;
    }
    string get_constant_text()
    {
        return constant_text;
    }

    void set_children_tree_nodes(size_t numChildren, ...) {
        va_list args;
        va_start(args, numChildren);

        for (size_t i = 0; i < numChildren; ++i) {
            SymbolInfo* childSymbol = va_arg(args, SymbolInfo*);
            parse_tree_node->childrenNodes.push_back(childSymbol->get_parse_tree_node());

            //

            child_symbols.push_back(childSymbol);
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
    vector<string>get_argument_types()
    {
        return argument_types;
    }
    void set_argument_types(vector<string>types)
    {
        argument_types.clear();
        argument_types = types;
    }
    void add_argument_type(string type)
    {
        argument_types.push_back(type);
    }
    int get_argument_count()
    {
        return (int)(argument_types.size());
    }
    void set_syntax_error()
    {
        set_property(SYNTAX_ERROR_OCCURRED);
        set_name("error");
        parse_tree_node->syntax_error_occurred = 1;
    }

    //

    string get_display_name()
    {
        return display_name;
    }
    void set_display_name(string s)
    {
        display_name = s;
    }
    
};

#endif  // SYMBOLINFO_HEADER
