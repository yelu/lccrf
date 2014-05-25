#include<iostream>
#include<boost/multi_array.hpp>
using namespace std;

int main()
{
	cout << "Hello Wordl!" << endl;
	int a;
	int b;
	cin >> a >> b;
	boost::multi_array<double, 2> _alphaMatrix(boost::extents[a][b]);
	_alphaMatrix[1][1] = 5.555;
	double c = _alphaMatrix[1][1];
	cout << c << endl;
	return 0;
}