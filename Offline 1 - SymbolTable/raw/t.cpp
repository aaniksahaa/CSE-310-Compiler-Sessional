
#include <bits/stdc++.h>
using namespace std;

/*** Definitions ***/

#define gap " "
#define br "\n"
#define pb push_back
#define all(a) a.begin(),a.end()
#define fo(i,n) for(int i=0;i<n;i++)
#define pii pair<int,int>
#define double long double
#define print(x) cout<< #x <<" = "<< x<<endl

/*** Constants ***/

/*** Random Number Genertor ***/

mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());

void initialize_random_seed(int seed)
{
	rng.seed(seed);
}

int random(int l,int r) {
	return uniform_int_distribution<int>(l, r)(rng);
}

class B{
public:
	int b;
	B* nextB;
	B(int p, B* nb = NULL){
		b = p;
		nextB = nb;
	}
	~B(){
		delete nextB;
	}
	void show(){
		print(b);
		if(nextB != NULL)
		{
			nextB->show();
		}
	}
};

class A{
public:
	int a;
	B* next;
	A(int p, B* np)
	{
		a = p;
		next = np;
	}
	~A(){
		delete next;
	}
	void show(){
		cout<<br;
		print(a);
		//next->show();
		cout<<br;
	}
};

class Hash
{
public:
	// here int is actually long long int, so it can fit unsigned long as per original definition
	static uint64_t get_sdbm_hash(string key_string)
	{
		uint64_t hash = 0;
		int len = key_string.length();
		for(int i=0; i<len; i++)
		{
			char ch = key_string[i];
			int c = ch;               // casting into int
			hash = c + (hash<<6) + (hash<<16) - hash;
		}
		return hash;
	}

};

static unsigned int sdbm(unsigned char *str)
{
    unsigned int hash = 0;
    int c;

    while (c = *str++)
    {
        hash = c + (hash << 6) + (hash << 16) - hash;
    }

    print(hash);

    return hash;
}

int32_t main()
{
	string s = "	";
					
}