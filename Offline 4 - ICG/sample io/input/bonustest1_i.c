int foo(int a, int b){
	if(a+b <= 5){
		return 7;
	}
	return foo(a-2,b-1) + 2*foo(a-1,b-2);
}
 
 
int main(){
	int i,j,k;
	i = 7;
	j = 3;
 
	k = foo(i,j);
	println(k);
 
	return 0;
}
