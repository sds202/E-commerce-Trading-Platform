import std;
import Shape_Module;

int main()
{
	double length, width, radius, side;
	std::cout << "请输入长和宽：\n";
	while (true)
	{
		using namespace std;
		try {
			cin >> length >> width;
			if (cin.fail())
				throw invalid_argument("无效输入,");
			if (length <= 0 || width <= 0)
				throw invalid_argument("输入非正数,");
			break;
		}
		catch (const invalid_argument& e) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cerr << e.what() << "请重新输入：" << endl;
		}
	}
	Rectangle rectangle{ length,width };

	std::cout << "请输入半径：\n";
	while (true)
	{
		using namespace std;
		try {
			cin >> radius;
			if (cin.fail())
				throw invalid_argument("无效输入,");
			if (radius <= 0)
				throw invalid_argument("输入非正数,");
			break;
		}
		catch (const invalid_argument& e) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cerr << e.what() << "请重新输入：" << endl;
		}
	}
	Circle circle{ radius };	

	std::cout << "请输入边长：\n";
	while (true)
	{
		using namespace std;
		try {
			cin >> side;
			if (cin.fail())
				throw invalid_argument("无效输入,");
			if (side <= 0)
				throw invalid_argument("输入非正数,");
			break;
		}
		catch (const invalid_argument& e) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cerr << e.what() << "请重新输入：" << endl;
		}
	}
	Square square{ side };

	std::cout << "矩形面积：" << rectangle.GetArea() << "\n";
	std::cout << "圆形面积：" << circle.GetArea() << "\n";
	std::cout << "正方形面积：" << square.GetArea() << std::endl;
}