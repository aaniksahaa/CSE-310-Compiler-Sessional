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

int main() {
	int a;
	a = func(7);
	println(a);
	return 0;
}

