int main(){ 
	int i;
	if(i>0 || i<10)
		i=100;
	else 
		i=200;
	
	if(i>20 && i<30)
		i=300;
	else 
		i=400;
	
	if((i>40 && i<50) || (i<60 && i>70))
		i=500;
	else 
		i=600;
	println(i);	
	
	return 0;
}

