class Test1{
	int a;
public:
	Test1(int p)
	{
		a = p;
	}
	int geta()
	{
		return a;
	}
	void seta(int p)
	{
		a = p;
	}
	void show()
	{
		cout<<a*a<<endl;
	}
};