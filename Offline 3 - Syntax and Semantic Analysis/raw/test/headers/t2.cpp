
#include <bits/stdc++.h>

#include "2005001_SymbolTable.hpp"
//#include "2005001_ParseTree.hpp"

using namespace std;

/*** Definitions ***/

#define gap " "
#define fo(i,n) for(int i=0;i<n;i++)
#define pii pair<int,int>
#define double long double
#define debug(x) cout<< #x <<" = "<< x<<endl

/*** Constants ***/

const int inf = INT_MAX;

/*** Random Number Generator ***/

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

void initialize_random_seed(int seed)
{
	rng.seed(seed);
}

int random(int l,int r) {
	return uniform_int_distribution<int>(l, r)(rng);
}

/*** Helper Functions ***/

bool fileExists(const string& filename) {
    ifstream file(filename);
    return file.good();
}


class TreeNode{
public:
    string content;
    int start_line_count, end_line_count;
    vector<TreeNode*> childrenNodes;
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

/*** Main Function ***/


int main()
{
    
    TreeNode* root = new TreeNode("Root", 1, 5);
    
    TreeNode* a = new TreeNode("a", 2, 3);
    TreeNode* b = new TreeNode("b", 4, 4);
    TreeNode* c = new TreeNode("c", 5, 7);
    TreeNode* d = new TreeNode("d", 8, 8);

    TreeNode* a1 = new TreeNode("a1", 3, 3);
    TreeNode* a2 = new TreeNode("a2", 4, 4);

    TreeNode* b1 = new TreeNode("b1", 5, 5);

    TreeNode* c1 = new TreeNode("c1", 6, 6);
    TreeNode* c2 = new TreeNode("c2", 7, 7);
    TreeNode* c3 = new TreeNode("c3", 8, 8);

    TreeNode* d1 = new TreeNode("d1", 9, 9);

    TreeNode* x = new TreeNode("x", 10, 10);
    TreeNode* y = new TreeNode("y", 11, 11);
    TreeNode* z = new TreeNode("z", 12, 12);

    // Setting up the tree structure
    root->setChildrenNodes(4, a, b, c, d);
    a->setChildrenNodes(2, a1, a2);
    b->setChildrenNodes(1, b1);
    c->setChildrenNodes(3, c1, c2, c3);
    d->setChildrenNodes(1, d1);
    c1->setChildrenNodes(3, x, y, z);

    // Access childrenNodes to verify
    // for (const auto& child : root->childrenNodes) {
    //     std::cout << "Child: " << child->text << std::endl;
    // }

    ParseTree tree(root);

    cout<<tree.print_tree();
	
}
