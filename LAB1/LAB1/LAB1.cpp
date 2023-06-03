// LAB1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <limits>

#define MAX_SIZE 100

int main()
{
    int numere [MAX_SIZE];
    int n = 0;
    FILE* fout = fopen("output.txt", "w");
    if (fout == NULL)
    {
        std::cout << "File open error";
        exit(-1);
    }
    //fprintf(fout, "Test initiated successfully! SALUT");
    std::cout << "How many integers to be read?\n";
    std::cin >> n;
    for (int i = 0; i < n; i++)
    {
        std::cout << "Enter integer:";
        std::cin >> numere[i];
    }
    for (int i = 0; i < n; i++)
    {
        fprintf(fout, "%d ", numere[i]);
    }
    fclose(fout);

    fout = fopen("output.txt", "r");
    int temp;
    for (int i = 0; i < n; i++)
    {
        fscanf(fout, "%d ", &temp);
        std::cout << temp <<" ";
    }
    fclose(fout);
}


// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
