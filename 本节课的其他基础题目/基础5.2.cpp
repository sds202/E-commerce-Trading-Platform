import std;
import Point_Module;

int main()
{
	double x, y;
	std::cout << "ÇëÊäÈë×ø±ê£º\n";
	std::cin >> x >> y;

	Point p1(x, y);
	
	std::cout << p1 << '\n';
	std::cout << p1++ << ' ' << p1 << '\n';
	std::cout << ++p1 << ' ' << p1 << '\n';
	std::cout << p1-- << ' ' << p1 << '\n';
	std::cout << --p1 << ' ' << p1 << std::endl;
}