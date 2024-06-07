
// Guard against multiple declarations

#ifndef PARSETREE_HEADER
#define PARSETREE_HEADER

#include<iostream>
#include<string>
#include<vector>

using namespace std;

class TreeNode{
public:
    string name, type;
    int start_line_count, end_line_count;
    vector<TreeNode*> childrenNodes;
    bool syntax_error_occurred = 0;
    
    TreeNode(string n="", string t="", int s=0, int e=0)
    {
        name = n;
        type = t;
        start_line_count = s;
        end_line_count = e;
    }
    void setChildrenNodes(size_t numChildren, ...) {
        va_list args;
        va_start(args, numChildren);

        for (size_t i = 0; i < numChildren; ++i) {
            TreeNode* child = va_arg(args, TreeNode*);
            childrenNodes.push_back(child);
        }

        va_end(args);
    }
    string get_text()
    {
        string line_text;
        string ret = "";
        string content = type + string(" : ") + name;
        if( childrenNodes.empty() or syntax_error_occurred )  // leaf
        {
            line_text = "<Line: " + to_string(start_line_count) + ">";
            ret = content + string("\t") + line_text;
        }
        else
        {
            line_text = "<Line: " + to_string(start_line_count) + string("-") + to_string(end_line_count) + ">";
            ret = content + string(" \t") + line_text;
        }
        return ret;
    }
    string print_subtree(int space_count)  // gives the text form of the subtree rooted at this node
    {
        string space = " ";
        string subtree = "";
        for(int i=0; i<space_count; i++)
        {
            subtree += space;
        }
        subtree += (get_text() + string("\n"));
        if(syntax_error_occurred)
        {
            // don't expand further
        }
        else
        {
            for(const auto& child : childrenNodes)
            {
                subtree += child->print_subtree(space_count+1);
            }
        }
        return subtree;
    }

};

class ParseTree{
    TreeNode* root;
public:
    ParseTree(TreeNode* root)
    {
        this->root = root;
    }
    string print_tree()
    {
        return root->print_subtree(0);
    }
};

#endif  // PARSETREE_HEADER
