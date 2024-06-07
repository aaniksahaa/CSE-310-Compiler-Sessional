
// Guard against multiple declarations

#ifndef PARSETREE_HEADER
#define PARSETREE_HEADER

#include<iostream>
#include<string>
#include<vector>

using namespace std;

class TreeNode{
public:
    string content;
    int start_line_count, end_line_count;
    vector<TreeNode*> childrenNodes;
    TreeNode()
    {
        content = "";
        start_line_count = 0;
        end_line_count = 0;
    }
    TreeNode(string c, int s, int e)
    {
        content = c;
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
        if(childrenNodes.empty())  // leaf
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
        for(const auto& child : childrenNodes)
        {
            subtree += child->print_subtree(space_count+1);
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
