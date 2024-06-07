int i,j,k[10];
int main(){
 
	int a[5];
    int b;
    a[1] = 3;
    a[3] = 4;
    k[6] = 5;
    i = a[1];
    j = a[3];
    b = k[6];
    a[2]=k[6];
    k[7]=a[1]++;
    k[0]= a[1] + a[3];
    println(i); // i=3
    println(j); // j=4
    println(b); // b=5
    i = a[2];
    println(i); // i=5
    i = k[7];
    println(i); // i=3
    i = k[0];
    println(i); // i=8

}
