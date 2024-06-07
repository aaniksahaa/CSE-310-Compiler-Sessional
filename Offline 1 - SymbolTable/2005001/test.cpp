
#include <bits/stdc++.h>

#include "2005001_SymbolTable.hpp"

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

void show(ofstream& out)
{
    out<<"Hello from function\n";
}

/*** Main Function ***/

int main()
{
    cout<<"Hello";
    ofstream out("hello.txt");
    out<<"Hello\n";
    show(out);
}
