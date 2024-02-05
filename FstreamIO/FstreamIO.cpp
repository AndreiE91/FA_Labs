using namespace std;
#include <iostream>
#include <fstream>
#include <string>

int main()
{
    ifstream fin("input.txt");
    ofstream fout("output.txt");

    string message = "If you see this in output.txt, reading from file didn't work.";
    getline(fin,message);
    fout << message;
}

