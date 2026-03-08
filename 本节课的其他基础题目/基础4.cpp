import std;

int main()
{
	std::random_device rnd;
	std::uniform_int_distribution<int> distribution{ 1,1000 };

	int price{ distribution(rnd)};
	std::string input;
	int input_num;

	std::cout << "请输入一个1~1000之间的整数\n";
	do
	{
		std::cin >> input;
		std::stringstream ss(input);

		if (!(ss >> input_num) || !ss.eof())
		{
			std::cout << "格式错误，请输入整数\n"; 
			continue;
		}
		if (input_num < 1 || input_num > 1000)
		{
			std::cout << "超出了1~1000的范围\n";
			continue;
		}
		if (input_num > price)
			std::cout << "猜大了\n";
		else if(input_num < price)
			std::cout << "猜小了\n";
	} while (input_num != price);

	std::cout << "猜中了\n";
}