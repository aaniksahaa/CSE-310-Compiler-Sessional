#include "cc/c.cpp"

class Test{
	int a;
public:
	Test(int p)
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
		cout<<a<<endl;
	}
};