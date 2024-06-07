int a, b, c;
int main()
{
    int i, j, k;
    i = 2;
    j = 4;
    a = 7;
    b = 7;
    k = i > j;
    println(k); // k=0
    k = i < a;
    println(k); // k=1
    k = a == b;
    println(k); // k=1
    k = a>=b;
    println(k); // k=1
    k = a<=b;
    println(k); // k=1
    k = i!=j;
    println(k); // k=1
    c = 0;
    k = a && c;
    println(k); // k=0
    k = a || c;
    println(k); // k=1
}