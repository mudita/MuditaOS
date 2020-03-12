#include <iostream>


class Abc {
	int x_=0;
	int y_=0;
	public:
	Abc() = default;
		Abc(int x,int y):
		x_(x),y_(y){}
		int x(){return x_;}
		int y(){return y_;}
};

int main() {
Abc abc_00;
		Abc abc_49(4,9);
  std::cout << "abc_00" << std::endl;
  std::cout << "x:" << abc_00.x() << "|y:" << abc_00.y() << std::endl;
  std::cout << "x:" << abc_49.x() << "|y:" << abc_49.y() << std::endl;
return 0;
}
