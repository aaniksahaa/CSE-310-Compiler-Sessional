/*

1
11
111
1111
111
11
1

*/

/*
void println(int a){
    printf("%d\n",a);
}
*/

int main()
{
    int n,m,i,j,k,p;
    n = 4;
    for(i=1; i<=n; i++){
        p = 0;
        j = 1;
        while(j <= i){
            p = p*10 + 1;
            j++;
        }
        println(p);
    }
    for(i=n-1; i>=1; i--){
        p = 0;
        j = 1;
        while(j <= i){
            p = p*10 + 1;
            j++;
        }
        println(p);
    }
}
