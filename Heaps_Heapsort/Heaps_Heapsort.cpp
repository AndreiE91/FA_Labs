#include <cstdlib>
#include <iostream>
#include <cmath>
#include <climits>
#include "Profiler.h"
using namespace std;

//CONCLUZII PROFILER
/*
* BEST CASE: Bottom-up ruleaza in complexitate O(n) pe un vector cu elemente egale, neavand de muncit deloc, doar verifica si vede acest lucru, apoi termina.
*            Top-down ruleaza si el in complexitate liniara O(n) pe un vector sortat descrescator, dar are panta mai mare decat bottom-up, deoarece el are de
*            verificat mai multe conditii in hIncreaseKey decat are bottom-up in maxHeapify luand in calcul si diferenta de input pentru best cases diferite
* 
* AVERAGE CASE: Bottom-up ruleaza in O(n) - algoritm eficient folosit atunci cand stim deja tot array-ul si vrem sa construim un heap.
*               Desi am fi inclinati sa credem ca si acest algoritm ar rula in O(nlgn), in realitate aceasta limita nu este stransa asimptotic si se poate demonstra
*               ca de fapt limita asimptotic stransa este O(n) - acest lucru se datoreaza faptului ca maxHeapify ruleaza in O(h), nu O(n), unde h este inaltimea in arbore a respectivului nod,
*               iar n/2 - 1 noduri nu vor fi frunze, ceea ce inseamna ca se economiseste timp pe masura ce se apropie de varf.
*               Top-down ruleaza in O(nlgn) - algoritm versatil ce poate crea un heap dinamic, necunoscand numarul de elemente ce vor fi adaugate de la inceput,
*               evident cu un trade intre dinamicitate si timp de rulare, aici avem lgn pentru fiecare insertie de element si, inserand n elemente, obtinem n*lgn complexitate
*               !!!Heapurile construite de Bottom-up si Top-down pot fi diferite, acesti 2 algoritmi sunt implementati diferit si permit asemenea diferente sa aiba loc.
* 
* WORST CASE:   Aici vedem clar cum bottom-up este eficient chiar si in cazuri nefavorabile, ruland tot in timp liniar O(n), in timp ce top-down incepe sa ne arate adevarata
*               natura a complexitatii sale linearitmice O(nlgn). Desi pare cam mult 2500 operatii pentru n=100, acest algoritm este departe de a fi O(n^2), deoarece cu cat creste
*               nr de pasi, cu atat este mai evidenta diferenta: de exemplu pentru n=10000, ne-am astepta la un nr de pasi egal cu 10^8, dar de fapt avem doar 524000, numar egal cu 1/200 din 10^8,
*               in timp ce la inceput pt numere mai mici aceasta diefernta este mai mica, 1/4 in cazul n=100 cu 2500 pasi.
*/

Profiler p("heapSortTest");

enum caseid { best, average, worst };
enum algid { botUp, heapS, topDown };

#define MAX_SIZE 10000
#define STEP_SIZE 100
#define NR_TESTS 5

int attrBotUp, attrTopDown, compBotUp, compTopDown;

//Prototypes
int hParent(int i);
int hLeftChild(int );
int hRightChild(int i);
int hHeight(int nrElements);
void maxHeapify(int arr[], int n, int i, int hSize);
void buildMaxHeap(int arr[], int n, int hSize);
void heapSort(int arr[], int n, int hSize);
int hMaximum(int arr[], int n);
int hExtractMax(int arr[], int n, int hSize);
void hIncreaseKey(int arr[], int n, int i, int key);
void hMaxInsert(int arr[], int n, int key, int hSize);
void insertionBuildMaxHeap(int arr[], int n, int hSize);
void copyArray(int in[], int out[], int n);
void printArr(int arr[], int n);
void swapVal(int &a, int &b);
void demo(int algid);
void perforn(int caseid);
void resetIndexes();
void fillEqualElementArray(int arr[], int n, int value);

//Declarations

void fillEqualElementArray(int arr[], int n, int value)
{
    for (int i = 0; i < n; ++i)
    {
        arr[i] = value;
    }
}


void resetIndexes()
{
    attrBotUp = 0;
    attrTopDown = 0;
    compBotUp = 0;
    compTopDown = 0;
}

int hParent(int i)
{
    return (i / 2);
}

int hLeftChild(int i)
{
    return (2 * i);
}

int hRightChild(int i)
{
    return(2 * i + 1);
}

int hHeight(int nrElements) 
{
    return(floor(log2(nrElements)));
}

int hMaximum(int arr[], int n)
{
    return arr[0];
}

int hExtractMax(int arr[], int n, int hSize) //O(lgn), only constant work besides maxHeapify which is already O(lgn)
{
    if (hSize < 1)
    {
        cout << "Error: Heap underflow";
        return INT_MIN;
    }
    int max = arr[0];
    arr[0] = arr[hSize - 1];
    --hSize;
    maxHeapify(arr, n, 0, hSize);
    return max;
}

void hIncreaseKey(int arr[], int n, int i, int key) //O(lgn)
{
    ++compTopDown;
    if (key < arr[i])
    {
        cout << "Error: new key smaller than current key";
        return;
    }
    ++attrTopDown;
    arr[i] = key;
    while (i > 1)
    {
        ++compTopDown;
        if (arr[hParent(i)] < arr[i])
        {
            attrTopDown += 3;
            swapVal(arr[i], arr[hParent(i)]);
            i = hParent(i);
        }
        else break;
    }
}

void hMaxInsert(int arr[], int n, int key, int hSize)
{
    ++hSize; //this one here will not get passed to next for loop iteration, it is just for the hIncreaseKey
    ++attrTopDown;
    arr[hSize] = INT_MIN;
    hIncreaseKey(arr,n,hSize,key);
}

void insertionBuildMaxHeap(int arr[], int n, int hSize) //O(nlgn)
{
    hSize = 1;
    for (int i = 2; i < n; ++i) 
    {
        hMaxInsert(arr, n, arr[i], hSize);
        ++hSize; //this is the carry over for the ++hSize from the hMaxInsert algo. Works properly, just as if hSize was a pointer. Will only bug if key < arr[i], which is impossible as it is set to INT_MIN
    }
}


void printArr(int arr[], int n)
{
    for (int i = 0; i < n; ++i)
    {
        cout << arr[i] << " ";
    }
    cout << "\n";
}

void copyArray(int in[], int out[], int n)
{
    for (int i = 0; i < n; ++i)
    {
        out[i] = in[i];
    }
}

void swapVal(int &a, int &b)
{
    int temp = a;
    a = b;
    b = temp;
}

void maxHeapify(int arr[], int n, int i, int hSize) //Runs in O(h) time, where h is the height of node at index i | Bottom-up approach
{//Basically O(lgn) time, but actually O(lgh), which is why it has an asymptotically tight upper bound at O(n) for building a heap(a good part of nodes are close to the top so their height is lower which results in faster than lgn time(lgh))
        int left = hLeftChild(i);
        int right = hRightChild(i);
        int largest = i;
        if (left <= hSize) //if left child not a leaf node
        {
            ++compBotUp;
            if (arr[left] > arr[i]) //if child bigger than parent, prepare to swap
            {
                largest = left;
            }
        }
        else
        {
            largest = i; //leave parent where it is
        }

        if (right <= hSize) //if right child not a leaf node
        {
            ++compBotUp;
            if (arr[right] > arr[largest]) //if child bigger than parent, prepare to swap
            {
                largest = right;
            }
        }

        if (largest != i) //if we have a reason to swap and elements are not equal
        {
            attrBotUp += 3;
            swapVal(arr[i], arr[largest]);
            maxHeapify(arr, n, largest, hSize);
        }
        /* So, for the worst case, one would need to create an imput where we always enter as many times as possible into the if conditions(elements come in ascending order and elements are distinct,
        *  so that parents will always be smaller than children) and that would also imply the most amount of swaps performed
        */
} 

void buildMaxHeap(int arr[], int n, int hSize) 
{
    hSize = n;
    for (int i = floor(n / 2); i >= 0; --i)//we only have floor(n/2) nodes, the rest are leaves, and as we climb up towards the root, the index in the array decreases
    {
        maxHeapify(arr, n, i, hSize);
    }
}

void heapSort(int arr[], int n, int hSize)
{
    buildMaxHeap(arr, n, hSize);
    for (int i = n - 1; i > 0; --i)
    {
        swapVal(arr[0], arr[i]);
        --hSize;
        maxHeapify(arr, n, 0, hSize);
    }
}


void perform(int caseid) //0 best  1 average  2 worst
{
    int v[MAX_SIZE];
    int v1[MAX_SIZE];

    for (int n = STEP_SIZE; n <= MAX_SIZE; n += STEP_SIZE)
    {
        for (int i = 0; i < NR_TESTS; ++i)
        {
            switch (caseid)
            {
                case 0:
                {
                    FillRandomArray(v1, n, 10, 50000, false, 2); //case 0 - BEST - decreasingly sorted array for topDown(niciodata nu este necesar sa restabilim proprietatea de heap pentru ca incepem de la varf spre baza si vectorul este sortat descrescator)
                                                                //              - all elements equal for bottomUp
                    fillEqualElementArray(v, n, 59); //can be whatever element we want, all that matters is that they are all the same(nu necesita nniciun swap deoarece largest mereu egal cu i in maxHeapify)
                    break;
                }
                case 1:
                {
                    FillRandomArray(v, n); //case 1 AVERAGE will give random inputs for both algorithms
                    copyArray(v, v1, n);
                    break;
                }
                case 2:
                {
                    FillRandomArray(v1, n, 10, 50000, true, 1); //case 2 - WORST - ascendingly and unique elements sorted array (tot trebuie reactualizata proprietatea de heap pt fiecare insertie)
                    copyArray(v1, v, n);
                    break;
                }
                default:
                {
                    cout << "Invalid case ID\n";
                    break;
                }
            }
            Operation opComp1 = p.createOperation("botUp-comp", n);
            Operation opAttr1 = p.createOperation("botUp-attr", n);
            Operation opComp2 = p.createOperation("topDown-comp", n);
            Operation opAttr2 = p.createOperation("topDown-attr", n);
            
            resetIndexes(); //Since I am working with global variables, I have to be extra careful that the indexes are 0 before starting to count steps
            buildMaxHeap(v, n, n - 1);
            insertionBuildMaxHeap(v1, n, 1);
            opComp1.count(compBotUp);
            opAttr1.count(attrBotUp);
            opComp2.count(compTopDown);
            opAttr2.count(attrTopDown);
            resetIndexes();
        }
    }

    p.divideValues("botUp-attr", NR_TESTS);
    p.divideValues("botUp-comp", NR_TESTS);
    p.divideValues("topDown-attr", NR_TESTS);
    p.divideValues("topDown-comp", NR_TESTS);

    p.addSeries("botUp", "botUp-attr", "botUp-comp");
    p.addSeries("topDown", "topDown-attr", "topDown-comp");

    switch (caseid)
    {
        case 0: //best
        {
            p.createGroup("total-best", "botUp", "topDown");
            p.createGroup("attr-best", "botUp-attr", "topDown-attr");
            p.createGroup("comp-best", "botUp-comp", "topDown-comp");
            break;
        }
        case 1: //average
        {
            p.createGroup("total-average", "botUp", "topDown");
            p.createGroup("attr-average", "botUp-attr", "topDown-attr");
            p.createGroup("comp-average", "botUp-comp", "topDown-comp");
            break;
        }
        case 2: //worst
        {
            p.createGroup("total-worst", "botUp", "topDown");
            p.createGroup("attr-worst", "botUp-attr", "topDown-attr");
            p.createGroup("comp-worst", "botUp-comp", "topDown-comp");
            break;
        }
    }
    p.showReport();
}

void demo(int algid)
{
    int v[] = { 0, 8, 2, 7, 5, 6, 1, 3, 2, 9, -1, -4, -4, 0, -51, 105 };
    int n = sizeof(v) / sizeof(int);
    int l = 0; int r = sizeof(v) / sizeof(int) - 1;
    cout << "Example hardcoded array prior to procedure:\n";
    printArr(v, n);
    switch (algid)
    {
        case 0: //bottomUp heap construction
        {
            buildMaxHeap(v, n, n - 1);
            break;
        }
        case 1: //heapSort algorithm
        {
            heapSort(v, n, n - 1);
            break;
        }
        case 2: //topDown heap construction
        {
            insertionBuildMaxHeap(v, n, 1);
            break;
        }
        default:
        {
            cout << "Invalid algorithm ID\n";
            break;
        }
    }
    
    cout << "Example hardcoded array after the procedure:\n";
    printArr(v, n);
}

int main() //code for better user interaction
{
    resetIndexes(); //make sure idexes are initialized, to avoid any undesired behaviour
    int decision = -1;
    do
    {
        cout << "Hello! Do you wish to demonstrate correctness or test running time?\n";
        cout << " ********************\n";
        cout << " * 0 - correctness  *\n * 1 - running time *\n";
        cout << " ********************\n";
        cin >> decision;
    } while (decision < 0 || decision > 1);
    if (decision == 0)
    {
        while (true)
        {
            int algchoice = -1;
            do
            {
                cout << "Which of the three would you like to see?\n";
                cout << " *************************************\n";
                cout << " * 0 - Bottom up heap construction   *\n * 1 - Heapsort algorithm            *\n * 2 - Top down heap construction    *\n";
                cout << " *************************************\n";
                cin >> algchoice;
            } while (algchoice < 0 || algchoice > 2);
            demo(algchoice);
            cout << "\nDo you want to try again?(y/n)\n";
            string answer;
            cin >> answer;
            if (answer == "N" || answer == "n") exit(0);
        }
        return 0;
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
