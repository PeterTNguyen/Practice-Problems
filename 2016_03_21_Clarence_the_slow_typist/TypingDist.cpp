#include <iostream>
#include <iomanip> 
#include <math.h>
//LUT for keypad pos	  0		  1	  	  2		  3		  4		  5		  6		  7		  8		  9		  .
int keypadLUT[11][2] = {{1, 0}, {0, 3}, {1, 3}, {2, 3}, {0, 2}, {1, 2}, {2, 2}, {0, 1}, {1, 1}, {2, 1}, {0, 0}};

using namespace std;
void getKeypadPos(const char c, int * x, int * y);

int main( int argc, char *argv[] )
{
	float distance = 0.0;
	int x0, y0, x1, y1;
	string ipAddr; 
	//input IP address
	cout << "Type in IP Address: " ;
	cin >> ipAddr;
	//get first keypad pos
	getKeypadPos(ipAddr[0], &x0, &y0);
	for ( string::iterator it=ipAddr.begin() + 1; it!=ipAddr.end(); ++it)
	{
		getKeypadPos(*it, &x1, &y1);
		distance += sqrt(pow(x1 - x0, 2.0) + pow(y1 - y0, 2.0));//calc distance
		x0 = x1; 
		y0 = y1;
	}
  	cout << "Typing Distance is "<< std::setprecision(4) << distance << "cm."<<endl;
}
//
void getKeypadPos(const char c, int * x, int * y)
{
	if(c == '.')
	{
		*x = keypadLUT[10][0];
		*y = keypadLUT[10][1];
	}
	else
	{
		*x = keypadLUT[c - '0'][0];
		*y = keypadLUT[c - '0'][1];
	}
}