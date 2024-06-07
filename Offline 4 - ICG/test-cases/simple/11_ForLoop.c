int a;
int main(){
 
	int i;
    a = 0;
 
	for(i=0;i<6;i++){
        a = a + i;
		println(i); // i=0,1,2,3,4,5
        println(a); // a=0,1,3,6,10,15
	}
}