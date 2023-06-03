using namespace std;
#include <iostream>
#include "Profiler.h"

//CONCLUZII PROFILER
/* În average case: Bubble sort este cel mai încet, în timp ce selection sort și insertion sunt la egalitate
* În worst case: Selection sort este cel mai rapid, insertion este la mijloc și bubble este cel mai încet
* În best case: Bubble sort este extrem de rapid, executând exact array size ca nr de pași. Acest lucru se datorează acelui boolean și condiție ce verifică dacă nu cumva este deja totul
* sortat. Insertion sort este și el foarte rapid, dar la fel ca bubble sort, iar selection sort se comportă polinomial deoarece trebuie să compare indiferent de situație.
*/

Profiler p("sortTest");

#define MAX_SIZE 5000
#define STEP_SIZE 100
#define NR_TESTS 5

void bubbleSort(int arr[], int n);
void insertionSort(int arr[], int n);
void selectionSort(int arr[], int n);
void copyArray(int v1[], int v2[], int n);

void printArr(int arr[], int n)
{
    for (int i = 0; i < n; ++i)
    {
        cout << arr[i] << " ";
    }
    cout << "\n";
}

void copyArray(int in[], int copy[], int n)
{
    for (int i = 0; i < n; ++i)
    {
        copy[i] = in[i];
    }
}

void perform(int caseid) //0 average  1 worst  2 best
{
    int v[MAX_SIZE];
    int v1[MAX_SIZE];
    int v2[MAX_SIZE];

        for (int n = STEP_SIZE; n <= MAX_SIZE; n += STEP_SIZE)
        {
            for (int i = 0; i < NR_TESTS; ++i)
            {
                switch (caseid)
                {
                    case 0:
                        {
                        FillRandomArray(v, n);
                        break;
                        }
                case 1:
                    {
                        FillRandomArray(v, n, 10, 50000, false, 2);
                        break;
                    }
                case 2:
                {
                    FillRandomArray(v, n, 10, 50000, false, 1);
                    break;
                }
                }
                copyArray(v, v1, n);
                copyArray(v, v2, n);

                bubbleSort(v, n);
                insertionSort(v1, n);
                selectionSort(v2, n);
            }
        }
        p.divideValues("bbl-attr", NR_TESTS);
        p.divideValues("bbl-comp", NR_TESTS);
        p.divideValues("ins-attr", NR_TESTS);
        p.divideValues("ins-comp", NR_TESTS);
        p.divideValues("sel-attr", NR_TESTS);
        p.divideValues("sel-comp", NR_TESTS);


        p.addSeries("bbl", "bbl-attr", "bbl-comp");
        p.addSeries("ins", "ins-attr", "ins-comp");
        p.addSeries("sel", "sel-attr", "sel-comp");

        p.createGroup("attr-average", "bbl-attr", "ins-attr", "sel-attr");
        p.createGroup("comp-average", "bbl-comp", "ins-comp", "sel-comp");

        p.createGroup("total-average", "bbl", "ins", "sel");

        p.showReport();
}

void test()
{
    int v[] = { 0, 8, 2, 7, 5, 6, 1, 3, 2, 9, -1, -4, -4, 0 };
    int n = sizeof(v) / sizeof(int);
    int l = 0; int r = sizeof(v) / sizeof(int) - 1;
    printArr(v, n);
    //bubbleSort(v, n);
    //selectionSort(v, n);
    insertionSort(v,n);
    printArr(v, n);
    //printArr(v);

}

void selectionSort(int arr[], int n)
{
    Operation opComp = p.createOperation("sel-comp", n);
    Operation opAttr = p.createOperation("sel-attr", n);

    for (int i = 0; i < n - 1; i++)
    {
        int max_i = i; //opAttr.count();
        for (int j = i + 1; j < n; j++)
        {
            opComp.count();
            if (arr[j] < arr[max_i])
                max_i = j; //opAttr.count();
        }
        opAttr.count(3);
        int aux = arr[i];
        arr[i] = arr[max_i];
        arr[max_i] = aux;

    }
}

void insertionSort(int arr[], int n)
{
    Operation opComp = p.createOperation("ins-comp", n);
    Operation opAttr = p.createOperation("ins-attr", n);
    for (int i = 2; i < n; ++i)
    {
        int aux = arr[i]; opAttr.count();
        int j = i - 1;
        while (arr[j] > aux && j >= 0)
        {
            opComp.count();
            arr[j + 1] = arr[j]; opAttr.count();
            j = j - 1;
        }
        opComp.count();

        arr[j + 1] = aux; opAttr.count();
    }
}

void bubbleSort(int arr[], int n)
{
    Operation opComp = p.createOperation("bbl-comp", n);
    Operation opAttr = p.createOperation("bbl-attr", n);
    for (int i = 0; i < n; ++i)
    {
        bool swapped = false;
        for (int j = n-1; j > i; --j)
        {
            opComp.count();
            if (arr[j] < arr[j-1])
            {
                int aux = arr[j]; opAttr.count(3);
                arr[j] = arr[j-1];
                arr[j-1] = aux;
                swapped = true;
            }
        }
        opComp.count();
        if (!swapped) return;

    }
}

int main()
{
    //test();
   perform(2); //0 average  1 worst  2 best
}
