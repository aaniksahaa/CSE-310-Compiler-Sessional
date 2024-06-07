/*

101
2
103
4
95

*/

/*
void println(int a){
    printf("%d\n",a);
}
*/

int main()
{
    int a,b;
    a = 1;
	while(a < 5){
        if(a %2 == 0){
            println(a);
        } else {
            b = a + 100;
            println(b);
        }
        a++;
	}

    int p;
    p = 0;

    for(a = 1; a <= 5; a++){
        int c;
        c = 0;
        b = 1;
        while(b<=a){
            c = c + b;
            if(b%2 == 0){
                c = c + 10;
            }
            b++;
        }
        p = p + c;
    }

    println(p);
}