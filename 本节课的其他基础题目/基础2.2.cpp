import std;
import Matrix_Module;

int main()
{
	int m, n;

	try {
		std::cout << "请输入矩阵A1的行数和列数：\n";
		std::cin >> m >> n;
		Matrix A1{ m,n };
		A1.InputMatrix();

		std::cout << "请输入矩阵A2的行数和列数：\n";
		std::cin >> m >> n;
		Matrix A2{ m,n };
		A2.InputMatrix();

		Matrix A3;

		std::cout << "计算A1+A2\n";
		A3 = A2 + A1;
		A3.OutputMatrix();
		std::cout << "计算A1-A2\n";
		A3 = A1 - A2;
		A3.OutputMatrix();


		std::cout << "请输入矩阵pA1的行数和列数：\n";
		std::cin >> m >> n;
		Matrix* pA1{ new Matrix(m,n) };
		pA1->InputMatrix();

		std::cout << "请输入矩阵pA2的行数和列数：\n";
		std::cin >> m >> n;
		Matrix* pA2{ new Matrix(m,n) };
		pA2->InputMatrix();

		Matrix* pA3{ new Matrix() };

		std::cout << "计算pA1+pA2\n";
		*pA3 = *pA2 + *pA1;
		pA3->OutputMatrix();
		std::cout << "计算pA1-pA2\n";
		*pA3 = *pA1 - *pA2;
		pA3->OutputMatrix();

		delete pA1, pA2, pA3;
	}
	catch(const std::logic_error& e){
		std::cerr << "错误：" << e.what() << std::endl;
		return -1;
	}
	catch (const std::bad_alloc& e) {
		std::cerr << "错误：" << e.what() << std::endl;
		return -1;
	}
}