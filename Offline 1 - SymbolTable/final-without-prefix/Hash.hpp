
// Guard against multiple declarations

#ifndef HASH_HEADER
#define HASH_HEADER

#include<iostream>
#include<string>
using namespace std;

// Hash Class for calculating hash function
// The hash function can be called as
// Hash.get_sdbm_hash(str)

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

#endif  // HASH_HEADER 