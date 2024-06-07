int a;
int main()
{

    int i;
    a = 0;
    i = 5;
    while (i--)
    {
        a = a + 2;
        println(a); // a=2,4,6,8,10
    }
    while(a>5)
    {
        i = a - 1;
        println(i); // i=9,8,7,6,5
        a--;
    }
}