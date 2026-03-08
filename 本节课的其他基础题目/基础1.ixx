export module Matrix;
import std;

//接口部分
export using Matrix = int**;
using ConstMatrix = int const* const*;

export Matrix AllocMatrix();
export void InitMatrix(Matrix matrix);
export void OutputMatrix(ConstMatrix matrix);
export void AddMatrix(ConstMatrix A, ConstMatrix B, Matrix result);
export void SubMatrix(ConstMatrix A, ConstMatrix B, Matrix result);
export void DeleteMatrix(Matrix& matrix);

//实现部分
Matrix AllocMatrix()
{
	using namespace std;

	try {
		int* data{ new int[4 * 5] };
		Matrix matrix{ new int* [4] };
		for (int i{ 0 }; i < 4; i++)
			matrix[i] = data + (i * 5);
		return matrix;
	}
	catch (const bad_alloc& e){
		cerr <<"内存分配错误 " << e.what() << endl;
		exit(1);
	}
}

void InitMatrix(Matrix matrix)
{
	using namespace std;

	while (true)
	{
		try {
			for (int i{ 0 }; i < 4; i++)
				for (int j{ 0 }; j < 5; j++)
					cin >> matrix[i][j];
			if (cin.fail()) 
				throw invalid_argument("无效输入，您输入的 不是数字 或者 数字超出int类型表示范围\n");
			cout << endl;
			break;
		}
		catch(const invalid_argument& e){
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cerr << e.what()<<"\n请重新输入：" << endl;
		}
	}
}

void OutputMatrix(ConstMatrix matrix)
{
	using namespace std;
	for (int i{ 0 }; i < 4; i++)
	{
		cout << "|";
		for (int j{ 0 }; j < 5; j++)
			cout <<right<< setw(11) << matrix[i][j] << " ";
		cout << "|\n";
	}
	cout << endl;
}

void AddMatrix(ConstMatrix A, ConstMatrix B, Matrix result)
{
	for (int i{ 0 }; i < 4; i++)
		for (int j{ 0 }; j < 5; j++) {
			result[i][j] = A[i][j] + B[i][j];
		}
}

void SubMatrix(ConstMatrix A, ConstMatrix B, Matrix result)
{
	for (int i{ 0 }; i < 4; i++)
		for (int j{ 0 }; j < 5; j++) {
			result[i][j] = A[i][j] - B[i][j];
		}
}

void DeleteMatrix(Matrix& matrix)
{
	delete[] matrix[0];
	delete[] matrix;
	matrix = nullptr;
}
