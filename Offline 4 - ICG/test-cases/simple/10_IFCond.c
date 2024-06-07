int a;
int main(){
 
	int i,j,k;
	
	i = 3;
	j = 8;
	k = 6;
    a = 1;
	
	
	if(i==3){
		println(j); // j=8
	}
	if(j==3)
    {
        println(i); // ignored
    }
    else
    {
        println(k); // k=6
    }
	if(a<=8){
		println(i); // i=3
	}
	else{
		println(k); // ignored
	}
	
	if(k != 6){
		println(k); // ignored
	}
	else if(j > 8){
		println(j); // ignored
	}
	else if(i < 5){
		println(i); // i=3
	}
	else{
		k = 0;
		println(k); // ignored
	}
 
 
	return 0;
}

