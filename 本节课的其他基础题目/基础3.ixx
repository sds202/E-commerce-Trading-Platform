export module Shape_Module;
import std;

export class Shape
{
public:
	Shape() 
	{ 
		std::cout << "\033[1;32m调用了Shape类的构造函数\033[0m" << std::endl; 
	}
	~Shape() 
	{
		std::cout << "\033[1;31m调用了Shape类的析构函数\033[0m" << std::endl; 
	}
	double GetArea()
	{ 
		return 0; 
	}
};

export class Rectangle :public Shape
{
public:
	Rectangle(double length, double width) :length{ length }, width{ width }
	{
		std::cout << "\033[1;32m调用了Rectangle类的构造函数\033[0m" << std::endl; 
	}
	~Rectangle() 
	{
		std::cout << "\033[1;31m调用了Rectangle类的析构函数\033[0m" << std::endl; 
	}
	double GetArea() 
	{
		return length * width;
	}
private:
	double length, width;
};

export class Circle :public Shape
{
public:
	Circle(double radius) :radius{ radius }
	{ 
		std::cout << "\033[1;32m调用了Circle类的构造函数\033[0m" << std::endl; 
	}
	~Circle() 
	{ 
		std::cout << "\033[1;31m调用了Circle类的析构函数\033[0m" << std::endl; 
	}
	double GetArea()
	{ 
		return std::numbers::pi * radius * radius;
	}
private:
	double radius;
};

export class Square :public Rectangle
{
public:
	Square(double side) :Rectangle{ side,side }
	{ 
		std::cout << "\033[1;32m调用了Square类的构造函数\033[0m" << std::endl; 
	}
	~Square() 
	{ 
		std::cout << "\033[1;31m调用了Square类的析构函数\033[0m" << std::endl; 
	}
};