/*

11
0
55
6
6
66

*/

/*
void println(int a){
    printf("%d\n",a);
}
*/

int f(int a, int b){
    return a+b;
}

int g(int a, int b){
    return a%b;
}

int main()
{
    int a,b,c,d,p;
    a = 10;
    b = 1;
    c = 55;
    d = 66;
    p = 77;
    if(f(a,b) - 11){
        println(a);
    }
    if(a++){
        println(a);
    }
    if(b--){
        println(b);
    }
    if(b++){
        println(p);
    }
    if(2 + 2){
        println(c);
    }
    if(2 - 2){
        println(d);
    }
    if(a = (b = 3 + 5 - 2*(!0))){
        println(b);
        println(a);
    }

    if(!g(2 + 2 , ((6 > 5) + (6 < 7)) * ( 2*3 >= 7-1))){
        println(d);
    }

}
