using namespace std;
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <iomanip>

#define MAXIM 200

void mergeSort(int arr[], int l, int r);
void merge(int arr[], int l, int mid, int r);
void generateRandomArray(int arr[], int n, int low, int high);
void generateRandomSortedArray(int arr[], int n, int low, int high);
void printArray(int arr[], int n, int elements_till_newl, int spaces_per_element);

void generateRandomSortedArray(int arr[], int n, int low, int high)
{
    generateRandomArray(arr, n, low, high);
    mergeSort(arr,0,n-1);
}

void generateRandomArray(int arr[],int n,int low,int high){
    srand(rand());
    //cout << "\n\n\n";
    for (int i = 0; i < n; ++i)
    {
        arr[i] = (rand() % (high - low)) + low;
        //cout << numere[i]<< "  ";
    }
}

void mergeSort(int arr[], int l, int r){
    if (l >= r) return;
    int mid = (l + r) / 2;
    mergeSort(arr,l,mid);
    mergeSort(arr, mid + 1,r);
    merge(arr,l,mid,r);
}

void printArray(int arr[], int n, int elements_till_newl, int spaces_per_element, string output) {
    ofstream fout(output);
    for (int i = 0; i < n; ++i) {
        //cout << setw(4) << vec[i] << " ";
        if (elements_till_newl % 25 == 0) fout << "\n";
        fout << setw(spaces_per_element) << arr[i] << " ";
        elements_till_newl++;
    }
}


void merge(int arr[],int l, int mid, int r) {
    int subArrLeft = mid - l + 1;
    int subArrRight = r - mid;
    //temp arrays
    int* leftArr = new int[subArrLeft];
    int* rightArr = new int[subArrRight];
    //copy data from original into temp arrays
    for (int i = 0; i < subArrLeft; ++i)
    {
        leftArr[i] = arr[l + i];
    }
    for (int i = 0; i < subArrRight; ++i)
    {
        rightArr[i] = arr[mid + i + 1];
    }

    int indexSubArrLeft = 0;
    int indexSubArrRight = 0;
    int indexArrMerged = l;

    while (indexSubArrLeft < subArrLeft && indexSubArrRight < subArrRight)
    {
        if (leftArr[indexSubArrLeft] < rightArr[indexSubArrRight])
        {
            arr[indexArrMerged] = leftArr[indexSubArrLeft];
            indexSubArrLeft++;
        }
        else
        {
            arr[indexArrMerged] = rightArr[indexSubArrRight];
            indexSubArrRight++;
        }
        indexArrMerged++;
    }
    //copy remaining elements if any
    while (indexSubArrLeft < subArrLeft)
    {
        arr[indexArrMerged] = leftArr[indexSubArrLeft];
        indexSubArrLeft++;
        indexArrMerged++;
    }
    while (indexSubArrRight < subArrRight)
    {
        arr[indexArrMerged] = rightArr[indexSubArrRight];
        indexSubArrRight++;
        indexArrMerged++;
    }
    //don't leave garbage in memory
    delete[] leftArr;
    delete[] rightArr;

}

int main(){
    srand(time(0));
    int n,low,high,spacer = 0;
    do{
        cout << "How many random numbers?(200 max)\n";
        cin >> n;
    } while (n > 200 || n < 1);
    cout << "low=";
    cin >> low;
    cout << "high=";
    cin >> high;

    int vec1[MAXIM];
    int vec2[MAXIM];

    generateRandomSortedArray(vec1, MAXIM, low, high);
    generateRandomSortedArray(vec2, MAXIM, low, high);

    printArray(vec1,MAXIM,25,4,"output.txt");
    printArray(vec2, MAXIM, 25, 4, "output.txt");

}
