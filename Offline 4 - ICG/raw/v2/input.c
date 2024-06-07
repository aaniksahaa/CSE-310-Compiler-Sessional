int f(int a){
    return 2*a;
}

int g(int a, int b){
    int x;
    x=f(a)+a+b;
    return x;
}

int main(){
    int a,b,c[5],d;
    a=1;
    b=1*(2+3)%3;
    a=g(a,b);
    println(a);
    return 0;
}
