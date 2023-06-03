using namespace std;
#include <iostream>
#include <cstdlib>
#include "Profiler.h"

/* CONCLUSIONS / OBSERVATIONS / INTERPRETATIONS

    Using a heap structure greatly speeds up the merging process of k lists; fact which is especially noticeable for very large inputs
    The merging algorithm implemented below has a time complexity of O(nlgk), which can be observed both by doing the math on the first graph but
    also by looking at the second graph, where we see that, for a constant n size of 10000, as we increase k, the number of steps increases slower the
    bigger k gets.
    Although bottomUp heap construction is faster for a single construction O(n), making it a great choice for heapSort, when we need to constantly add
    and remove elements from a heap, topUp construction ends up saving uncomprehensibly more time in the long run, as it has a time complexity of only 
    O(lgn) compared to the same O(n) from bottomUp as we have to rebuild a new heap even if we only modify one element
    It makes sense to have O(nlgk) time complexity, as we have k lists, so the heap we need to build will be of size k, and the total number of elements will
    be n, so n times logk to add an element, gives us nlgk. But we also have to remove, and some other miscellaneous operations done in between, but those are regarded
    as constants and therefore can be omitted by the big O notation. Growth rate is still cvasilnear, with log k which is always smaller than n so it is even better.
    We save 300% more time on medium sized inputs compared to small ones, but we save hundreds of times more time for very large inputs, such as  n = 10^6, k = ....
*/


Profiler p("MergeKLists");

#define K1 5
#define K2 10
#define K3 100
#define N_MAX 10000
#define K_MAX 500
#define STEP_SIZE 100
#define K_INCREMENT 10

int comp, attr;

class CustomData {
public:
    int data;
    int belongingListIndex;
};

class Node {
public:
    CustomData customData;
    Node* next, *prev;
};

class List {
public:
    Node* firstAdded, * lastAdded;
    int size, index;
};

List **lists;

//Prototypes
int hParent(int i);
int hLeftChild(int);
int hRightChild(int i);
int hHeight(int nrElements);
void minHeapify(CustomData arr[], int n, int i, int hSize);
void buildMinHeap(CustomData arr[], int n, int hSize);
void copyArray(int in[], int out[], int n);
void swapVal(CustomData& a, CustomData& b);
int randomNumber(int low, int high);
void addNode(int element, List* list);
void arrayToList(int arr[], int n, List* list);
void printArr(int arr[], int n);
void printList(List* list, boolean reversed);
void generateKRandomLists(int n, int k);
void listToArray(CustomData arr[], List *list);
void listToMinHeap(CustomData arr[], List* list);
void mergeKLists(int n, int k, List *list);
void insertionBuildMinHeap(CustomData arr[], int n, int hSize);
void hMinInsert(CustomData arr[], int n, CustomData key, int hSize);
void hIncreaseKey(CustomData arr[], int n, int i, CustomData key);
CustomData hExtractMin(CustomData arr[], int n, int hSize);
void printCustomDataArr(CustomData arr[], int n);
void printListFromNode(Node* node, boolean reversed);
void merge2Lists(int n, List* outputList);
void resetIndexes();
void performOperationsTest();
void deleteList(List *list);
void deleteAllLists(int k);
void performSecondOperationsTest();



void resetIndexes() {
    attr = 0;
    comp = 0;
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

CustomData hExtractMin(CustomData arr[], int n, int hSize) //O(lgn), only constant work besides minHeapify which is already O(lgn)
{
    if (hSize < 1)
    {
        cout << "Error: Heap underflow";
        CustomData intMAX;
        intMAX.data = INT_MAX;
        intMAX.belongingListIndex = -1;
        return intMAX;
    }
    CustomData min = arr[1];
    arr[1] = arr[hSize - 1];
    --hSize;
    minHeapify(arr, n, 1, hSize);
    return min;
}

void hIncreaseKey(CustomData arr[], int n, int i, CustomData key) //O(lgn)
{
    ++comp;
    if (key.data > arr[i].data)
    {
        cout << "Error: new key larger than current key";
        return;
    }
    ++attr;
    arr[i] = key;
    while (i > 1)
    {
        ++comp;
        if (arr[hParent(i)].data > arr[i].data)
        {
            attr += 3;
            swapVal(arr[i], arr[hParent(i)]);
            ++attr;
            i = hParent(i);
        }
        else break;
    }
}

void hMinInsert(CustomData arr[], int n, CustomData key, int hSize)
{
    ++hSize; //this one here will not get passed to next for loop iteration, it is just for the hIncreaseKey
    arr[hSize].data = INT_MAX;
    hIncreaseKey(arr, n, hSize, key);
}

void insertionBuildMinHeap(CustomData arr[], int n, int hSize) //O(nlgn)
{
    hSize = 1;
    for (int i = 2; i < n; ++i)
    {
        hMinInsert(arr, n, arr[i], hSize);
        ++hSize; //this is the carry over for the ++hSize from the hMinInsert algo. Works properly, just as if hSize was a pointer. Will only bug if key > arr[i], which is impossible as it is set to INT_MAX
    }
}

void swapVal(CustomData& a, CustomData& b)
{
    CustomData temp = a;
    a = b;
    b = temp;
}

void copyArray(int in[], int out[], int n)
{
    for (int i = 0; i < n; ++i)
    {
        out[i] = in[i];
    }
}

void minHeapify(CustomData arr[], int n, int i, int hSize) //Runs in O(h) time, where h is the height of node at index i | Bottom-up approach
{//Basically O(lgn) time, but actually O(lgh), which is why it has an asymptotically tight upper bound at O(n) for building a heap(a good part of nodes are close to the top so their height is lower which results in faster than lgn time(lgh))
    int left = hLeftChild(i);
    int right = hRightChild(i);
    int smallest = i;
    attr += 2;
    if (left <= hSize) //if left child not a leaf node
    {
        ++comp;
        if (arr[left].data < arr[i].data) //if child bigger than parent, prepare to swap
        {
            ++attr;
            smallest = left;
        }
    }
    else
    {
        smallest = i; //leave parent where it is
    }
    if (right <= hSize) //if right child not a leaf node
    {
        ++comp;
        if (arr[right].data < arr[smallest].data) //if child bigger than parent, prepare to swap
        {
            ++attr;
            smallest = right;
        }
    }
    if (smallest > n - 1) {
        --smallest;
    }
    if (smallest != i) //if we have a reason to swap and elements are not equal
    {
        attr += 3;
        swapVal(arr[i], arr[smallest]);
        minHeapify(arr, n, smallest, hSize);
    }
    /* So, for the worst case, one would need to create an imput where we always enter as many times as possible into the if conditions(elements come in ascending order and elements are distinct,
    *  so that parents will always be smaller than children) and that would also imply the most amount of swaps performed
    */
}

void buildMinHeap(CustomData arr[], int n, int hSize)
{
    hSize = n;
    for (int i = floor(n / 2); i >= 0; --i)//we only have floor(n/2) nodes, the rest are leaves, and as we climb up towards the root, the index in the array decreases
    {
        minHeapify(arr, n, i, hSize);
    }
}

void addNode(int element, List *list) {
        attr += 6;
        Node* newN = new Node();
        CustomData newCustomData;
        newCustomData.data = element;
        newCustomData.belongingListIndex = list->index;
        newN->customData = newCustomData;
        newN->next = list->lastAdded;
        newN->prev = NULL;
        ++comp;
        if (list->lastAdded == NULL) {
            attr += 2;
            list->firstAdded = newN;
            list->size = 0;
        }
        else {
            ++attr;
            (list->lastAdded)->prev = newN;

        }
        attr += 2;
        list->lastAdded = newN;
        ++list->size;
}

void deleteLastNode(List* list) {
    Node* temp = list->lastAdded;
    list->lastAdded = list->lastAdded->prev;
    free(temp);
}

void arrayToList(int arr[], int n, List *list) {
    for (int i = n - 1; i >= 0; --i) {
        addNode(arr[i], list);
    }
}

void printArr(int arr[], int n) {
    for (int i = 0; i < n; ++i) {
        cout << arr[i] << " ";
    }
    cout << "\n";
}

void printCustomDataArr(CustomData arr[], int n) {
    for (int i = 0; i < n; ++i) {
        cout << arr[i].data << " ";
    }
    cout << "\n";
}

void printList(List *list, boolean reversed) {
    if (!reversed) {
        Node* temp = list->lastAdded;
        while (temp != NULL) {
            cout << temp->customData.data << " ";
            temp = temp->next;
        }
        cout << "\n";
    }
    else {
        Node* temp = list->firstAdded;
        while (temp != NULL) {
            cout << temp->customData.data << " ";
            temp = temp->prev;
        }
        cout << "\n";
    }
}

void deleteAllLists(int k) {
    for (int i = 0; i < k; ++i) {
        if (lists[i]->lastAdded != NULL) {
            deleteList(lists[i]);
        }
    }
}

void deleteList(List* list) {
    list->size = 0;
    list->lastAdded = NULL;
    list->firstAdded = NULL;
    list->index = 0;
}

void printListFromNode(Node* node, boolean reversed) {
    if (!reversed) {
        Node* temp = node;
        while (temp != NULL) {
            cout << temp->customData.data << " ";
            temp = temp->next;
        }
        cout << "\n";
    }
    else {
        Node* temp = node;
        while (temp != NULL) {
            cout << temp->customData.data << " ";
            temp = temp->prev;
        }
        cout << "\n";
    }

}

void generateKRandomLists(int n, int k) {
    lists = (List**)malloc(k * sizeof(List*));
    int i = 0, temp = n;
    while (i < k) {
        int sizeOfCurrentList = randomNumber(1, n / k );
        temp -= sizeOfCurrentList;
        lists[i] = (List*)(new List());
        lists[i]->index = i;
        int* v = new int[sizeOfCurrentList];
        FillRandomArray(v, sizeOfCurrentList, 10, 100, false, 1);
        arrayToList(v, sizeOfCurrentList, (lists[i]));
        ++i;
    }
    while (temp > 0) {
        int randomListIndex = randomNumber(0, k - 1);
        addNode(lists[randomListIndex]->lastAdded->customData.data - randomNumber(0, 10), lists[randomListIndex]);
        --temp;
    }
}

void listToArray(CustomData arr[], List *list) {
    int n = list->size;
    Node* temp = list->lastAdded;
    for (int i = 0; i < n; ++i) {
        arr[i].data = temp->customData.data;
        temp = temp->next;
    }
}

void listToMinHeap(CustomData arr[], List *list) {
    listToArray(arr, list);
    buildMinHeap(arr, list->size, list->size);
}

void merge2Lists(int n, List* outputList) {
    int indexFirstList = 0;
    int indexSecondList = 0;
    int sizeFirstList = lists[0]->size;
    int sizeSecondList = lists[1]->size;
    Node* lastFromFirstList = lists[0]->lastAdded;
    Node* lastFromSecondList = lists[1]->lastAdded;
    while (indexFirstList < sizeFirstList && indexSecondList < sizeSecondList)
    {
        if (lastFromFirstList->customData.data < lastFromSecondList->customData.data)
        {
            if (lastFromFirstList != NULL) {
                addNode(lastFromFirstList->customData.data, outputList);
            }
            if (lastFromFirstList->next != NULL) {
                lastFromFirstList = lastFromFirstList->next;
            }
            else {
                lastFromFirstList == NULL;
            }
            ++indexFirstList;
        }
        else {
            if (lastFromSecondList != NULL)
                addNode(lastFromSecondList->customData.data, outputList);
            if (lastFromSecondList->next != NULL) {
                lastFromSecondList = lastFromSecondList->next;
            }
            else {
                lastFromSecondList = NULL;
            }
            ++indexSecondList;
        }
    }
    //Add remaining elements directly to output list
    if (indexFirstList < sizeFirstList) {
        while (lastFromFirstList != NULL) {
            addNode(lastFromFirstList->customData.data, outputList);
            lastFromFirstList = lastFromFirstList->next;
        }
    }
    if (indexSecondList < sizeSecondList) {
        while (lastFromSecondList != NULL) {
            addNode(lastFromSecondList->customData.data, outputList);
            lastFromSecondList = lastFromSecondList->next;
        }
    }
    
}

void mergeKLists(int n, int k, List* outputList) {
    CustomData* arr = new CustomData[k + 1];
    int hSize = 2;
    //lists = (List**)malloc(k * sizeof(List*));
    Node** auxNodes = (Node**)malloc(k * sizeof(Node*));
    //building initial min-heap
    for (int i = 0; i < k; ++i) {
        ++attr;
        auxNodes[i] = lists[i]->lastAdded;
    }
    for (int i = 0; i < k; ++i) {
        while (i < 2) { //first element of index 0 messes up topdown heap construction
            ++attr;
            arr[i + 1].data = auxNodes[i]->customData.data;
            arr[i + 1].belongingListIndex = auxNodes[i]->customData.belongingListIndex; 
            ++i;
        }
        hMinInsert(arr, k + 1, auxNodes[i]->customData, hSize);
        ++hSize;
    }
    
    //taking from built heap and putting into final output list, inserting new element from the k lists and restoring the min heap property
    boolean keepGoing = true;
    while(keepGoing) {
        addNode(arr[1].data, outputList); //save it in output list
        ++attr;
        int indexOfLastList = arr[1].belongingListIndex;
        ++attr;
        Node* temp = auxNodes[indexOfLastList];
        ++comp;
        if (temp->next != NULL) {
            attr += 2;
            temp = temp->next;
            auxNodes[indexOfLastList] = auxNodes[indexOfLastList]->next;
        }
        else {
            int l = 0;
            while (l < k) {
                ++comp;
                if (auxNodes[l]->next != NULL) {
                    attr += 2;
                    temp = auxNodes[l]->next;
                    auxNodes[l] = auxNodes[l]->next;
                    l = 0;
                    break;
                }
                else {
                    ++l;
                }//if exiting this while loop, it means there are no more elements left in any of the lists, so we are done
                if (l >= k) {
                    keepGoing = false;
                }
            }
        }
        ++attr;
        arr[1] = temp->customData; //override old element taken out with next one from its list
        minHeapify(arr, k + 1, 1, hSize); //restore heap property with newly replaced element, since we take from the top of the heap only once is sufficient        
    }
    //empty remaining elements from the heap
    ++attr;
    int removalIndex = k;
    while (hSize > 2) {
        ++attr;
        arr[1] = arr[removalIndex]; //override first with last element(since we already took out first into output list, we can delete it), then decrement heap size
        --hSize;
        minHeapify(arr, removalIndex + 1, 1, hSize); //restore heap property with newly replaced element, since we take from the top of the heap only once is sufficient
        addNode(arr[1].data, outputList); //save it in output list
        --removalIndex;
    }
    addNode(arr[2].data, outputList); //take out the last element from the heap, which will be the largest

}

void performOperationsTest() {
    for (int n = 100; n <= N_MAX; n += STEP_SIZE) {
        List* outputListDummy = new List();
        generateKRandomLists(n, K1);
        resetIndexes();
        mergeKLists(n, K1, outputListDummy);
        deleteList(outputListDummy);
        Operation opComp1 = p.createOperation("mergeK5_comp", n);
        Operation opAttr1 = p.createOperation("mergeK5_attr", n);
        opComp1.count(comp);
        opAttr1.count(attr);
        resetIndexes();
    }
    for (int n = 100; n <= N_MAX; n += STEP_SIZE) {
        List* outputListDummy = new List();
        generateKRandomLists(n, K2);
        resetIndexes();
        mergeKLists(n, K2, outputListDummy);
        deleteList(outputListDummy);
        Operation opComp2 = p.createOperation("mergeK10_comp", n);
        Operation opAttr2 = p.createOperation("mergeK10_attr", n);
        opComp2.count(comp);
        opAttr2.count(attr);
        resetIndexes();
    }
    for (int n = 100; n <= N_MAX; n += STEP_SIZE) {
        List* outputListDummy = new List();
        generateKRandomLists(n, K3);
        resetIndexes();
        mergeKLists(n, K3, outputListDummy);
        deleteList(outputListDummy);
        Operation opComp3 = p.createOperation("mergeK100_comp", n);
        Operation opAttr3 = p.createOperation("mergeK100_attr", n);
        opComp3.count(comp);
        opAttr3.count(attr);
        resetIndexes();
    }
    p.addSeries("mergeK5", "mergeK5_comp", "mergeK5_attr");
    p.addSeries("mergeK10", "mergeK10_comp", "mergeK10_attr");
    p.addSeries("mergeK100", "mergeK100_comp", "mergeK100_attr");

    p.createGroup("mergeK_total", "mergeK5", "mergeK10", "mergeK100");
    p.createGroup("mergeK_total_attr", "mergeK5_attr", "mergeK10_attr", "mergeK100_attr");
    p.createGroup("mergeK_total_comp", "mergeK5_comp", "mergeK10_comp", "mergeK100_comp");

    p.showReport();
}

void performSecondOperationsTest() {
    for (int k = 10; k <= K_MAX; k += K_INCREMENT) {
        List* outputListDummy = new List();
        generateKRandomLists(N_MAX, k);
        resetIndexes();
        mergeKLists(N_MAX, k, outputListDummy);
        deleteList(outputListDummy);
        Operation opCompK = p.createOperation("mergeK_comp", k);
        Operation opAttrK = p.createOperation("mergeK_attr", k);
        opCompK.count(comp);
        opAttrK.count(attr);
        resetIndexes();
    }
    p.addSeries("mergeK", "mergeK_comp", "mergeK_attr");
    p.createGroup("mergeK_total", "mergeK");
    p.createGroup("mergeK_total_attr", "mergeK_attr");
    p.createGroup("mergeK_total_comp", "mergeK_comp");

    p.showReport();
}

void demo() {
    int k, n;
    cout << "How many lists?\n";
    cin >> k;
    cout << "How many elements in total?\n";
    cin >> n;
    if (k <= 0 || n < k) {
        cout << "Invalid parameters. Please input some valid values for n and k.\n";
        exit(-1);
    }
    cout << "The test will be executed for k = " << k << " lists, with a total sum of n = " << n << " elements:\n";
    generateKRandomLists(n, k);
    int sum = 0;
    for (int i = 0; i < k; ++i) {
        cout << "Size of list " << i << " is: " << lists[i]->size <<"\n";
        printList(lists[i], false);
        sum += lists[i]->size;
    }
    cout << "\n\nThe sum of all list sizes is: " << sum << "\n";
    
    List* outputList = new List();
    if (k == 1) {
        outputList = lists[0];
        cout << "The final merged list is:\n";
        printList(outputList, false);
        cout << "Size of output list: " << outputList->size << "\n";
    }
    else if(k == 2) {
        merge2Lists(n, outputList);
        cout << "The final merged list is:\n";
        printList(outputList, true);
        cout << "Size of output list: " << outputList->size << "\n";
    }
    else {
        mergeKLists(n, k, outputList);
        cout << "The final merged list is:\n";
        printList(outputList, true);
        cout << "Size of output list: " << outputList->size << "\n";
    }
    
}

int randomNumber(int low, int high) {
    return(rand()%(high - low + 1) + low);
}

int main()
{
    srand(time(NULL));
    int decision = -1;
    do
    {
        cout << "Hello! Choose one option:\n";
        cout << " ******************************\n";
        cout << " * 0 - correctness            *\n * 1 - first operation test   *\n * 2 - second operation test  *\n";
        cout << " ******************************\n";
        cin >> decision;
    } while (decision < 0 || decision > 2);
    switch (decision) {
        case 0: {
            demo();
            break;
        }
        case 1: {
            performOperationsTest();
            break;
        }
        case 2: {
            performSecondOperationsTest();
            break;
        }
    }
}

