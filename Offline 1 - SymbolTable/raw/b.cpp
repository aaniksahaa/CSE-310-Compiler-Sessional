
#include <bits/stdc++.h>
using namespace std;
#include "a.cpp"

/*** Definitions ***/

#define gap " "
#define br "\n"
#define pb push_back
#define all(a) a.begin(),a.end()
#define int long long int
#define fo(i,n) for(int i=0;i<n;i++)
#define pii pair<int,int>
#define double long double
#define print(x) cout<< #x <<" = "<< x<<endl

/*** Constants ***/

const int inf = LLONG_MAX;

/*** Random Number Genertor ***/

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

void initialize_random_seed(int seed)
{
	rng.seed(seed);
}

int random(int l,int r) {
	return uniform_int_distribution<int>(l, r)(rng);
}

int32_t main()
{
	Test1 t(5);
	t.show();
}