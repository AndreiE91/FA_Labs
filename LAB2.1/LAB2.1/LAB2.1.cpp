#include <iostream>
#include "Profiler.h"
using namespace std;

//CONCLUZII PROFILER
/* 
* In best case, selection sort este singurul care munceste degeaba pentru un array deja sortat. Gratie conditiei de verificare implementata in bubblesort, si bubblesort reuseste sa
* performeze in complexitate de timp liniara impreuna cu insertion sort. Totusi, un singur element ar fi nevoie sa nu fie sortat si bubblesort ar incepe si el sa munceasca mult pentru
* un array aproape sortat, chiar mai mult decat selection sort. Insertion sort este singurul suficient de inteligent incat sa se poata adapta pentru bucati sortate din array.
* 
* In average case, insertion sort este cel mai eficient dintre cele 3, deoarece el este singurul care poate sa profite de vectori partial sortati (lucru inevitabil in orice vector
* random generat) si in acest fel scurteaza drumul intr-un mod adaptiv unde se poate scurta. Selection sort este putin mai eficient decat bubblesort, dar asta nu pentru ca selection sort
* este un algoritm rapid, ci pentru ca bubble sort este unul extrem de ineficient. Un trade intre simplitatea de a fi tinut minte si scris rapid de catre programator si eficienta.
* 
* In worst case, bubble sort pastreaza rutina de a munci mult cu care deja suntem obisnuiti, in timp ce selection si insertion sort se chinuie sa fie mai rapide, dar niciunul dintre
* ele nu reuseste sa scurteze timpul de executie. Nu avem bucati partial sortate intr-un vector sortat descrescator, deci insertion sort nu poate scurta nimic.
* 
* Overall, insertion sort este castigator in toate situatiile dintre acesti 3 algoritmi, pentru ca am vazut ca niciudata nu ajunge sa performeze mai prost decat bubble sau selection sort.
* In best case e foarte rapid, in average case e cel mai rapid dar nu foarte, iar in worst case e la egalitate cu selection sort. 
*/

Profiler p("sortTest");

enum caseid {best,average,worst};
enum algid {bubble, insertion, selection};

#define MAX_SIZE 10000
#define STEP_SIZE 100
#define NR_TESTS 5

void bubbleSort(int arr[], int n);
void insertionSort(int arr[], int n);
void selectionSort(int arr[], int n);
void copyArray(int v1[], int v2[], int n);
void demo(int algid);

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

void perform(int caseid) //0 best  1 average  2 worst
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
                        FillRandomArray(v, n, 10, 50000, false, 1); //case 0 - BEST will give already sorted array for all algorithms
                        break;
                    }
                    case 1:
                    {
                        FillRandomArray(v, n); //case 1 AVERAGE will give random inputs for all algorithms
                        break;
                    }
                    case 2:
                    {
                        FillRandomArray(v, n, 10, 50000, false, 2); //case 2 - WORST will give a reversely sorted array for all algorithms
                        break;
                    }
                    default:
                    {
                        cout << "Invalid case ID\n";
                        break;
                    }
                }
                copyArray(v, v1, n); //array is copied to ensure fairness for the test subjects
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
        
        switch (caseid)
        {
            case 0: //best
            {
                p.createGroup("total-best", "bbl", "ins", "sel");
                p.createGroup("attr-best", "bbl-attr", "ins-attr", "sel-attr");
                p.createGroup("comp-best", "bbl-comp", "ins-comp", "sel-comp");
                break;
            }
            case 1: //average
            {
                p.createGroup("total-average", "bbl", "ins", "sel");
                p.createGroup("attr-average", "bbl-attr", "ins-attr", "sel-attr");
                p.createGroup("comp-average", "bbl-comp", "ins-comp", "sel-comp");
                break;
            }
            case 2: //worst
            {
                p.createGroup("total-worst", "bbl", "ins", "sel");
                p.createGroup("attr-worst", "bbl-attr", "ins-attr", "sel-attr");
                p.createGroup("comp-worst", "bbl-comp", "ins-comp", "sel-comp");
                break;
            }
        }
        p.showReport();
}

void demo(int algid)
{
    int v[] = { 0, 8, 2, 7, 5, 6, 1, 3, 2, 9, -1, -4, -4, 0, -51, 105};
    int n = sizeof(v) / sizeof(int);
    int l = 0; int r = sizeof(v) / sizeof(int) - 1;
    cout << "Example hardcoded array prior to sorting:\n";
    printArr(v, n);
    switch (algid)
    {
        case 0:
        {
            bubbleSort(v, n);
            break;
        }
        case 1:
        {
            insertionSort(v, n);
            break;
        }
        case 2:
        {
            selectionSort(v, n);
            break;
        }
        default:
        {
            cout << "Invalid algorithm ID\n";
            break;
        }
    }
    cout << "Example hardcoded array after the sorting:\n";
    printArr(v, n);
}

void selectionSort(int arr[], int n)
{
    Operation opComp = p.createOperation("sel-comp", n);
    Operation opAttr = p.createOperation("sel-attr", n);

    for (int i = 0; i < n - 1; i++)
    {
        int max_i = i; opAttr.count();
        for (int j = i + 1; j < n - 1; j++)
        {
            opComp.count();
            if (arr[j] < arr[max_i])
                max_i = j; opAttr.count();
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
    for (int i = 1; i < n-1; ++i)
    {
        int aux = arr[i]; opAttr.count();
        int j = i - 1;
        while (arr[j] > aux && j >= 0)
        {
            opComp.count(); //this will count all comparisons except for the first one
            arr[j + 1] = arr[j]; opAttr.count();
            j = j - 1;
        }
        opComp.count(); //count the first comparison separately, in case condition evaluates to false from first try

        arr[j + 1] = aux; opAttr.count();
    }
}

void bubbleSort(int arr[], int n)
{
    Operation opComp = p.createOperation("bbl-comp", n);
    Operation opAttr = p.createOperation("bbl-attr", n);
    for (int i = 0; i < n - 1; ++i)
    {
        bool swapped = false;
        for (int j = n - 1; j > i; --j)
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

int main() //code for better user interaction
{
    int decision = -1;
    do
    {
        cout << "Hello! Do you wish to demonstrate correctness or test running time?\n";
        cout << " ********************\n";
        cout << " * 0 - correctness  *\n * 1 - running ime  *\n";
        cout << " ********************\n";
        cin >> decision;
    } while (decision < 0 || decision > 1);
    if (decision == 0)
    {
        bool wantsToRepeat = true;
        while (wantsToRepeat)
        {
            int algchoice = -1;
            do
            {
                cout << "Which of the three would you like to see?\n";
                cout << " ************************\n";
                cout << " * 0 - bubbleSort()     *\n * 1 - insertionSort()  *\n * 2 - selectionSort()  *\n";
                cout << " ************************\n";
                cin >> algchoice;
            } while (algchoice < 0 || algchoice > 2);
            demo(algchoice);
            cout << "\nDo you want to try again?(y/n)\n";
            char answer;
            cin >> answer;
            if (answer == 'N' || answer == 'n') wantsToRepeat = false;
        }
    }
    else
    {
        int caseiduser = -1;
        do
        {
            cout << "Which case shall we test?\n";
            cout << " ***************\n";
            cout << " * 0 - BEST    *\n * 1 - AVERAGE *\n * 2 - WORST   *\n";
            cout << " ***************\n";

            cin >> caseiduser;
        } while (caseiduser < 0 || caseiduser > 2);
        cout << "Please wait, this might take a while...";
        perform(caseiduser);
    }
}
