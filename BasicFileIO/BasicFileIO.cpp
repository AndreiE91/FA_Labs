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
