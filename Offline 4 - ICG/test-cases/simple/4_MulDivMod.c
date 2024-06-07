int a, b, c;
int main()
{
    int i, j, k;
    i = 2;
    j = 4;
    c = 3;
    a = i * j;
    b = j / i;
    println(a); // a=8
    println(b); // b=2
    k = a / b;
    println(k); // k=4
    k = k % c;
    println(k); // k=1
}