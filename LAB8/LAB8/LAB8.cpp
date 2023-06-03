using namespace std;
#include <iostream>
#include <windows.h> //WinApi header for colored text
#include "Profiler.h"

/* CONCLUSIONS|OBSERVATIONS|INTERPRETATIONS
* 
*   Red-Black trees are incredibly good for searching, but slower for maintaining(intertion/deletion). A great practical use of such an augmented data structure
*   could be a phone book, where people need to search for phone numbers all the time but rarely insert or delete numbers. As it can be noticed in the profiler
*   report, while building the tree takes the longest, as it is O(nlogn), an insertion taking logn time and, there being n nodes, we multiply and get nlogn. It can be
    noticed how by dividing the obtained values from buildTree() by n we obtain also a logn value similar to the delete and search operations, which are really fast.
*   
*   Calculating the size of each node takes O(n) time, but for insertions it is done only once after building the whole tree, not affecting performance. 
*   For deletions, it is done after each deletion, slowing the deletion down to O(n) if size must also be kept consistent. 
*/

Profiler p("OSTrees");

#define NR_TESTS 5
#define N 10000
#define STEP_SIZE 100

int compSearch, compDelete, attrSearch, attrDelete, compBuild, attrBuild;

//RB implementation
typedef struct Node {
    int key, size;
    bool black;
    Node* parent, *left, *right;
};

//Prototypes
void demo();
void perf();
void resetIndexes();
int randomNumber(int low, int high);
Node* newNode(int key, bool black);
void buildRB(int n, Node*& outputRoot);
Node* insertBST(Node*& root, Node* newNode);
void insertRB(Node*& root, Node* z);
void insertRBFixup(Node*& root, Node* z);
Node* OSSelect(Node* x, int i);
void RBTransplant(Node*& root, Node* u, Node* v);
void RBDelete(Node*& root, Node* z);
void RBDeleteFixup(Node*& root, Node* x);
void printTree(Node* root, int indent, bool firstTime);
void printNode(int data, int size, int indentation, int parent, bool black);
int getHeight(Node* root);
int getBalance(Node* node);
void rightRotate(Node* x, Node*& root, bool fromInsert);
void leftRotate(Node* x,Node*& root, bool fromInsert);
int getSize(Node*& node, bool setRoot, bool fromInsert);
Node* treeMinimum(Node* x);
Node* treeMaximum(Node* x);

Node* TNIL = newNode(NULL, true);

HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

void resetIndexes() {
    compSearch = 0;
    compDelete = 0;
    attrSearch = 0;
    attrDelete = 0;
    compBuild = 0;
    attrBuild = 0;
}

Node* OSSelect(Node* x, int i) {
    ++compSearch;
    if (x == TNIL) {
        cout << "Node doesn't exist in the tree!\n";
        return TNIL;
    }
    ++attrSearch;
    int r = x->left->size + 1; //compute rank of node x within subtree rooted at x, number of nodes that come before node x in an inorder tree walk
    ++compSearch;
    if (i == r) {              //we know that for a bst, an inorder tree walk gives a sorted list of nodes so rank of an element can be computed this way
        return x;
    }
    else {
        ++compSearch;
        if (i < r) {
            return OSSelect(x->left, i);
        }
        else {
            return OSSelect(x->right, i - r);
        }
    }
}

void RBTransplant(Node*& root, Node* u, Node* v) {
    ++compDelete;
    if (u->parent == TNIL) {
        ++attrDelete;
        root = v;
    }
    else {
        ++compDelete;
        if (u == u->parent->left) {
            ++attrDelete;
            u->parent->left = v;
        }
        else {
            ++attrDelete;
            u->parent->right = v;
        }
    }
    ++attrDelete;
    v->parent = u->parent;
}


void rightRotate(Node* x, Node*& root, bool fromInsert) { 
    fromInsert ? (attrBuild += 2) : (attrDelete += 2);
    Node* y = x->left;
    x->left = y->right;
    fromInsert ? (compBuild += 1) : (compDelete += 1);
    if (y->right != TNIL) {
        fromInsert ? (attrBuild += 1) : (attrDelete += 1);
        y->right->parent = x;
    }
    fromInsert ? (attrBuild += 1) : (attrDelete += 1);
    y->parent = x->parent;
    fromInsert ? (compBuild += 1) : (compDelete += 1);
    if (x->parent == TNIL) {
        fromInsert ? (attrBuild += 1) : (attrDelete += 1);
        root = y;
    }
    else {
        fromInsert ? (compBuild += 1) : (compDelete += 1);
        if (x == x->parent->right) {
            fromInsert ? (attrBuild += 1) : (attrDelete += 1);
            x->parent->right = y;
        }
        else {
            fromInsert ? (attrBuild += 1) : (attrDelete += 1);
            x->parent->left = y;
        }
    }
    fromInsert ? (attrBuild += 2) : (attrDelete += 2);
    y->right = x;
    x->parent = y;
}

void leftRotate(Node* x, Node*& root, bool fromInsert) { //same logic like rightRotate but mirrored
    fromInsert ? (attrBuild += 2) : (attrDelete += 2);
    Node* y = x->right;
    x->right = y->left;
    fromInsert ? (compBuild += 1) : (compDelete += 1);
    if (y->left != TNIL) {
        fromInsert ? (attrBuild += 1) : (attrDelete += 1);
        y->left->parent = x;
    }
    fromInsert ? (attrBuild += 1) : (attrDelete += 1);
    y->parent = x->parent;
    fromInsert ? (compBuild += 1) : (compDelete += 1);
    if (x->parent == TNIL) {
        fromInsert ? (attrBuild += 1) : (attrDelete += 1);
        root = y;
    }
    else {
        fromInsert ? (compBuild += 1) : (compDelete += 1);
        if (x == x->parent->left) {
            fromInsert ? (attrBuild += 1) : (attrDelete += 1);
            x->parent->left = y;
        }
        else {
            fromInsert ? (attrBuild += 1) : (attrDelete += 1);
            x->parent->right = y;
        }
    }
    fromInsert ? (attrBuild += 2) : (attrDelete += 2);
    y->left = x;
    x->parent = y;
}

int getBalance(Node* node) {
    if (node == NULL) {
        return -1;
    }
    else {
        return(getHeight(node->left) - getHeight(node->right));
    }
}

int getHeight(Node* root) { // O(n) as we need to iterate through each node to make sure height is maximum length path(in edges)
    //recursion exit condition
    if (root == TNIL) { //root becomes NULL only at the leaf nodes
        return -1;//we need it to return -1 so we can add +1 at the return call and preserve correctness
    }
    else {
        int heightLeft = getHeight(root->left);
        int heightRight = getHeight(root->right);
        if (heightLeft > heightRight) {
            return heightLeft + 1;//allows incrementation of height when going up a level of recursion in the tree
        }
        else {
            return heightRight + 1;
        }
    }
}


int randomNumber(int low, int high) {
    return(rand() % (high - low + 1) + low);
}

void printTree(Node* root, int indent, bool firstTime) {
    if (firstTime) {
        SetConsoleTextAttribute(hConsole, 11);
        cout << "Tree print:\n";
        SetConsoleTextAttribute(hConsole, 7);
        firstTime = false;
    }
    if (root != TNIL && root->key != NULL) {
        printNode(root->key, root->size, indent, root->parent->key, root->black);
        printTree(root->left, indent + 1, firstTime);
        printTree(root->right, indent + 1, firstTime);
    }
}

void printNode(int data, int size, int indentation, int parent, bool black) { //k = 4 red color, k = 8 gray(black would be invisible on black background)
    for (int i = 0; i < indentation; ++i) {
        cout << "    ";
    }
    SetConsoleTextAttribute(hConsole, 15);
    cout << data;
    SetConsoleTextAttribute(hConsole, 7);
    cout <<"  |s=" << size << "|p=" << parent << "|c=";
    if (black) {
        SetConsoleTextAttribute(hConsole, 8);
        cout << "B\n";
    }
    else {
        SetConsoleTextAttribute(hConsole, 4);
        cout << "R\n";
    }
    SetConsoleTextAttribute(hConsole, 7);
}

Node* insertBST(Node*& root, Node* newNode) { //O(h) where h is height of tree/subtree rooted at node "root"
    if (root == TNIL) {
        root = newNode;
        return root;
    }
    if (newNode->key < root->key) {
        ++root->size;
        root->left = insertBST(root->left, newNode);
        root->left->parent = root;
    }
    else if (newNode->key > root->key) {
        ++root->size;
        root->right = insertBST(root->right, newNode);
        root->right->parent = root;
    }
    else {
        cout << "No duplicate node values allowed!\n";
        return root;
    }
    return root;
}

int getSize(Node*& node, bool setRoot, bool fromInsert) {//O(n) as all tree/sub-tree must be passed, with O(n) calls on the stack (additional memory)
    //fromInsert ? (compBuild += 1) : (compDelete += 1);
    if (node == TNIL) { //won't affect performance of buildTree as it is already O(nlogn), but if it is run after each insertion it will make it O(n) instead of O(logn)
        return 0;
    }
    else {
        //fromInsert ? (compBuild += 1) : (compDelete += 1);
        if (!setRoot && node->parent == TNIL) {
            //fromInsert ? (attrBuild += 1) : (attrDelete += 1);
            node->size = getSize(node, true, fromInsert);
        }
        //fromInsert ? (attrBuild += 1) : (attrDelete += 1);
        int leftSize = getSize(node->left, setRoot, fromInsert);
        //fromInsert ? (attrBuild += 1) : (attrDelete += 1);
        node->left->size = leftSize;
        //fromInsert ? (attrBuild += 1) : (attrDelete += 1);
        int rightSize = getSize(node->right, setRoot, fromInsert);
        //fromInsert ? (attrBuild += 1) : (attrDelete += 1);
        node->right->size = rightSize;
        return(leftSize + 1 + rightSize);
    }
}

void insertRB(Node*& root, Node* z) { //O(logn)
    attrBuild += 2;
    Node* y = TNIL;
    Node* x = root;
    while (x != TNIL) { //loop that will advance through the tree based on the value of newly inserted node Z's key
        ++compBuild;
        ++attrBuild;
        y = x;
        ++compBuild;
        if (z->key < x->key) {
            ++attrBuild;
            x = x->left;
        }
        else {
            ++attrBuild;
            x = x->right;
        }
    }
    ++compBuild;
    ++attrBuild;
    z->parent = y; //set parent of newly inserted node the node that we get to when we walk through the tree with the given inserted value
    ++compBuild;
    if (y == TNIL) { //if y is null it means that root was null earlier and while loop got skipped
        ++attrBuild;
        root = z; //then make the root be the newly inserted node
    }
    else {
        ++compBuild;
        if (z->key < y->key) { //this is where the new node will be inserted either to the left or the right of the corresponding node that y reached, given that y is not null
            ++attrBuild;
            y->left = z;
        }
        else {
            ++compBuild;
            if (z->key > y->key) {
                ++attrBuild;
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
    insertRBFixup(root , z); //correct violations if any
}

void insertRBFixup(Node*& root, Node* z) {
    while (z->parent->black == false) { //if z's parent were to be black, we can be 100% sure that no RB property has been violated 
        ++compBuild;
        ++compBuild;
        if (z->parent == z->parent->parent->left) { //z's parent would equal left child of z's grandparent only if the tree was left heavy after inserting z
            ++attrBuild;
            Node* y = z->parent->parent->right; //z's uncle
            ++compBuild;
            if (y->black == false) { //if red uncle
                attrBuild += 4;
                z->parent->black = true; //solution for this case is to recolor, parent made black from red
                y->black = true; //uncle made black from red
                z->parent->parent->black = false; //grandparent must then be made red to preserve property
                z = z->parent->parent; //update iterator for while loop, in case an imbalance appears two levels higher
            } else {
                ++compBuild;
                 if (z == z->parent->right) { //both case 2 and 3 need to recolor z's parent and grandparent, but only case 2 needs a left rotation around z's parent
                     ++attrBuild;
                     z = z->parent;           //if z equals the right child of its parent, we know for sure we need to perform a rotation
                     leftRotate(z, root, true);
                 } //else it must mean that z equals the left child of its parent, either way z's parent and grandparent need recoloring and a right rotation is needed as
                   //the tree is currently too left heavy
                 attrBuild += 2;
                 z->parent->black = true;
                 z->parent->parent->black = false;
                 rightRotate(z->parent->parent, root, true);
            }
        }
        else { //if z's parent doesn't equal the left child of z's grandparent, it only means that it must equal the right child of its grandparent, so the tree became right heavy after inserting z
            ++attrBuild;
            Node* y = z->parent->parent->left; //same thinking as before but mirrored
            ++compBuild;
            if (y->black == false) { //same recoloring as before but mirrored
                attrBuild += 4;
                z->parent->black = true;
                y->black = true;
                z->parent->parent->black = false;
                z = z->parent->parent;
            } else {
                ++compBuild;
                if (z == z->parent->left) {
                    ++attrBuild;
                    z = z->parent;
                    rightRotate(z, root, true);
                }
                attrBuild += 2;
                z->parent->black = true;
                z->parent->parent->black = false;
                leftRotate(z->parent->parent, root, true);
        }
        }
    }
    ++compBuild;
    ++attrBuild;
    root->black = true;
}

Node* treeMinimum(Node* x) {
    while(x->left != TNIL) {
        ++attrDelete;
        ++compDelete;
        x = x->left;
    }
    ++compDelete;
    return x;
}

Node* treeMaximum(Node* x) {
    while (x->right != TNIL) {
        ++attrDelete;
        ++compDelete;
        x = x->right;
    }
    ++compDelete;
    return x;
}

void buildRB(int n, Node*& outputRoot) {
    for (int i = 1; i <= n; ++i) {
        Node* newN = newNode(i, false);
        insertRB(outputRoot, newN);
    }
}

void RBDelete(Node*& root, Node* z) {
    attrDelete += 3;
    Node* x = TNIL;
    Node* y = z;
    bool yOriginalColor = y->black;
    ++compDelete;
    if (z->left == TNIL) {
        ++attrDelete;
        x = z->right;
        RBTransplant(root, z, z->right);
    }
    else {
        ++compDelete;
        if (z->right == TNIL) {
            ++attrDelete;
            x = z->left;
            RBTransplant(root, z, z->left);
        }
        else {
            attrDelete += 3;
            y = treeMinimum(z->right);
            yOriginalColor = y->black;
            x = y->right;
            ++compDelete;
            if (y->parent == z) {
                ++attrDelete;
                x->parent = y;
            }
            else {
                RBTransplant(root, y, y->right);
                attrDelete += 2;
                y->right = z->right;
                y->right->parent = y;
            }
            RBTransplant(root, z, y);
            attrDelete += 3;
            y->left = z->left;
            y->left->parent = y;
            y->black = z->black;
        }
    }
    ++compDelete;
    if (yOriginalColor == true) {
        RBDeleteFixup(root, x);
    }
}

void RBDeleteFixup(Node*& root, Node* x) {
    while (x != root && x->black == true) {
        compDelete += 2;
        ++compDelete;
        if (x == x->parent->left) {
            ++attrDelete;
            Node* w = x->parent->right;
            ++compDelete;
            if (w->black == false) {
                attrDelete += 3;
                w->black = true;
                x->parent->black = false;
                leftRotate(x->parent, root, false);
                w = x->parent->right;
            }
            compDelete += 2;
            if (w->left->black == true && w->right->black == true) {
                attrDelete += 2;
                w->black = false;
                x = x->parent;
            }
            else {
                ++compDelete;
                if (w->right->black == true) {
                    attrDelete += 3;
                    w->left->black = true;
                    w->black = false;
                    rightRotate(w, root, false);
                    w = x->parent->right;
                }
                attrDelete += 3;
                w->black = x->parent->black;
                x->parent->black = true;
                w->right->black = true;
                leftRotate(x->parent, root, false);
                x = root;
            }
        }
        else {
            ++attrDelete;
            Node* w = x->parent->left;
            ++compDelete;
            if (w->black == false) {
                attrDelete += 3;
                w->black = true;
                x->parent->black = false;
                rightRotate(x->parent, root, false);
                w = x->parent->left;
            }
            compDelete += 2;
            if (w->right->black == true && w->left->black == true) {
                attrDelete += 2;
                w->black = false;
                x = x->parent;
            }
            else {
                ++compDelete;
                if (w->left->black == true) {
                    attrDelete += 3;
                    w->right->black = true;
                    w->black = false;
                    leftRotate(w, root, false);
                    w = x->parent->left;
                }
                attrDelete += 4;
                w->black = x->parent->black;
                x->parent->black = true;
                w->left->black = true;
                rightRotate(x->parent, root, false);
                x = root;
            }
        }
    }
    compDelete += 2;
    ++attrDelete;
    x->black = true;
}


Node* newNode(int key, bool black) {
    Node* newN = new Node();
    newN->left = TNIL;
    newN->right = TNIL;
    newN->size = 0;
    newN->key = key;
    newN->black = black;
    newN->parent = TNIL;
    return newN;
}

void demo() {
    Node* tree1 = TNIL;
    buildRB(11, tree1);
    getSize(tree1, false, true);
    printTree(tree1, 0, true);
    int repeat;
    int x;
    do {
        SetConsoleTextAttribute(hConsole, 14);
        cout << "OSSelect(x), x=";
        SetConsoleTextAttribute(hConsole, 7);
        cin >> x;
        cout << "The element with the xth smallest key = ";
        SetConsoleTextAttribute(hConsole, 8);
        cout << OSSelect(tree1, x)->key << "\n";
        SetConsoleTextAttribute(hConsole, 7);
        cout << "Repeat?(0/1)";
        cin >> repeat;
    } while (repeat != 0);

    do {
        SetConsoleTextAttribute(hConsole, 13);
        cout << "RBDelete(x), x=";
        SetConsoleTextAttribute(hConsole, 7);
        cin >> x;
        RBDelete(tree1, OSSelect(tree1, x));
        getSize(tree1, false, false);
        printTree(tree1, 0, true);
        cout << "Repeat?(0/1)";
        cin >> repeat;
    } while (repeat != 0);

}

void perf() {
    for (int n = STEP_SIZE; n <= N; n += STEP_SIZE) {
        for (int i = 0; i < NR_TESTS; ++i) {
            Node* tree2 = TNIL;
            resetIndexes();
            buildRB(n, tree2);
            getSize(tree2, false, true);
            int remaining = n;
            OSSelect(tree2, randomNumber(1, remaining));
            RBDelete(tree2, OSSelect(tree2, randomNumber(1, remaining)));
            getSize(tree2, false, false);
            --remaining;

            Operation compSch = p.createOperation("search-comp", n);
            Operation attrSch = p.createOperation("search-attr", n);
            Operation compDel = p.createOperation("delete-comp", n);
            Operation attrDel = p.createOperation("delete-attr", n);
            Operation compBld = p.createOperation("build-comp", n);
            Operation attrBld = p.createOperation("build-attr", n);

            compSch.count(compSearch);
            attrSch.count(attrSearch);
            compDel.count(compDelete);
            attrDel.count(attrDelete);
            compBld.count(compBuild);
            attrBld.count(attrBuild);
        }
    }
    p.divideValues("search-comp", NR_TESTS);
    p.divideValues("search-attr", NR_TESTS);
    p.divideValues("delete-comp", NR_TESTS);
    p.divideValues("delete-attr", NR_TESTS);
    p.divideValues("build-comp", NR_TESTS);
    p.divideValues("build-attr", NR_TESTS);


    p.addSeries("search-total", "search-attr", "search-comp");
    p.addSeries("delete-total", "delete-attr", "delete-comp");
    p.addSeries("build-total", "build-attr", "build-comp");

    p.createGroup("total", "search-total", "delete-total", "build-total");
    p.createGroup("total-comp", "search-comp", "delete-comp", "build-comp");
    p.createGroup("total-attr", "search-attr", "delete-attr", "build-attr");

    p.showReport();
}

int main()
{
    srand(time(NULL));
    int decision = -1;
    do
    {
        SetConsoleTextAttribute(hConsole, 14);
        cout << "Hello! Choose one option:\n";
        SetConsoleTextAttribute(hConsole, 10);
        cout << " ******************************\n";
        cout << " * 0 - correctness            *\n * 1 - performance            *\n";
        cout << " ******************************\n";
        SetConsoleTextAttribute(hConsole, 7);
        cin >> decision;
    } while (decision < 0 || decision > 1);
    switch (decision) {
        case 0: {
            demo();
            break;
        }
        case 1: {
            cout << "Please wait, this might take a while...\n";
            perf();
            break;
        }
        default: {
            cout << "Invalid decision ID\n";
            break;
        }
    }
}

