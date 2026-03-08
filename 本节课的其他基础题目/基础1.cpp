import std;
import Matrix;

int main()
{
	Matrix A1{ AllocMatrix() }, A2{ AllocMatrix() }, A3{ AllocMatrix() };

	std::cout << "ÇëÊäÈë¾ØÕóA1£¨4¡Á5£©£º\n";
	InitMatrix(A1);
	std::cout << "ÇëÊäÈë¾ØÕóA2£¨4¡Á5£©£º\n";
	InitMatrix(A2);

	AddMatrix(A1, A2, A3);
	std::cout << "Êä³öA1+A2£º\n";
	OutputMatrix(A3);

	SubMatrix(A1, A2, A3);
	std::cout << "Êä³öA1-A2£º\n";
	OutputMatrix(A3);

	DeleteMatrix(A1);
	DeleteMatrix(A2);
	DeleteMatrix(A3);

	return 0;
}

