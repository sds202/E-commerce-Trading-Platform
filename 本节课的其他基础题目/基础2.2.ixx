export module Matrix_Module;
import std;

//接口部分
export class Matrix {
public:
	Matrix() :rows{ 0 }, columns{ 0 }, value{ nullptr } {};
	Matrix(int rows, int columns);
	Matrix(const Matrix& matrix);
	Matrix(Matrix&& src) noexcept;
	~Matrix() = default;
	Matrix& operator=(const Matrix& rhs);
	Matrix& operator=(Matrix&& ths) noexcept;
	Matrix operator+(const Matrix& matrix);
	Matrix operator-(const Matrix& matrix);
	void InputMatrix();
	void OutputMatrix();
	
private:
	int rows;
	int columns;
	std::unique_ptr<std::unique_ptr<double[]>[]> value;
};

//实现部分
Matrix::Matrix(int rows, int columns)
	:rows{ rows }, columns{ columns }
{
	try {
		value = std::make_unique<std::unique_ptr<double[]>[]>(rows);
		for (int i{ 0 }; i < rows; i++)
			value[i] = std::make_unique<double[]>(columns);
	}
	catch(const std::bad_alloc& e){
		std::cerr << "内存分配失败" << e.what() << std::endl;
		exit(1);
	}
}
Matrix::Matrix(const Matrix& matrix)
	:Matrix{matrix.rows,matrix.columns}
{
	for (int i{ 0 }; i < matrix.rows; i++)
		for (int j{ 0 }; j < matrix.columns; j++)
			value[i][j] = matrix.value[i][j];
}
Matrix& Matrix::operator=(const Matrix& rhs)
{
	if (this == &rhs)
		return *this;

	std::unique_ptr<std::unique_ptr<double[]>[]> tmp;
	try {

		tmp = std::make_unique<std::unique_ptr<double[]>[]>(rhs.rows);
		for (int i{ 0 }; i < rhs.rows; i++)
			tmp[i] = std::make_unique<double[]>(rhs.columns);
	}
	catch (const std::bad_alloc& e) {
		std::cerr << "内存分配失败，=运算符左值将保持原值" << e.what() << std::endl;
		return *this;
	}

	value = std::move(tmp);
	rows = rhs.rows;
	columns = rhs.columns;
	for (int i{ 0 }; i < rows; i++)
		for (int j{ 0 }; j < columns; j++)
			value[i][j] = rhs.value[i][j];

	return *this;
}
Matrix::Matrix(Matrix&& src) noexcept
{
	value = std::move(src.value);
	rows = std::exchange(src.rows, 0);
	columns = std::exchange(src.columns, 0);
}
Matrix& Matrix::operator=(Matrix&& rhs) noexcept
{
	if (this == &rhs)
		return *this;

	value = std::move(rhs.value);
	rows = std::exchange(rhs.rows, 0);
	columns = std::exchange(rhs.columns, 0);
}
Matrix Matrix::operator+(const Matrix& matrix)
{
	if (rows != matrix.rows || columns != matrix.columns)
		throw std::invalid_argument("矩阵大小不同，无法相加");

	Matrix tmp{ rows,columns };

	for (int i{ 0 }; i < rows; i++)
		for (int j{ 0 }; j < columns; j++)
			tmp.value[i][j] = value[i][j] + matrix.value[i][j];
	
	return tmp;
}
Matrix Matrix::operator-(const Matrix& matrix)
{
	if (rows != matrix.rows || columns != matrix.columns)
		throw std::invalid_argument("矩阵大小不同，无法相加");

	Matrix tmp{ rows,columns };

	for (int i{ 0 }; i < rows; i++)
		for (int j{ 0 }; j < columns; j++)
			tmp.value[i][j] = value[i][j] - matrix.value[i][j];

	return tmp;
}
void Matrix::InputMatrix()
{
	using namespace std;
	cout << "请输入矩阵的元素：\n";
	while (true)
	{
		try {
			for (int i{ 0 }; i < rows; i++)
				for (int j{ 0 }; j < columns; j++)
					cin >> value[i][j];
			if (cin.fail())
				throw invalid_argument("无效输入，您输入的 不是数字 或者 数字超出double类型表示范围\n");
			cout << endl;
			break;
		}
		catch (const invalid_argument& e) {
			cin.clear();
			cin.ignore(numeric_limits<streamsize>::max(), '\n');
			cerr << e.what() << "\n请重新输入：" << endl;
		}
	}
}
void Matrix::OutputMatrix()
{
	using namespace std;
	for (int i{ 0 }; i < rows; i++)
	{
		cout << "|";
		for (int j{ 0 }; j < columns; j++)
			cout << right << setw(11) << value[i][j] << " ";
		cout << "|\n";
	}
	cout << endl;
}