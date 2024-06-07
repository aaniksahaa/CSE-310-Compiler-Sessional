int a, b, c;
int main()
{
    int i, j, k;
    i = 2;
    j = 4;
    c = 3;
    a = i * j * c;
    println(a);  // a=24
    b = a / j / i;
    println(b);  // b=3
}