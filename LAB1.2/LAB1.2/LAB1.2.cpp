using namespace std;
#include <iostream>
#include <fstream>
#include <cmath>
#include <climits>
#include "Profiler.h"

Profiler p("BasicFunctionsTest");

#define MAX_SIZE 10000
#define STEP_SIZE 100

int fact(int n)
{
	if (n > 1) return n * fact(n - 1);
	else return 1;

} //this function grows so fast that computer throws a 0 instead of actual value. It works for n < 15, but it quickly starts to overflow and do all sorts of weird things after that.
//an easy fix would be to use LLONG_MAX to be able to see something else than 0
void writeFunctionValues(int maxVal, int step, string output)
{
	ofstream fout(output);

	for (int i = step; i <= maxVal; i += step)
	{
		fout << i << " " << 100 * log2(i) << " " << 10 * i << " " << 0.5 * i * i << " " << pow(2, i) << " " << /*fact(i)*/ LLONG_MAX << "\n";
	}
	for (int i = step; i <= maxVal; i += step)
	{
		Operation linear = p.createOperation("linear", i);
		linear.count(i);
		Operation logarithmic = p.createOperation("log", i);
		logarithmic.count(100*log2(i));
		Operation ten = p.createOperation("ten", i);
		ten.count(10 * i);
		Operation halfsq = p.createOperation("halfsq", i);
		halfsq.count(0.5 * i*i);
		Operation power = p.createOperation("power", i);
		power.count(pow(2,i));
		Operation factorial = p.createOperation("fac", i);
		factorial.count(/*fact(i)*/LLONG_MAX);
	}
}

int main()
{
	writeFunctionValues(MAX_SIZE, STEP_SIZE, "output.txt");

	p.createGroup("all","linear","log","ten","halfsq","power","fac");

	p.showReport();

}

