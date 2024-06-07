/*

24
63
22
46
2
4
1
0
67
95
1
11
111
1111
111
11
1
78
720
515
104
103
5
-1
12
1
102

*/

/*
void println(int a){
    printf("%d\n",a);
}
*/

int fact(int n){
    if(n == 0){
        return 1;
    }
    return fact(n-1)*n;
}

int x;
void h();
int foo(int a, int b);

int foo(int a, int b){
	if(a+b <= 5){
		return 7;
	}
	return foo(a-2,b-1) + 2*foo(a-1,b-2);
}

int y[5];

void h(){
    int y;
    y = 22;
    println(y);
}

void g(){

}

int gg(int a, int b){
    return a%b;
}
int i;
void hh(){
    for(i=0; i<=5; i++){
        y[i] = 101 + i;
    }
}

int f2(){
    int y;
    return 1;
    int u[5];
    return 2;
    int v[10];
    return 3;
}

int func (int n){
  int i;
  if (n==0) return 0;
  i=n;
  return func(n-1)+i;
}

int func2 (int n){
  int i;
  if (n==0) return 0;
  i=n;
  return func(n-1)+i;
}

int main(){
    int a,b,c,d,e,n,p,i,j,k;
    a = fact(4);
    println(a);
    b = foo(14/2, -15%3 + 3);
    println(b);
    h();
    int arr[5];

    for(i=0;i<5;i++){
        y[i] = i;
    }
    j = 4;
    while(j>=0){
        arr[j] = y[j];
        j--;
    }
    int s;
    s = 0;
    for(i=0; i<5; i++){
        {
            {
                if(arr[i] % 2 == 0){
                    {
                        s = s + y[i];
                    }
                }
                else{
                    s = s + 10*y[i];
                }
            }
        }
    }

    println(s);

    g();
    g();

    a = 2;
    if(a <= 3){
        int b;
        b = 4;
        if(b >= 2){
            {
                int u;
                u = 7;
                {
                    int v;
                    for(v=1; v<=5; v++){
                        if(v % 2 == 0){
                            println(v);
                            {
                                {

                                }
                            }
                        }
                    }
                }
            }
        }
    }

    p = f2();
    println(p);

    p = !(!(a > 15 && (b <= 50 || c == 67)));
    println(p);
    if( !(!(a > 15 && (b <= 50 || c == 67))) ){
        println(a);
    } else{
        int c;
        c = 67;
        {
            {
                println(c);
            }
        }
    }

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

    d = 78;
    if(!gg(2 + 2 , ((6 > 5) + (6 < 7)) * ( 2*3 >= 7-1))){
        println(d);
    }

    d = fact(fact(3));

    println(d);

    hh();

    p=0;
    j = 0;
    while(j<5){
        p = p + y[j];
        j++;
    }

    println(p);

    p = y[fact(3)+(-12)%3-1*3/1];

    println(p);

    p = y[y[y[2]-101]-101];

    println(p);

    int y[100];

    y[0] = 5;
    y[5] = 0;

    p = y[y[y[0]]];

    println(p);

    p = func(7);

    println(p);

    b=0;
	c=1;
    for(i=0;i<4;i++){
        a=3;
        while(a--){
            b++;
        }
    }
    println(a);
    println(b);
    println(c);

    a = 3;
    b = 100;
    c = 101;
    d = 102;
    e = 103;

    if(a == 1){
        println(b);
    } else if(a == 2){
        println(c);
    } else if(a == 3){
        println(d);
    } else{
        println(e);
    }


    return 0;
}
