#include<bits/stdc++.h>
#include "headers/2005001_SymbolTable.hpp"

using namespace std;

class B;

class A{
public:
    int a;
    B* hasB;
    A(int p){
        a = p;
    }
};

class B{
public:
    int b;
    A* hasA;
    B(int p){
        b = p;
    }
};

int main(){
    A* a1 = new A(2);
    B* b1 = new B(3);
    a1->hasB = b1;
    b1->hasA = a1;
    cout<<a1->hasB->hasA->a<<endl;
}