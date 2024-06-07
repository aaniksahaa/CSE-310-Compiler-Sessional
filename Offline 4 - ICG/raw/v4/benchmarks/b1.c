/*

1
23
1
1
23
68
24
54

*/

/*
void println(int a){
    printf("%d\n",a);
}
*/

int main()
{
    int a,b,c,d,p;
    a = 23;
    b = 54;
    c = 67;
    d = 0;
    p = !(!(a > 15 && (b <= 50 || c == 67)));
    println(p);
    if( !(!(a > 15 && (b <= 50 || c == 67))) ){
        println(a);
    }
    p = a >= 25 || b == 54;
    println(p);
    p = a != 53 && b < 60;
    println(p);
    p = a + !b;
    println(p);
    p = c + !d;
    println(p);
    p = a + (!d)*(!d);
    println(p);

    if(a + (!d)*(!d)){
        println(b);
    }

}
