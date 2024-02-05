#include <cstdlib>
#include <iostream>
#include <cmath>
#include <ctime>
#include <climits>
#include "Profiler.h"
using namespace std;

//CONCLUZII PROFILER
/*
* QuickSort se comporta aproape identic in average si best case
* RandomizedQuickSort reuseste marginal sa fie mai bun decat QuickSort in toate cazurile, mai ales in worst unde QuickSort este O(n^2)
* De aceea se si foloseste in practica randomized pivot selection, deoarece mai este o metoda care ar asigura un caz favoarbil, si anume median of medians, sau median of 5, 8, etc...
* dar dureaza mult timp sa gasim respectivele mediane si overall pierdem mai mult timp asa
* 
* HeapSort bate QuickSort in best case si in worst case, dar in average case QuickSort este mai rapid. Important de mentionat ca marea majoritate a cazurilor sunt
* foarte apropiate de best case pentru QuickSort, si overall QuickSort este mai eficient chiar si fara a fi implementat cu randomized selection fata de HeapSort.
* 
* Observ ca desi ambele variante de bubbleSort(iterativ si recursiv) au acelasi numar de pasi(de inteles deoarece codul parcurge aceeasi pasi in ordine diferita), dar 
* bubbleSort recursiv ruleaza putin mai rapid in timp real fata de cel iterativ. Cel mai probabil deoarece in cel recursiv am folosit doar o bucla for. 
* Trade intre memorie pe call stack si viteza.
* 
* QuickSelect se dovedeste a fi o metoda foarte simpla si eficienta pentru a rezolva problemele particulare ale algoritmului QuickSort. Gratie acestui fapt,
* este imposibil practic pentru un om sa gaseasca un worst case input pentru asa implementare a algoritmului, pentru ca asta ar insemna sa fie in stare sa ghiceasca fiecare
* plasare random a pivotului. Ar fi necesar sa se nimereasca pivotul pe prima sau pe ultima pozitie a vectorului de n ori, ceea ce ne da o sansa de (2/n)/n!. Un lucru am 
* invatat aici, ca factorialul creste asa de rapid incat crapa graficul cand dau showReport(), deci daca impartim un numar, oricat de mare ar fi el, la n!, va tinde la 0.
* Acestea ar fi snsele de a intra in worst case cu QuickSort care foloseste metoda QuickSelect. Teoretic nu schimbam decat distributia cazurilor, aruncand cazul nefavorabil
* undeva unde nimeni nu il poate prezice, in loc sa il lasam sa fie sortarea crescatoare/descrescatoare a vectorului. 
* 
*/

Profiler p("quickSortTest");

#define MAX_SIZE 10000
#define STEP_SIZE 100
#define NR_TESTS 5

int attrQuickSort, attrRandQuickSort, attrHeapSort, attrBblRec, attrBbl, compQuickSort, compRandQuickSort ,compHeapSort, compBblRec, compBbl;

//Prototypes
int hParent(int i);
int hLeftChild(int);
int hRightChild(int i);
int hHeight(int nrElements);
void maxHeapify(int arr[], int n, int i, int hSize);
void buildMaxHeap(int arr[], int n, int hSize);
void heapSort(int arr[], int n, int hSize);
void copyArray(int in[], int out[], int n);
void printArr(int arr[], int n);
void swapVal(int& a, int& b);
void demo(int algid);
void perform(int caseid);
void resetIndexes();
void fillEqualElementArray(int arr[], int n, int value);
void quickSort(int arr[], int left, int right);
int partition(int arr[], int left, int right, bool comesFromRandQuickSort);
int random(int range_low, int range_high);
void bubbleSort(int arr[], int n);
void bubbleSortRecursive(int arr[], int n);
int randomPartition(int arr[], int left, int right);
void randomQuickSort(int arr[], int left, int right);
void printArrayIndexes(int size);
void performRuntimeTest();

//Declarations

int random(int range_low, int range_high) {
    return (rand() % (range_high - range_low + 1)) + range_low;
}

void performRuntimeTest() {
    int v[MAX_SIZE];
    int v1[MAX_SIZE];
    cout << "Please wait, this might take a while...";
    for (int n = STEP_SIZE; n <= MAX_SIZE; n += STEP_SIZE)
    {
        for (int i = 0; i < NR_TESTS * 5; ++i)
        {
            FillRandomArray(v, n);
            copyArray(v, v1, n);

            p.startTimer("bubbleSort", n);
            bubbleSort(v, n);
            p.stopTimer("bubbleSort", n);

            p.startTimer("bubbleSortRecursive", n);
            bubbleSortRecursive(v1, n);
            p.stopTimer("bubbleSortRecursive", n);
        }
    }
    p.divideValues("bubbleSort", NR_TESTS * 5);
    p.divideValues("bubbleSortRecursive", NR_TESTS * 5);
    p.createGroup("Iterative VS Recursive", "bubbleSort", "bubbleSortRecursive");
    p.showReport();
}

void printArrayIndexes(int size) {
    for (int i = 0; i < size; ++i) {
        cout << i << " ";
    }
    cout << "\n";
}


void randomQuickSort(int arr[], int left, int right) {
    if (left < right) {
        ++attrRandQuickSort;
        int pivot = randomPartition(arr, left, right);
        randomQuickSort(arr, left, pivot - 1);
        randomQuickSort(arr, pivot + 1, right);

    }
}

int randomPartition(int arr[], int left, int right) {
    int i = random(left, right - 1);
    attrRandQuickSort += 3;
    swap(arr[right], arr[i]);
    return partition(arr, left, right, true);
}

void bubbleSort(int arr[], int n)
{
    
    for (int i = 0; i < n - 1; ++i)
    {
        bool swapped = false;
        for (int j = n - 1; j > i; --j)
        {
            ++compBbl;
            if (arr[j] < arr[j - 1])
            {
                int aux = arr[j]; attrBbl += 3;
                arr[j] = arr[j - 1];
                arr[j - 1] = aux;
                swapped = true;
            }
        }
        if (!swapped) return;
    }
}

void bubbleSortRecursive(int arr[], int n) {
    if (n < 2) return;
    bool sorted = true;
    for (int i = 0; i < n - 1; ++i) {
        ++compBblRec;
        if (arr[i] > arr[i + 1]) {
            attrBblRec += 3;
            swapVal(arr[i], arr[i + 1]);
            sorted = false;
        }
    }
    if (sorted) return;
    bubbleSortRecursive(arr, n - 1);
}

int partition(int arr[], int left, int right, bool comesFromRandQuickSort) {
    if (comesFromRandQuickSort)
        ++attrRandQuickSort;
    else
        ++attrQuickSort;
    int x = arr[right];
    int i = left - 1;
    for (int j = left; j <= right - 1; ++j) {
            if (comesFromRandQuickSort)
                ++compRandQuickSort;
            else
                ++compQuickSort;
        if (arr[j] <= x) {
            ++i;
                if (comesFromRandQuickSort)
                    attrRandQuickSort += 3;
                else
                    attrQuickSort += 3;
            swap(arr[i],arr[j]);
        }
    }
    attrQuickSort += 3;
    swap(arr[i + 1], arr[right]);
    return i + 1;
}

void quickSort(int arr[], int left, int right) {
    if (left < right) {
        int pivot = partition(arr, left, right, false);
        quickSort(arr, left, pivot - 1);
        quickSort(arr, pivot + 1, right);
    }
}

void fillEqualElementArray(int arr[], int n, int value)
{
    for (int i = 0; i < n; ++i)
    {
        arr[i] = value;
    }
}

void resetIndexes()
{
    attrBblRec = 0;
    attrHeapSort = 0;
    attrQuickSort = 0;
    attrRandQuickSort = 0;
    compBblRec = 0;
    compHeapSort = 0;
    compQuickSort = 0;
    compRandQuickSort = 0;
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

void swapVal(int& a, int& b)
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
        ++compHeapSort;
        if (arr[left] > arr[i]) //if child bigger than parent, prepare to swap
        {
            largest = left;
        }
    }
    else
    {
        largest = i; //leave parent where it is
    }
    ++compHeapSort;
    if (right <= hSize) //if right child not a leaf node
    {
        if (arr[right] > arr[largest]) //if child bigger than parent, prepare to swap
        {
            largest = right;
        }
    }

    if (largest != i) //if we have a reason to swap and elements are not equal
    {
        attrHeapSort += 3;
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
        attrHeapSort += 3;
        swapVal(arr[0], arr[i]);
        --hSize;
        maxHeapify(arr, n, 0, hSize);
    }
}


void perform(int caseid) //0 best  1 average  2 worst
{
    int v[MAX_SIZE];
    int v1[MAX_SIZE];
    int v2[MAX_SIZE];
    int v3[MAX_SIZE];
    int v4[MAX_SIZE];

    for (int n = STEP_SIZE; n <= MAX_SIZE; n += STEP_SIZE)
    {
        for (int i = 0; i < NR_TESTS; ++i)
        {
            switch (caseid)
            {
            case 0: //BEST CASE
            {
                FillRandomArray(v, n, 10, 50000, true, 0); //quickSort
                fillEqualElementArray(v1, n, random(10, 50000)); //heapSort
                FillRandomArray(v2, n, 10, 50000, false, 1); //bblSort
                FillRandomArray(v3, n, 10, 50000, false, 1); //bblSortRec
                copyArray(v, v4, n); //randomQuickSort
                break;
            }
            case 1: //AVERAGE CASE
            {
                FillRandomArray(v, n); //this is for quickSort
                copyArray(v, v1, n); //this is for heapSort
                copyArray(v, v2, n); //this is for bblSort
                copyArray(v, v3, n); //this is for bblSortRec
                copyArray(v, v4, n); //this is for randomQuickSort
                break;
            }
            case 2: //WORST CASE
            {
                FillRandomArray(v, n, 10, 50000, true, 1); //quickSort
                FillRandomArray(v1, n, 10, 50000, true, 1); //heapSort
                FillRandomArray(v2, n, 10, 50000, false, 2); //bblSort
                FillRandomArray(v3, n, 10, 50000, false, 2); //bblSortRec
                copyArray(v, v4, n); //randomQuickSort
                break;
            }
            default:
            {
                cout << "Invalid case ID\n";
                break;
            }
            }
            Operation opComp = p.createOperation("bbl-comp", n);
            Operation opAttr = p.createOperation("bbl-attr", n);
            Operation opComp1 = p.createOperation("quickSort-comp", n);
            Operation opAttr1 = p.createOperation("quickSort-attr", n);
            Operation opComp2 = p.createOperation("heapSort-comp", n);
            Operation opAttr2 = p.createOperation("heapSort-attr", n);
            Operation opComp3 = p.createOperation("bblRec-comp", n);
            Operation opAttr3 = p.createOperation("bblRec-attr", n);
            Operation opComp4 = p.createOperation("randQuickSort-comp", n);
            Operation opAttr4 = p.createOperation("randQuickSort-attr", n);

            resetIndexes(); //Since I am working with global variables, I have to be extra careful that the indexes are 0 before starting to count steps
            quickSort(v, 0, n-1);
            heapSort(v1, n, n-1);
            bubbleSort(v2, n);
            bubbleSortRecursive(v3, n);
            randomQuickSort(v4, 0, n-1);

            opComp.count(compBbl);
            opAttr.count(attrBbl);
            opComp1.count(compQuickSort);
            opAttr1.count(attrQuickSort);
            opComp2.count(compHeapSort);
            opAttr2.count(attrHeapSort);
            opComp3.count(compBblRec);
            opAttr3.count(attrBblRec);
            opComp4.count(compRandQuickSort);
            opAttr4.count(attrRandQuickSort);
        }
    }

    p.divideValues("quickSort-attr", NR_TESTS);
    p.divideValues("quickSort-comp", NR_TESTS);
    p.divideValues("heapSort-attr", NR_TESTS);
    p.divideValues("heapSort-comp", NR_TESTS);
    p.divideValues("bblRec-attr", NR_TESTS);
    p.divideValues("bblRec-comp", NR_TESTS);
    p.divideValues("bbl-attr", NR_TESTS);
    p.divideValues("bbl-comp", NR_TESTS);
    p.divideValues("randQuickSort-attr", NR_TESTS);
    p.divideValues("randQuickSort-comp", NR_TESTS);

    p.addSeries("quickSort", "quickSort-attr", "quickSort-comp");
    p.addSeries("heapSort", "heapSort-attr", "heapSort-comp");
    p.addSeries("bbl", "bbl-attr", "bbl-comp");
    p.addSeries("bblRec", "bblRec-attr", "bblRec-comp");
    p.addSeries("randQuickSort", "randQuickSort-attr", "randQuickSort-comp");



    switch (caseid)
    {
    case 0: //best
    {
        p.createGroup("total-best-nlg", "quickSort", "heapSort", "randQuickSort");
        p.createGroup("attr-best-nlg", "quickSort-attr", "heapSort-attr", "randQuickSort-attr");
        p.createGroup("comp-best-nlg", "quickSort-comp", "heapSort-comp", "randQuickSort-comp");
        p.createGroup("total-best-n2", "bbl", "bblRec");
        p.createGroup("attr-best-n2", "bbl-attr", "bblRec-attr");
        p.createGroup("comp-best-n2", "bbl-comp", "bblRec-comp");
        break;
    }
    case 1: //average
    {
        p.createGroup("total-average-nlg", "quickSort", "heapSort", "randQuickSort");
        p.createGroup("attr-average-nlg", "quickSort-attr", "heapSort-attr", "randQuickSort-attr");
        p.createGroup("comp-average-nlg", "quickSort-comp", "heapSort-comp", "randQuickSort-comp");
        p.createGroup("total-average-n2", "bbl", "bblRec");
        p.createGroup("attr-average-n2", "bbl-attr", "bblRec-attr");
        p.createGroup("comp-average-n2", "bbl-comp", "bblRec-comp");
        break;
    }
    case 2: //worst
    {
        p.createGroup("total-worst-nlg", "quickSort", "heapSort", "randQuickSort");
        p.createGroup("attr-worst-nlg", "quickSort-attr", "heapSort-attr", "randQuickSort-attr");
        p.createGroup("comp-worst-nlg", "quickSort-comp", "heapSort-comp", "randQuickSort-comp");
        p.createGroup("total-worst-n2", "bbl", "bblRec");
        p.createGroup("attr-worst-n2", "bbl-attr", "bblRec-attr");
        p.createGroup("comp-worst-n2", "bbl-comp", "bblRec-comp");
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
    case 0: //quickSort algorithm
    {
        quickSort(v, 0, n - 1);
        break;
    }
    case 1: //heapSort algorithm
    {
        heapSort(v, n, n - 1);
        break;
    }
    case 2: //quickSelect(randomPartition) algorithm
    {
        cout << "\nThe output(RandomPartition) for the given array is: " <<randomPartition(v, 0, n-1) << "\n";
        break;
    }
    case 3: //randomized Qsort
    {
        randomQuickSort(v, 0, n - 1);
        break;
    }
    case 4: //bubbleSort algorithm
    {
        bubbleSort(v, n);
        break;
    }
    case 5: //bubbleSort recursive algorithm
    {
        bubbleSortRecursive(v, n);
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
    srand(time(NULL)); //initialize a seed for the rand() function
    resetIndexes(); //make sure idexes are initialized, to avoid any undesired behaviour
    int decision = -1;
    do
    {
        cout << "Hello! Choose one option:?\n";
        cout << " ******************************\n";
        cout << " * 0 - correctness            *\n * 1 - running time           *\n * 2 - operation count report *\n";
        cout << " ******************************\n";
        cin >> decision;
    } while (decision < 0 || decision > 2);
    if (decision == 0)
    {
        while (true)
        {
            int algchoice = -1;
            do
            {
                cout << "Which of the six would you like to see?\n";
                cout << " ********************************************\n";
                cout << " * 0 - QuickSort algorithm                  *\n * 1 - Heapsort algorithm                   *\n * 2 - QuickSelect algorithm                *\n";
                cout << " * 3 - Random QuickSort(using QuickSelect)  * \n * 4 - BubbleSort iterative                 * \n * 5 - BubbleSort recursive                 *\n";
                cout << " ********************************************\n";
                cin >> algchoice;
            } while (algchoice < 0 || algchoice > 5);
            demo(algchoice);
            cout << "\nDo you want to try again?(y/n)\n";
            string answer;
            cin >> answer;
            if (answer == "N" || answer == "n") exit(0);
        }
        return 0;
    }
    else if(decision == 2)
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
    else {
        performRuntimeTest();
    }
}
