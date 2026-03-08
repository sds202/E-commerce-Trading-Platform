export module Point_Module;
import std;

//接口
export class Point
{
public:
	Point(double x, double y) :x{ x }, y{ y } {};
	Point& operator++();	//Prefix
	Point operator++(int);	//Postfix
	Point& operator--();
	Point operator--(int);
	friend std::ostream& operator<<(std::ostream& ostr, const Point& point);
private:
	double x;
	double y;
};

//实现
Point& Point::operator++()
{
	++x;
	++y;
	return *this;
}
Point Point::operator++(int)
{
	Point oldPoint(*this);
	++(*this);
	return oldPoint;
}
Point& Point::operator--()
{
	--x;
	--y;
	return *this;
}
Point Point::operator--(int)
{
	Point oldPoint(*this);
	--(*this);
	return oldPoint;
}

export std::ostream& operator<<(std::ostream& ostr, const Point& point)
{
	ostr << '(' << point.x << ',' << point.y << ')';
	return ostr;
}