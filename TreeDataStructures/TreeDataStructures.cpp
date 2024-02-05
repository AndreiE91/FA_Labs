using namespace std;
#include <iostream>

/*
       For R2 transformation I used a data structure consisting of the key, an array of hard-coded size of 100 and nr of children for each node in the tree.
       This can be inefficient memory usage in case of very small numbers of children, can also completely go wrong in case of many more than 100 children.
       It could be that a linked list is better, although that depends on the use case, as it might have the advantage of dynamic size but it lacks in access
       speed of an array. If speed is a concern, array, else if space is a concern, linked list.

       R3 is done much more efficiently, with only an int for key and two pointers for left and right children. This does not waste any unnecessary memory.
       The R3 print has been altered so that only left nodes increment the indentation when traversing and printing in pre-order. The rest of the trees have
       their own different methods which all yield the same prettyPrint output but in different ways to accomodate for the different data structures.

       The efficiency of the first convert algorithm, from R1 to R2 runs in O(kn) time, where k is the maximum number of children that a node can have. This is because
       the algorithm is implemented with a for loop which iterates through all members of the array, ignoring the nulls. This also requires to set each member of the array
       to null beforehand with another for loop. As each time we check for a node, we will have k steps in the for loop(k is size of the array/max nr of children), and we
       run through the for loop as many times as the function is called, which is n times(the number of all nodes we have in the tree), the final performance evaluates to
       the product of number of nodes and how many children each can have at most, giving O(kn) performance in all cases, there being no worst, average or best, as the
       for loop runs no matter what at all times. This is where a little improvement could be done, although I wanted to play it safe and be sure I don't miss anything
       when I first implemented the algorithm.

       The second conversion from R2 to R3 also runs in 0(kn) time, as it knows exactly when to stop thanks to the nrKids present in the R2 struct, enabling
       the algorithm to never run more times than needed. This in turn creates differences from the best to the worst case, the best case being O(n), where each node has only one child
       worst case being O(kn), where each node has k children, and average being somewhere in the middle, where a node might have k children, others none, others 1, others k-1 etc.

       Both conversion algorithms employ O(1) constant additional memory, used only for a counter, a boolean which tells whether its the first time running the algorithm or not, and
       a struct pointer for storing the root as an output for future use such as printing the tree later, and in the case of R2toR3 another struct pointer for remembering 
       the last accessed node, needed for creating brother relationships of the nodes.
*/

typedef struct R1 {
    int key;
    int parent;
};

typedef struct R2 {
    int key;
    R2* children[100];
    int nrKids;
};

typedef struct R3 {
    int key;
    R3* left;
    R3* right;
};

//PROTOTYPES
void R1toR2(int root, R1 structKPV[], int size, R2* outputRoot, bool firstTime);
R2* newR2(int key);
R3* newR3(int key);
void printR2(R2* root, int indent, bool firsTime);
void R2toR3(R2 *root, R3 * outputRoot, R3*lastAccessed, bool firstTime);
void printR3(R3* root, int indent, bool firstTime);
void debugPrintStructKPV(R1 structKPV[], int size);
void printNode(int data, int indentation);
void printRR1(int root, R1 structKPV[], int size, int indent, bool firstTimeRealRoot);
int findRootInR1(R1 structKPV[], int size);
void demo();

R3* newR3(int key) {
    R3* newN = new R3();
    newN->key = key;
    newN->left = NULL;
    newN->right = NULL;
    return newN;
}

//additional memory in lastAccessed struct pointer, firstTime boolean and count variable, plus outputRoot struct pointer; O(1) constant additional memory
void R2toR3(R2* root, R3* outputRoot, R3* lastAccessed, bool firstTime) { //O(kn), will not pass twice through same node, while loop stops exactly where it needs to
    if (firstTime) {
        outputRoot->key = root->key;
        outputRoot->left = NULL;
        outputRoot->right = NULL;
        firstTime = false;
    }
    int count = 0;
    while (count < root->nrKids) {
        R3* newNode = newR3(root->children[count]->key);
        //cout << "Passed through node = " << root->children[count]->key << "\n";
        if (count == 0) {
            outputRoot->left = newNode;
            lastAccessed = outputRoot->left;
        } else {
            lastAccessed->right = newNode;
            lastAccessed = lastAccessed->right;
        }
        R2toR3(root->children[count], newNode, lastAccessed, firstTime);
        ++count;
    }
}

void printR3(R3* root, int indent, bool firstTime) {
    if (firstTime) {
        cout << "R3 print:\n";
        firstTime = false;
    }
    if (root != NULL) {
        printNode(root->key, indent);
        printR3(root->left, indent + 1, firstTime);
        printR3(root->right, indent, firstTime);
    }
}


R2* newR2(int key) {
    R2 *newNode = new R2();
    newNode->key = key;
    newNode->nrKids = 0;
    for (int i = 0; i < 100; ++i) {
        newNode->children[i] = NULL;
    }
    return newNode;
}

//additional memory in fisrtTime boolean and count integer plus the outputRoot struct pointer; O(1) constant additional memory
void R1toR2(int root, R1 structKPV[], int size, R2 *outputRoot, bool firstTime) { //O(kn), where k is the maximum number of subtrees that a node can have
    if (firstTime) {
        outputRoot->key = root;
        outputRoot->nrKids = 0;
        for (int i = 0; i < 100; ++i) {
            outputRoot->children[i] = NULL;
        }
        firstTime = false;
    }
    int count = 0;
    for (int i = 0; i < size; ++i) {
        if (structKPV[i].parent == root) {
            R2 *newNode = newR2(structKPV[i].key);
            outputRoot->children[count] = newNode;
            ++count;
            outputRoot->nrKids = count;
            R1toR2(newNode->key, structKPV, size, newNode, firstTime);
        }
    }
}

void printR2(R2 *root, int indent, bool firstTime) {
    if (firstTime) {
        cout << "R2 print:\n";
        printNode(root->key, indent);
        //cout << "nr kids = " << root->nrKids << "\n";
        ++indent;
        firstTime = false;
    }
    int count = 0;
    while (root->children[count] != NULL) {
        printNode(root->children[count]->key, indent);
        //cout << "nr kids = " << root->children[count]->nrKids << "\n";
        printR2(root->children[count], indent + 1, firstTime);
        ++count;
    }
    return;
}

void debugPrintStructKPV(R1 structKPV[], int size) {
    cout << "Will print an array which associates every node in the tree with its corresponding parent for the R1 structure case:\n";
    for (int i = 0; i < size; ++i) {
        cout << "Node " << structKPV[i].key << " with parent " << structKPV[i].parent << "\n";
    }
    cout << "\n";
}

void printNode(int data, int indentation) {
    for (int i = 0; i < indentation; ++i) {
        cout << "    ";
    }
    cout << data << "\n";
}

void printRR1(int root, R1 structKPV[], int size, int indent, bool firstTimeRealRoot) {
    if (firstTimeRealRoot) {
        firstTimeRealRoot = false;
        cout << "R1 print:\n";
        printNode(root, indent);
        ++indent;
    }
    for (int i = 0; i < size; ++i) {
        if (root == structKPV[i].parent) {
            printNode(structKPV[i].key, indent);
            printRR1(structKPV[i].key, structKPV, size, indent + 1, firstTimeRealRoot);
        }
    }
}

int findRootInR1(R1 structKPV[], int size) {
    for (int i = 0; i < size; ++i) {
        if (structKPV[i].parent == -1) {
            return structKPV[i].key;
        }
    }
    cout << "Couldn't find any root\n";
    return -1;
}

void demo() {
    int keyParentVector[] = {2, 7, 5, 2, 7, 7, -1, 5, 2, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int keyParentVectorSize = sizeof(keyParentVector) / sizeof(keyParentVector[0]);
    R1 *structKPV = new R1[keyParentVectorSize / 2];
    for (int i = 0; i < keyParentVectorSize / 2; ++i) {
        structKPV[i].parent = keyParentVector[i];
        structKPV[i].key = keyParentVector[i + keyParentVectorSize / 2];
    }
    debugPrintStructKPV(structKPV, keyParentVectorSize / 2);
    int root = findRootInR1(structKPV, keyParentVectorSize / 2);
    printRR1(root, structKPV, keyParentVectorSize / 2, 0, true);
    cout << "Converting from R1 to R2...\n";
    R2 *root2 = new R2;
    R1toR2(root, structKPV, keyParentVectorSize / 2, root2, true);
    printR2(root2, 0, true);
    cout << "Converting from R2 to R3...\n";
    R3* root3 = new R3;
    R2toR3(root2, root3, NULL, true);
    printR3(root3, 0, true);
}

int main()
{
    demo();
}

