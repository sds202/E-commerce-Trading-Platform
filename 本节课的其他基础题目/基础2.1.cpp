import std;
import Circle_Module;

int main()
{
	double x, y, r;
	GetInput(x, y, r);
	Circle O1{ x,y,r };
	GetInput(x, y, r);
	Circle O2{ x,y,r };

	O1.IsIntersectant(O2);
	return 0;
}