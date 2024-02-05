using namespace std;
#include <iostream>
#include "Profiler.h"

Profiler p("sortTest");

#define MAX_SIZE 1000
#define STEP_SIZE 50

void bubbleSort(int arr[], int n);
void insertionSort(int arr[], int n);
void selectionSort(int arr[], int n);
void mergeSort(int arr[], int l, int h);
void mergef(int arr[], int l, int mid, int h);
void mergeCall(int arr[], int n);

void printArr(int arr[], int n)
{
    for (int i = 0; i < n; ++i)
    {
        cout << arr[i] << " ";
    }
    cout << "\n";
}

void perform()
{
    int v[MAX_SIZE];
    for (int n = STEP_SIZE; n <= MAX_SIZE; ++n)
    {
        FillRandomArray(v, n);
        bubbleSort(v, n);
        FillRandomArray(v, n);
        insertionSort(v, n);
        FillRandomArray(v, n);
        selectionSort(v, n);
        FillRandomArray(v,n);
        mergeCall(v,n);
    }
    p.addSeries("bbl","bbl-attr","bbl-comp");
    p.addSeries("ins", "ins-attr", "ins-comp");
    p.addSeries("sel", "sel-attr", "sel-comp");
    p.addSeries("merge", "merge-attr","merge-comp");


    p.createGroup("attr","bbl-attr","ins-attr","sel-attr","merge-attr");
    p.createGroup("comp", "bbl-comp", "ins-comp","sel-comp","merge-comp");

    p.createGroup("total", "bbl", "ins","sel","merge");


    p.showReport();
}

void test()
{
    int v[] = {0, 8, 2, 7, 5, 6, 1, 3, 2, 9, -1, -4, -4, 0};
    int n = sizeof(v) / sizeof(int);
    int l = 0; int r = sizeof(v) / sizeof(int) - 1;
    printArr(v, n);
    //bubbleSort(v, n);
    mergeCall(v, n);
    //selectionSort(v, n);
    printArr(v, n);
    //mergeSort(v, l, r);
    //printArr(v);

}

void mergeCall(int arr[], int n)
{
    mergeSort(arr,0,n-1);
}

void selectionSort(int arr[], int n)
{
    Operation opComp = p.createOperation("sel-comp",n);
    Operation opAttr = p.createOperation("sel-attr", n);

    for (int i = 0; i < n - 1; i++)
    {
        int max_i = i; //opAttr.count();
        for (int j = i+1; j < n; j++)
        {
            opComp.count();
            if (arr[j] > arr[max_i])
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
    for (int i = 2; i < n; i++)
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
        for (int j = 0; j < n; ++j)
        {
            opComp.count();
            if (arr[i] < arr[j])
            {
                int aux = arr[i]; opAttr.count(3);
                arr[i] = arr[j];
                arr[j] = aux;
            }
        }
    }
}

void mergeSort(int arr[], int l, int h)
{
    Operation opComp = p.createOperation("merge-comp", h+1);
    Operation opAttr = p.createOperation("merge-attr", h+1);
    if (l >= h) opComp.count();
        return;
        int mid = (l + h) / 2; opAttr.count();
    mergeSort(arr, l, mid);
    mergeSort(arr, mid + 1, h);
    mergef(arr, l, mid, h);
}

void mergef(int arr[], int l, int mid, int h)
{
    Operation opComp = p.createOperation("merge-comp", h+1);
    Operation opAttr = p.createOperation("merge-attr", h+1);
    int subArrayOne = mid - l + 1; opAttr.count(2);
    int subArrayTwo = h - mid;

    // Create temp arrays
    int* leftArray = new int[subArrayOne],
        * rightArray = new int[subArrayTwo]; opAttr.count(2);

    // Copy data to temp arrays leftArray[] and rightArray[]
    for (int i = 0; i < subArrayOne; i++)
        leftArray[i] = arr[l + i]; opAttr.count();
    for (int j = 0; j < subArrayTwo; j++)
        rightArray[j] = arr[mid + 1 + j]; opAttr.count();

    int indexOfSubArrayOne
        = 0, // Initial index of first sub-array
        indexOfSubArrayTwo
        = 0; // Initial index of second sub-array
    int indexOfMergedArray
        = l; // Initial index of merged array
    opAttr.count(3);

    // Merge the temp arrays back into array[left..right]
    while (indexOfSubArrayOne < subArrayOne){
        opComp.count(2);
        if (indexOfSubArrayTwo >= subArrayTwo) break;
        opComp.count();
        if (leftArray[indexOfSubArrayOne]
            <= rightArray[indexOfSubArrayTwo]) {
            arr[indexOfMergedArray]
                = leftArray[indexOfSubArrayOne]; opAttr.count();
            indexOfSubArrayOne++;
        }
        else {
            arr[indexOfMergedArray]
                = rightArray[indexOfSubArrayTwo]; opAttr.count();
            indexOfSubArrayTwo++;
        }
        indexOfMergedArray++;
    }
    opComp.count();
    // Copy the remaining elements of
    // left[], if there are any
    while (indexOfSubArrayOne < subArrayOne) {
        opComp.count();
        arr[indexOfMergedArray]
            = leftArray[indexOfSubArrayOne]; opAttr.count();
        indexOfSubArrayOne++;
        indexOfMergedArray++;
    }
    opComp.count();
    // Copy the remaining elements of
    // right[], if there are any
    while (indexOfSubArrayTwo < subArrayTwo) {
        opComp.count();
        arr[indexOfMergedArray]
            = rightArray[indexOfSubArrayTwo]; opAttr.count();
        indexOfSubArrayTwo++;
        indexOfMergedArray++;
    }
    opComp.count();
    delete[] leftArray;
    delete[] rightArray;
}

int main()
{
    //test();
    perform();
}
