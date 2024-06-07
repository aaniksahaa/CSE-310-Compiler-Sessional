#include<bits/stdc++.h>
#include "headers/2005001_SymbolInfo.hpp"

using namespace std;

ofstream logout("log.txt");

class Writer
{
    ofstream& out;
public:
    Writer(ofstream& outFile) : out(outFile)
    {

    }
    void write_grammar_rule(SymbolInfo* info)
    {
        out<<info->get_type()<<"\t: "<<info->get_name()<<" \n";
    }
    void write_grammar_rule(string rule) // overridden for directly pasting as per testcase
    {
        out<<rule<<" \n";
    }
};

int main()
{
    SymbolInfo* info = new SymbolInfo("INT", "type_specifier");
    Writer logwriter(logout);
    logwriter.write_grammar_rule(info);
    logwriter.write_grammar_rule("var_declaration : type_specifier declaration_list SEMICOLON");
}