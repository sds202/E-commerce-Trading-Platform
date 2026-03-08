export module Circle_Module;
import std;

//接口
export class Point
{
public:
	Point(double x, double y)
		:x{ x }, y{ y }
	{
		std::cout << std::format("\033[1;32m调用了Point类的构造函数，构造点({},{})\033[0m",x,y)<< std::endl;
	};
	~Point() 
	{
		std::cout << std::format("\033[1;31m调用了Point类的析构函数，析构点({},{})\033[0m",x,y) << std::endl;
	};
	double CalculateDistance(const Point& point);
	double ReturnX()const { return x; };
	double ReturnY()const { return y; };
private:
	double x;
	double y;
};

export class Circle
{
public:
	Circle(double x, double y, double r)
		:center{ x, y }, radius{ r }
	{
		std::cout << std::format("\033[1;32m调用了Circle类的构造函数，构造圆心({},{})，半径{}\033[0m",x,y,r) << std::endl;
	}
	~Circle()
	{
		std::cout << std::format("\033[1;31m调用了Circle类的析构函数，析构圆心({},{})，半径{}\033[0m",center.ReturnX(), center.ReturnY(), radius) << std::endl;
	};
	void IsIntersectant(const Circle& circle);
private:
	Point center;
	double radius;
};

export void GetInput(double& x, double& y, double& r);

//实现
double Point::CalculateDistance(const Point& point)
{
	using namespace std;
	return sqrt((x - point.x)* (x - point.x)+ (y - point.y)*(y - point.y));
}
void Circle::IsIntersectant(const Circle& circle)
{
	using namespace std;
	cout << format("两圆坐标分别为({},{})({},{})，半径分别为{},{}",
		this->center.ReturnX(),
		this->center.ReturnY(),
		circle.center.ReturnX(),
		circle.center.ReturnY(),
		this->radius,
		circle.radius) << endl;
	double distance = center.CalculateDistance(circle.center);
	double sumOfRadius = radius + circle.radius;
	double diffOfRadius = abs(radius - circle.radius);
	if (distance<=sumOfRadius && distance>=diffOfRadius)
		cout << "两圆相交" << endl;
	else
		cout << "两圆不相交" << endl;
}
void GetInput(double& x, double& y, double& r)
{
	using namespace std;
	x = 0;
	y = 0;
	r = 0;
	cout << "请输入坐标，默认为(0,0)\n";
	string tmp;
	getline(cin, tmp);
	if (!tmp.empty())
	{
		stringstream ss{ tmp };
		ss >> x >> y;
	}
	cout << "请输入半径\n";
	try
	{
		cin >> r;
		if (r <= 0)
			throw invalid_argument("半径要求是正数\n");
	}
	catch (const invalid_argument& e)
	{
		r = 1;
		cerr << e.what()<<"已经自动设置为1" << endl;
	}
	
	cin.get();
}
