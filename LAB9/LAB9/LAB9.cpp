using namespace std;
#include <iostream>
#include <windows.h> //WinApi header for colored text
#include "Profiler.h"

/* CONCLUSIONS|OBSERVATIONS|INTERPRETATIONS
* 
*      Iterative traversals are harder to implement given their less obvious approach, but they yield a slightly better performance in terms of running time than
*      recursive ones do, because of their advantage of not using the call stack for recursive calls, of which half will terminate doing absolutely nothing because
*      half of the nodes will be tree nodes. The print operations are equal in both cases, as expected, otherwise we wouldn't be talking about correct algorithms since
*      it is mandatory to only print n nodes and not more or less, regardless of approach used.
* 
*      Quicksort is a great algorithm for really large inputs, because it does not require additional memory, sorts in place and is expected to run in O(nlgn) time.
*      Although it is already great news, we notice that insertion sort in particular runs really well, even better than quicksort on really small inputs(n < 30). 
*      Because of that, the idea of hybridization is born and so we notice how hybrid quick sort not only runs in less operations but also in considerably less time.
* 
*      With the help of the profiler library and some algebra for computing the difference between quicksort total operations(used as a refference point) and hybrid quick sort
*      total operations, we see an increasing graph, which then decreases(so far a parabola). After that, when it reaches 0 it bounces back and heads for infinity.
*      Because we cannot input negative values for the count method present in the profiler library, I used absolute value for negative values to avoid any errors.
* 
*      This explains the tendance to infinity after reaching 0. In reality this difference heads towards negative infinity, as quicksort becomes increasingly
*      better than hybrid quicksort the more the threshold approaches n. At threshold = n, we have expected performance of insertion sort which is O(n^2), creating
*      astronomical gaps between them at high n values. Knowing all of that, we know for sure that the optimal threshold value lies where the top of the first parabola
*      forms, as that indicates the point where quicksort is slowest compared to hybrid quicksort for same unsorted array as input.
* 
*      From my observations analyzing the graph generated with the help of the profiler library, that optimal value lies between 11 and 12, both
*      being really close to the best possible usage of insertion sort with quicksort.
*
*/

Profiler p("TraversalsQSHibridization");
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

//Constants
#define NR_TESTS 5
#define N 2500
#define STEP_SIZE 100
#define MAX_SIZE 1000
#define N2 10000

//Data structures
typedef struct Node {
    int key;
    bool black;
    Node* parent, * left, * right;
};

//Prototypes
void demo1();
void demo2();
void perf1();
void perf2(int thrshold, bool random);
void perf3(bool random);
void performRuntimeTest1();
void performRuntimeTest2(int threshold, bool random);
void resetIndexes();
int randomNumber(int low, int high);
Node* newNode(int key, bool black);
void iterativeTreePrint(Node* root, int direction, int order, bool show);
void recursiveTreePrint(Node* root, int indent, bool firstTime, int order, bool show);
void printNode(int data, int indentation, int parent, bool black, bool show);
void rightRotate(Node* x, Node*& root);
void leftRotate(Node* x, Node*& root);
void insertRB(Node*& root, Node* z);
void insertRBFixup(Node*& root, Node* z);
void buildRB(int n, Node*& outputRoot);
void quickSort(int arr[], int left, int right, bool random, bool hybrid);
int partition(int arr[], int left, int right, bool hybrid);
int randomPartition(int arr[], int left, int right, bool hybrid);
void swapVal(int& a, int& b);
void insertionSort(int arr[], int begin, int end);
void printArr(int arr[], int n);
void copyArray(int in[], int copy[], int n);
void hybridQSort(int arr[], int left, int right, bool random, int threshold);



//Globals
Node* TNIL = newNode(NULL, true);
int printRecursive, printIterative, compQS, attrQS, compHQS, attrHQS;

//Function definitions
void resetIndexes() {
    printRecursive = 0;
    printIterative = 0;
    compQS = 0;
    attrQS = 0;
    compHQS = 0;
    attrHQS = 0;
}

Node* newNode(int key, bool black) {
    Node* newN = new Node();
    newN->left = TNIL;
    newN->right = TNIL;
    newN->key = key;
    newN->black = black;
    newN->parent = TNIL;
    return newN;
}

void copyArray(int in[], int copy[], int n) {
    for (int i = 0; i < n; ++i) {
        copy[i] = in[i];
    }
}

void printArr(int arr[], int n) {
    for (int i = 0; i < n; ++i) {
        cout << arr[i] << " ";
    }
    cout << "\n";
}

int randomPartition(int arr[], int left, int right, bool hybrid) {
    hybrid ? ++attrHQS : ++attrQS;
    int i = randomNumber(left, right - 1);
    hybrid ? attrHQS += 3 : attrQS += 3;
    swapVal(arr[right], arr[i]);
    return partition(arr, left, right, hybrid);
}

int partition(int arr[], int left, int right, bool hybrid) {
    hybrid ? attrHQS += 1 : attrQS += 1;
    int x = arr[right];
    int i = left - 1;
    for (int j = left; j <= right - 1; ++j) {
        hybrid ? ++compHQS : ++compQS;
        if (arr[j] <= x) {
            ++i;
            hybrid ? attrHQS += 3 : attrQS += 3;
            swapVal(arr[i], arr[j]);
        }
    }
    hybrid ? attrHQS += 3 : attrQS += 3;
    swapVal(arr[i + 1], arr[right]);
    return i + 1;
}

void quickSort(int arr[], int left, int right, bool random, bool hybrid) {
    hybrid ? ++compHQS : ++compQS;
    if (left < right) {
        hybrid ? ++attrHQS : ++attrQS;
        int pivot = random ? randomPartition(arr, left, right, hybrid) : partition(arr, left, right, hybrid);
        quickSort(arr, left, pivot - 1, random, hybrid);
        quickSort(arr, pivot + 1, right, random, hybrid);
    }
}

void swapVal(int& a, int& b)
{
    int temp = a;
    a = b;
    b = temp;
}

void buildRB(int n, Node*& outputRoot) {
    for (int i = 1; i <= n; ++i) {
        Node* newN = newNode(i, false);
        insertRB(outputRoot, newN);
    }
}

void insertRB(Node*& root, Node* z) { //O(logn)
    Node* y = TNIL;
    Node* x = root;
    while (x != TNIL) { //loop that will advance through the tree based on the value of newly inserted node Z's key
        y = x;
        if (z->key < x->key) {
            x = x->left;
        }
        else {
            x = x->right;
        }
    }
    z->parent = y; //set parent of newly inserted node the node that we get to when we walk through the tree with the given inserted value
    if (y == TNIL) { //if y is null it means that root was null earlier and while loop got skipped
        root = z; //then make the root be the newly inserted node
    }
    else {
        if (z->key < y->key) { //this is where the new node will be inserted either to the left or the right of the corresponding node that y reached, given that y is not null
            y->left = z;
        }
        else {
            if (z->key > y->key) {
                y->right = z;
            }
            else {
                cout << "No duplicates allowed!\n";
                return;
            }
        }
    }
    //up to this point, the code behaves identically like a simple BST insertion but this time it is done iteratively instead of recursively, to allow the correction
    //of potential RB tree property violations right after each insertion
    insertRBFixup(root, z); //correct violations if any
}

void insertRBFixup(Node*& root, Node* z) {
    while (z->parent->black == false) { //if z's parent were to be black, we can be 100% sure that no RB property has been violated 
        if (z->parent == z->parent->parent->left) { //z's parent would equal left child of z's grandparent only if the tree was left heavy after inserting z
            Node* y = z->parent->parent->right; //z's uncle
            if (y->black == false) { //if red uncle
                z->parent->black = true; //solution for this case is to recolor, parent made black from red
                y->black = true; //uncle made black from red
                z->parent->parent->black = false; //grandparent must then be made red to preserve property
                z = z->parent->parent; //update iterator for while loop, in case an imbalance appears two levels higher
            }
            else {
                if (z == z->parent->right) { //both case 2 and 3 need to recolor z's parent and grandparent, but only case 2 needs a left rotation around z's parent
                    z = z->parent;           //if z equals the right child of its parent, we know for sure we need to perform a rotation
                    leftRotate(z, root);
                } //else it must mean that z equals the left child of its parent, either way z's parent and grandparent need recoloring and a right rotation is needed as
                  //the tree is currently too left heavy
                z->parent->black = true;
                z->parent->parent->black = false;
                rightRotate(z->parent->parent, root);
            }
        }
        else { //if z's parent doesn't equal the left child of z's grandparent, it only means that it must equal the right child of its grandparent, so the tree became right heavy after inserting z
            Node* y = z->parent->parent->left; //same thinking as before but mirrored
            if (y->black == false) { //same recoloring as before but mirrored
                z->parent->black = true;
                y->black = true;
                z->parent->parent->black = false;
                z = z->parent->parent;
            }
            else {
                if (z == z->parent->left) {
                    z = z->parent;
                    rightRotate(z, root);
                }
                z->parent->black = true;
                z->parent->parent->black = false;
                leftRotate(z->parent->parent, root);
            }
        }
    }
    root->black = true;
}

void printNode(int data, int indentation, int parent, bool black, bool show) { //k = 4 red color, k = 8 gray(black would be invisible on black background)
    for (int i = 0; i < indentation; ++i) {
        if (show) {
            cout << "    ";
        }
    }
    SetConsoleTextAttribute(hConsole, 15);
    if (show) {
        cout << data;
    }
    SetConsoleTextAttribute(hConsole, 7);
    if (show) {
        cout << "|p=" << parent << "|c=";
    }
    if (black) {
        SetConsoleTextAttribute(hConsole, 8);
        if (show) {
            cout << "B\n";
        }
    }
    else {
        SetConsoleTextAttribute(hConsole, 4);
        if (show) {
            cout << "R\n";
        }
    }
    SetConsoleTextAttribute(hConsole, 7);
}

void recursiveTreePrint(Node* root, int indent, bool firstTime, int order, bool show) {
    if (order < 0 || order > 2) {
        order = 0;
        SetConsoleTextAttribute(hConsole, 12);
        cout << "Invalid order, setting it to 0...\n";
        SetConsoleTextAttribute(hConsole, 7);
    }
    if (firstTime) {
        SetConsoleTextAttribute(hConsole, 11);
        if (show) {
            cout << "Recursive tree print:\n";
        }
        SetConsoleTextAttribute(hConsole, 7);
        firstTime = false;
    }
    if (root != TNIL && root->key != NULL) {
        if (order == 0) {
            ++printRecursive;
            printNode(root->key, indent, root->parent->key, root->black, show);
        }
        recursiveTreePrint(root->left, indent + 1, firstTime, order, show);
        if (order == 1) {
            ++printRecursive;
            printNode(root->key, indent, root->parent->key, root->black, show);
        }
        recursiveTreePrint(root->right, indent + 1, firstTime, order, show);
        if (order == 2) {
            ++printRecursive;
            printNode(root->key, indent, root->parent->key, root->black, show);
        }
    }
}

int randomNumber(int low, int high) {
    return(rand() % (high - low + 1) + low);
}

void rightRotate(Node* x, Node*& root) {
    Node* y = x->left;
    x->left = y->right;
    if (y->right != TNIL) {
        y->right->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == TNIL) {
        root = y;
    }
    else {
        if (x == x->parent->right) {
            x->parent->right = y;
        }
        else {
            x->parent->left = y;
        }
    }
    y->right = x;
    x->parent = y;
}

void leftRotate(Node* x, Node*& root) { //same logic like rightRotate but mirrored
    Node* y = x->right;
    x->right = y->left;
    if (y->left != TNIL) {
        y->left->parent = x;
    }
    y->parent = x->parent;
    if (x->parent == TNIL) {
        root = y;
    }
    else {
        if (x == x->parent->left) {
            x->parent->left = y;
        }
        else {
            x->parent->right = y;
        }
    }
    y->left = x;
    x->parent = y;
}

void iterativeTreePrint(Node* root, int direction, int order, bool show) { //0 - preorder | 1 - inorder | 2 - postorder
    if (order < 0 || order > 2) {
        order = 0;
        SetConsoleTextAttribute(hConsole, 12);
        cout << "Invalid traversal, setting it to 0...\n";
        SetConsoleTextAttribute(hConsole, 7);
    }
    SetConsoleTextAttribute(hConsole, 13);
    if (show) {
        cout << "Iterative tree print:\n";
    }
    SetConsoleTextAttribute(hConsole, 7);
    Node* node = root;
    int indent = 0;
    do {
        if (direction == 1) {
            if (order == 0) {
                ++printIterative;
                printNode(node->key, indent, node->parent->key, node->black, show);
            }
            ++indent;
            if (node->left != TNIL) {
                node = node->left; //advance to the left with direction still 1
            }
            else {
                direction = 2;
            }
        }
        if (direction == 2) {
            if (order == 1) {
                ++printIterative;
                printNode(node->key, indent, node->parent->key, node->black, show);
            }
            if (node->right != TNIL) {
                node = node->right; //advance to the right -> first time
                direction = 1;
            }
            else {
                direction = 3;     //set direction to 3 as you meet node third time -> advance to parent
            }
        }
        if (direction == 3) {      //we are not done
            if (order == 2) {
                ++printIterative;
                printNode(node->key, indent, node->parent->key, node->black, show);
            }
            --indent;
            if (node->parent != TNIL) {
                if (node == node->parent->left) {
                    direction = 2;            //else remains on 3
                }
                node = node->parent;      //advance to the parent
            }
        }
    } while (node != root || direction != 3); //node == root means node->parent == TNIL
    if (order == 2) {
        ++printIterative;
        printNode(node->key, indent, node->parent->key, node->black, show);
    }
}

void hybridQSort(int arr[], int left, int right, bool random, int threshold) {
    ++compHQS;
    if (left < right) {
        ++compHQS;
        if (right - left < threshold) {
            insertionSort(arr, left, right + 1); //sort smaller part of the array using insertionSort
        }
        else {
            ++attrHQS;
            int pivot = random ? randomPartition(arr, left, right, true) : partition(arr, left, right, true);
            hybridQSort(arr, left, pivot - 1, random, threshold);
            hybridQSort(arr, pivot + 1, right, random, threshold);
        }
    }
}

void insertionSort(int arr[], int begin, int end) {
    for (int i = begin + 1; i < end; ++i) {
        ++attrHQS;
        int aux = arr[i];
        int j = i - 1;
        while (arr[j] > aux && j >= 0) {
            ++compHQS;
            ++attrHQS;
            arr[j + 1] = arr[j];
            j = j - 1;
        }
        ++compHQS;
        ++attrHQS;
        arr[j + 1] = aux;
    }
}

void demo2() {
    int v1[] = { 5, -6, 15, 20, -6, 0, 4, 9, 2, 3, 0, 11 };
    int n = sizeof(v1) / sizeof(int);
    int l = 0, r = n - 1;
    int* v = new int[n];
    int* v2 = new int[n];
    int* v3 = new int[n];
    copyArray(v1, v2, n);
    copyArray(v1, v3, n);
    copyArray(v1, v, n);
    SetConsoleTextAttribute(hConsole, 10);
    cout << "Example hardcoded array prior to sorting:\n";
    SetConsoleTextAttribute(hConsole, 15);
    printArr(v1, n);
    SetConsoleTextAttribute(hConsole, 12);
    cout << "InsertionSort:\n";
    SetConsoleTextAttribute(hConsole, 15);
    insertionSort(v1, 0, n);
    printArr(v1, n);
    SetConsoleTextAttribute(hConsole, 13);
    cout << "QuickSort(last pos pivot):\n";
    SetConsoleTextAttribute(hConsole, 15);
    quickSort(v2, l, r, false, false);
    printArr(v2, n);
    SetConsoleTextAttribute(hConsole, 14);
    cout << "QuickSort(random pivot):\n";
    SetConsoleTextAttribute(hConsole, 15);
    quickSort(v3, l, r, true, false);
    printArr(v3, n);

    int again = 1;
    do {
        int* v4 = new int[n];
        int* v5 = new int[n];
        copyArray(v, v4, n);
        copyArray(v, v5, n);
        int x = -1;
        do {
            SetConsoleTextAttribute(hConsole, 5);
            cout << "Please input a threshold value for the hybrid quicksort algorithm:\n";
            SetConsoleTextAttribute(hConsole, 6);
            cout << "x=";
            cin >> x;
        } while (x < 0 || x > n);

        SetConsoleTextAttribute(hConsole, 9);
        cout << "HybridQuickSort(last pos pivot, threshold = x):\n";
        SetConsoleTextAttribute(hConsole, 15);
        hybridQSort(v4, l, r, false, x);
        printArr(v4, n);
        SetConsoleTextAttribute(hConsole, 6);
        cout << "HybridQuickSort(random pivot, threshold = x):\n";
        SetConsoleTextAttribute(hConsole, 15);
        hybridQSort(v5, l, r, false, x);
        printArr(v5, n);
        cout << "\n";
        SetConsoleTextAttribute(hConsole, 8);
        cout << "Try again?(1/0)\n";
        cin >> again;
    } while (again == 1);
}

void demo1() {
    int repeat = 1;
    do {
        SetConsoleTextAttribute(hConsole, 10);
        cout << "Hello! Choose an option:\n";
        SetConsoleTextAttribute(hConsole, 15);
        cout << " 0 - ";
        SetConsoleTextAttribute(hConsole, 11);
        cout << "recursiveTreePrint\n";
        SetConsoleTextAttribute(hConsole, 7);
        SetConsoleTextAttribute(hConsole, 15);
        cout << " 1 - ";
        SetConsoleTextAttribute(hConsole, 13);
        cout << "iterativeTreePrint\n";
        SetConsoleTextAttribute(hConsole, 7);
        int decision = -1;
        int order = -1;
        Node* tree1 = TNIL;
        buildRB(11, tree1);
        cin >> decision;
        SetConsoleTextAttribute(hConsole, 10);
        cout << "Choose traversal:\n";
        SetConsoleTextAttribute(hConsole, 15);
        cout << " 0 - ";
        SetConsoleTextAttribute(hConsole, 10);
        cout << "preorder\n";
        SetConsoleTextAttribute(hConsole, 7);
        SetConsoleTextAttribute(hConsole, 15);
        cout << " 1 - ";
        SetConsoleTextAttribute(hConsole, 14);
        cout << "inorder\n";
        SetConsoleTextAttribute(hConsole, 15);
        cout << " 2 - ";
        SetConsoleTextAttribute(hConsole, 9);
        cout << "postorder\n";
        SetConsoleTextAttribute(hConsole, 7);
        cin >> order;
        if (decision == 0) {
            recursiveTreePrint(tree1, 0, true, order, true);
        }
        else if (decision == 1) {
            int direction = 1;
            iterativeTreePrint(tree1, direction, order, true);
        }
        SetConsoleTextAttribute(hConsole, 8);
        cout << "Try again?(1/0)\n";
        SetConsoleTextAttribute(hConsole, 7);
        cin >> repeat;
    } while (repeat == 1);
}

void perf1() {
    SetConsoleTextAttribute(hConsole, 10);
    cout << "Please wait, this might take a while...\n";
    SetConsoleTextAttribute(hConsole, 7);
    for (int n = STEP_SIZE; n < N; n += STEP_SIZE) {
        Node* tree2 = TNIL;
        buildRB(n, tree2);
        for (int i = 0; i < NR_TESTS; ++i) {
            resetIndexes();
            int order = 1, direction = 1;                              //will test with preoder, can be changed from here if desired
            recursiveTreePrint(tree2, 0, true, order, false);
            iterativeTreePrint(tree2, direction, order, false);
            Operation opRec = p.createOperation("recursive-printOperations", n);
            Operation opItr = p.createOperation("iterative-printOperations", n);
            opRec.count(printRecursive);
            opItr.count(printIterative);
        }
    }
    p.divideValues("recursive-printOperations", NR_TESTS);
    p.divideValues("iterative-printOperations", NR_TESTS);
    p.createGroup("Traversals", "recursive-printOperations", "iterative-printOperations");
    performRuntimeTest1();
    p.showReport();
}

void perf2(int threshold, bool random) {
    SetConsoleTextAttribute(hConsole, 10);
    cout << "Please wait, this might take a while...\n";
    SetConsoleTextAttribute(hConsole, 7);
    for (int n = STEP_SIZE; n < N2; n += STEP_SIZE) {
        for (int i = 0; i < NR_TESTS; ++i) {
            resetIndexes();
            int* v = new int[n];
            int* v1 = new int[n];
            FillRandomArray(v, n, 10, 50000, false, 0);
            copyArray(v, v1, n);
            
            quickSort(v, 0, n - 1, random, false);
            hybridQSort(v1, 0, n - 1, random, threshold);
            Operation compQSort = p.createOperation("quickSort-comp", n);
            Operation attrQSort = p.createOperation("quickSort-attr", n);
            Operation compHQSort = p.createOperation("hybridQuickSort-comp", n);
            Operation attrHQSort = p.createOperation("hybridQuickSort-attr", n);
            compQSort.count(compQS);
            attrQSort.count(attrQS);
            compHQSort.count(compHQS);
            attrHQSort.count(attrHQS);
        }
    }
    p.divideValues("quickSort-comp", NR_TESTS);
    p.divideValues("quickSort-attr", NR_TESTS);
    p.divideValues("hybridQuickSort-comp", NR_TESTS);
    p.divideValues("hybridQuickSort-attr", NR_TESTS);
    p.addSeries("quickSort", "quickSort-attr", "quickSort-comp");
    p.addSeries("hybridQuickSort", "hybridQuickSort-attr", "hybridQuickSort-comp");
    p.createGroup("Total", "quickSort", "hybridQuickSort");
    p.createGroup("Total-Comp", "quickSort-comp", "hybridQuickSort-comp");
    p.createGroup("Total-Attr", "quickSort-attr", "hybridQuickSort-attr");
    performRuntimeTest2(threshold, random);
    p.showReport();
}

void performRuntimeTest1() {
    for (int n = STEP_SIZE; n <= MAX_SIZE; n += STEP_SIZE)
    {
        Node* tree = TNIL;
        buildRB(n, tree);
        for (int i = 0; i < NR_TESTS; ++i)
        {
            int order = 1, direction = 1;                              //will test with preoder, can be changed from here if desired
            p.startTimer("recursivePrint", n);
            recursiveTreePrint(tree, 0, true, order, false);
            p.stopTimer("recursivePrint", n);

            p.startTimer("iterativePrint", n);
            iterativeTreePrint(tree, direction, order, false);
            p.stopTimer("iterativePrint", n);
        }
    }
    p.divideValues("recursivePrint", NR_TESTS);
    p.divideValues("iterativePrint", NR_TESTS);
    p.createGroup("Time", "recursivePrint", "iterativePrint");
}

void performRuntimeTest2(int threshold, bool random) {
    int v[MAX_SIZE * 10];
    int v1[MAX_SIZE * 10];
    for (int n = STEP_SIZE; n <= MAX_SIZE * 10; n += STEP_SIZE)
    {
        for (int i = 0; i < NR_TESTS * 20; ++i)
        {
            FillRandomArray(v, n);
            copyArray(v, v1, n);

            p.startTimer("quickSortTime", n);
            quickSort(v, 0, n - 1, random, false);
            p.stopTimer("quickSortTime", n);

            p.startTimer("hybridQSortTime", n);
            hybridQSort(v1, 0, n - 1, random, threshold);
            p.stopTimer("hybridQSortTime", n);
        }
    }
    p.divideValues("quickSortTime", NR_TESTS * 20);
    p.divideValues("hybridQSortTime", NR_TESTS * 20);
    p.createGroup("Regular VS Hybrid", "quickSortTime", "hybridQSortTime");
}

void perf3(bool random) {
    SetConsoleTextAttribute(hConsole, 10);
    cout << "Please wait, this might take a while...\n";
    SetConsoleTextAttribute(hConsole, 7);
    int* v = new int[N];
    int* v1 = new int[N];
    int* v2 = new int[N];
    FillRandomArray(v, N, 10, 50000, false, 0);
    for (int t = 0; t < N / 10; ++t) {
        copyArray(v, v1, N);
        copyArray(v, v2, N);
        resetIndexes();
        quickSort(v1, 0, N - 1, random, false);
        hybridQSort(v2, 0, N - 1, random, t);
        Operation differenceOp = p.createOperation("Threshold-perf-diff", t);
        int difference = (compQS + attrQS) - (compHQS + attrHQS);
        if (difference < 0) {
            difference *= -1;
        }
        differenceOp.count(difference);
    }
    p.showReport();
}

int main() {
    srand(time(NULL));
    int repeat = 0;
    do {
        int decision = -1;
        do
        {
            SetConsoleTextAttribute(hConsole, 14);
            cout << "Hello! Choose one option:\n";
            SetConsoleTextAttribute(hConsole, 10);
            cout << " ***************************************\n";
            cout << " * 0 - Traversals Demo                 *\n * 1 - Traversals Performance Report   *\n";
            cout << " * 2 - Hybrid QSort Demo               *\n * 3 - Hybrid QSort Performance Report *\n";
            cout << " * 4 - Optimal Threshold Determination *\n";
            cout << " ***************************************\n";
            SetConsoleTextAttribute(hConsole, 7);
            cin >> decision;
        } while (decision < 0 || decision > 4);
        switch (decision) {
        case 0: {
            demo1();
            break;
        }
        case 1: {
            perf1();
            break;
        }
        case 2: {
            demo2();
            break;
        }
        case 3: {
            perf2(12, false);
            break;
        }
        case 4: {
            perf3(false);
            break;
        }
        default: {
            cout << "Invalid decision ID\n";
            break;
        }
        }
        cout << "Repeat?(1/0)\n";
        cin >> repeat;
    } while (repeat == 1);
}
