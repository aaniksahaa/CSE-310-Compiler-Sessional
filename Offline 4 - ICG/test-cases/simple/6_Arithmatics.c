int a, b, c;
int main()
{
    int i, j, k;
    i = 2;
    j = 4;
    c = 3;
    a = (i -j) * (i + j); 
    println(a); // a=-12
    b = ((a%c)+i)*4-2*j;
    println(b); // b=0
}