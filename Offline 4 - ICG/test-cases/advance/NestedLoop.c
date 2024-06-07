int main(){
    int a,b,c,i;
    b=0;
	c=1;
    for(i=0;i<4;i++){
        a=3;
        while(a--){
            b++;
        }
    }
    println(a); //a=-1
    println(b); //b=12
    println(c); //c=1
}
