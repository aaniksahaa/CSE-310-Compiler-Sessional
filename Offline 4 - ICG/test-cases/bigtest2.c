/*

1
3
5
4
3
2
1
3
-1
-116
-16
4
3
111
222
333
441
111
5
1
1
1
0
0
1
20

*/

/*
void println(int a){
    printf("%d\n",a);
}
*/

int a, arr[5];
int f(int a, int b, int c)
{
    println(a);
    println(b);
    println(c);
    arr[1] = 441;
    arr[0] = 555;
    b = arr[1];
    println(b);
    return arr[0];
}
int recursive(int n)
{
    if (n == 1)
        return 1;
    if (n == 0)
        return 0;
    return recursive(n - 1) + recursive(n - 2);
}
void v()
{
    a = 3;
    if (a)
    {
        int a;
        a = 1;
        println(a);
    }
    println(a);
}
int main()
{
    int i, j, k, a, array[5];
    a = 5;
    v();
    println(a);
    for (i = 0; i < 5; i++)
    {
        array[i] = i + 1;
    }
    i = 4;
    while (i--)
    {
        j = array[i];
        println(j);
    }
    k = 2;
    if (k > 0)
        k++;
    else
        k--;
    println(k);
    k = -2;
    if (k < 0)
        k++;
    else
        k--;
    println(k);
    k = 121;
    k = -k;
    i = 5;
    k = i + k;
    println(k);
    k = -4;
    k = k * 4;
    println(k);
    j = 19;
    i = 4;
    k = j / i;
    println(k);
    k = j % i;
    println(k);
    k = f(111, 222, 333) - 444;
    println(k);
    k = recursive(5);
    println(k);
    k = 2;
    i = 1;
    j = i || k;
    println(j);
    j = i && k;
    println(j);
    k = 2;
    i = 0;
    j = i || k;
    println(j);
    j = i && k;
    println(j);
    j = !k;
    println(j);
    j = !j;
    println(j);
    j = 12 + 2 / 89 % (3 - 33 + 64 * 2) - 3 + (3 + 59 / 9 * 2 - 4);
    println(j);

    return 0;
}
