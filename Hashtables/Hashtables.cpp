using namespace std;
#include <iostream>
#include <cstdlib>
#include <string.h>
#include "Profiler.h"

/* CONCLUSIONS / OBSERVATIONS / INTERPRETATIONS
    
    The following trend can be observed:
    The closer fill factor gets to 1, the slower the performance of the operations. Also, by deleting just a few elements, 
    performance improves by simply not being so close to a fill factor of 1. 
    By choosing the inputs uniformly, performance also tends to improve compared to when inputs are chosen non-uniformly.

    In many practical cases, this behaviour is the exact reason hash tables are kept below a fill factor of 0.8, and whenever one gets above it,
    a new, bigger table is created where all old elements are copied into, thus avoiding a fill factor greater thab 0.8. There are many ways to 
    optimize a hash table, in the case of open addressing performance being at its best when fill factor is low, elements are uniformly distributed and
    size of the table is a prime number(to reduce chances of modulo operator from hash function creating extra steps in navigation). The last statement
    about size being a prime number only makes sense for certain probing methods, mainly quadratic probing, as linear probing isn't affected by that.
*/

Profiler p("HashTables");

#define NR_TESTS 5
#define N 10007
#define M 3000

double totalEffortFound, totalEffortNotFound, avgMaxEffortOneSearchFound, avgMaxEffortOneSearchNotFound, maxEffortOneSearchFound, maxEffortOneSearchNotFound, currEffort;
double fillFactor, nrElements;
double fillFactorValues[] = {0.2, 0.85, 0.9, 0.95, 0.99};
int fillFactorValuesSize = sizeof(fillFactorValues) / sizeof(fillFactorValues[0]);

class Entry {
public:
    int id;
    char name[30];
};

//Prototypes
int randomNumber(int low, int high);
int insert(Entry table[], int size, int element);
void resetIndexes();
void setValuesToUninitialized(Entry table[], int size);
void printArr(int arr[], int n);
void printTable(Entry table[], int size);
int search(Entry table[], int size, int key, boolean logToConsole);
void demo();
void deleteEntry(Entry table[], int size, int key, boolean logToConsole);
int hashf(int k, int i, int size);
void perform0(boolean showMessage);
void perform1(boolean showMessage);
void perform2(boolean showMessage);
void printTableHeader();
void printTableRow(double fillFactor, double avgEfFound, double maxEfFound, double avgEfNotFound, double maxEfNotFound);
void buildAnalysisTable(int nrRows, double par1[], double par2[], double par3[], double par4[]);
void buildAnalysisTable(double fillFactor, double par1, double par2, double par3, double par4);


int randomNumber(int low, int high) {
    return(rand() % (high - low + 1) + low);
}

int hashf(int k, int i, int size) {
    return(k + i + i * i) % size;
}

int insert(Entry table[], int size, int key) {
    int i = 0;
    do {
        int j = hashf(key, i, size);
        if (strcmp(table[j].name, "null") == 0 || strcmp(table[j].name, "deleted") == 0) {
            table[j].id = key;
            strcpy_s(table[j].name, "initialized");
            fillFactor += 1.0 / size;
            ++nrElements;
            return j;
        }
        else {
            ++i;
        }
    } while (i < size);
    cout << "Hash table overflow at key = " << key << "\n";
}

void resetIndexes() {
    currEffort = 0;
    totalEffortFound = 0;
    totalEffortNotFound = 0;
    avgMaxEffortOneSearchFound = 0;
    avgMaxEffortOneSearchNotFound = 0;
    maxEffortOneSearchFound = 0;
    maxEffortOneSearchNotFound = 0;
}

void setValuesToUninitialized(Entry table[], int size) {
    for (int i = 0; i < size; ++i) {
        strcpy_s(table[i].name, "null");
        table[i].id = 0;
    }
    fillFactor = 0;
    nrElements = 0;
}

void printArr(int arr[], int n) {
    for (int i = 0; i < n; ++i) {
        cout << arr[i] << " ";
    }
    cout << "\n";
}

void printTable(Entry table[], int size) {
    cout << "This is the printed hash table with fill factor = " << fillFactor << "\n";
    for (int i = 0; i < size; ++i) {
        if (strcmp(table[i].name, "null")) {
            cout << i << ": " << table[i].id << " " << table[i].name << "\n";
        }
        else {
            cout << i << ":  " << table[i].name << "\n";
        }
    }
    cout << "\n";
}

int search(Entry table[], int size, int key, boolean logToConsole) {
    int i = 0, j = 0;
    currEffort = 0;
    do {
        j = hashf(key, i, size);
        if (j < 0) {
            return -1;
        }
        ++currEffort;
        if (table[j].id == key) {
            if (strcmp(table[j].name, "initialized") == 0) {
                if(logToConsole)
                    cout << "Found key = " << key << " at index " << j << "\n";
                if (currEffort > maxEffortOneSearchFound) {
                    maxEffortOneSearchFound = currEffort;
                }
                totalEffortFound += currEffort;
                return j;
            }
        }
        ++i;
    } while(strcmp(table[j].name, "null") != 0 && i < size); //strcmp(table[j].name, "deleted") != 0 &&
    if(logToConsole)
        cout << "Key = " << key << " not found\n";
    totalEffortNotFound += currEffort;
    if (currEffort > maxEffortOneSearchNotFound) {
        maxEffortOneSearchNotFound = currEffort;
    }
}

void deleteEntry(Entry table[], int size, int key, boolean logToConsole) {
    int i = 0, j;
    do {
        j = hashf(key, i, size);
        if (j < 0) {
            return;
        }
        if (table[j].id == key) {
            if(logToConsole)
                cout << "Deleted key = " << key << " at index " << j << "\n";
            table[j].id = 0;
            strcpy_s(table[j].name, "deleted");
            fillFactor -= 1.0 / size;
            --nrElements;
            return;
        }
        ++i;
    } while (strcmp(table[j].name, "null") != 0 && i < size);
    if(logToConsole)
        cout << "Key = " << key << " not found\n";
}

void printTableHeader() {
    cout << "_________________________________________________________________________________\n";
    cout << "|  Filling  | Avg. Effort     | Max. Effort    | Avg. Effort    | Max. Effort   |\n";
    cout << "|  factor   | found           | found          | not-found      | not-found     |\n";
    cout << "_________________________________________________________________________________\n";
}

void printTableRow(double fillFactor, double avgEfFound, double maxEfFound, double avgEfNotFound, double maxEfNotFound) {
    printf_s("|  %-7.2lf  |     %-7.2lf     |     %-7.2lf    |     %-7.2lf    |     %-7.2lf   |\n", fillFactor, avgEfFound, maxEfFound, avgEfNotFound, maxEfNotFound);
    cout << "_________________________________________________________________________________\n";
}

void buildAnalysisTable(int nrRows, double par1[], double par2[], double par3[], double par4[]) {
    printTableHeader();
    for (int i = 0; i < nrRows; ++i) {
        printTableRow(fillFactorValues[i], par1[i], par2[i], par3[i], par4[i]);
    }
}

void buildAnalysisTable(double fillFactor, double par1, double par2, double par3, double par4) {
    printTableHeader();
    printTableRow(fillFactor, par1, par2, par3, par4);
}

void demo() {
    Entry table1[11];
    int size1 = 11;
    setValuesToUninitialized(table1, size1);
    cout << "Table before inserting anything:\n";
    printTable(table1, size1);
    int inputs[] = { 0, 5, 6, 1, 17, 5, 4, 1576, 91 };
    int sizeInputArr = sizeof(inputs) / sizeof(inputs[0]);
    cout << "Will insert the following into the table:\n";
    printArr(inputs, sizeInputArr);
    cout << "\n";
    for (int i = 0; i < sizeInputArr; ++i) {
        insert(table1, size1, inputs[i]);
    }
    printTable(table1, size1);
    cout << "Which element(s) do you want to search?\n";
    boolean keepGoing = true;
    int nr, decision;
    while (keepGoing) {
        cout << "nr = ";
        cin >> nr;
        search(table1, size1, nr, true);
        cout << "Repeat?(0/1)\n";
        cin >> decision;
        if (decision == 0) {
            keepGoing = false;
        }
    }
    cout << "Which element(s) do you want to delete?\n";
    keepGoing = true;
    while (keepGoing) {
        cout << "nr = ";
        cin >> nr;
        deleteEntry(table1, size1, nr, true);
        cout << "Repeat?(0/1)\n";
        cin >> decision;
        if (decision == 0) {
            keepGoing = false;
        }
    }
    cout << "Which element(s) do you want to search?\n";
    keepGoing = true;
    while (keepGoing) {
        cout << "nr = ";
        cin >> nr;
        search(table1, size1, nr, true);
        cout << "Repeat?(0/1)\n";
        cin >> decision;
        if (decision == 0) {
            keepGoing = false;
        }
    }
    cout << "\nThe table after all operations:\n";
    printTable(table1, size1);
}

void perform0(boolean showMessage) {
    double par2, par4;
    double par1, par3;
    Entry table2[N];
    int size2 = N;
    resetIndexes();
    for (int i = 0; i < NR_TESTS; ++i) {
            setValuesToUninitialized(table2, size2);
            int* v = new int[M / 2];
            int k = 0;
            while (fillFactor < fillFactorValues[0]) {
                int temp = randomNumber(0, 3000);
                insert(table2, size2, temp);
                if (k < M / 2) {
                    v[k] = temp;
                    ++k;
                }
            }
            //searching elements which we know will not be found
            for (int k = 0; k < M / 2; ++k) {
                search(table2, size2, randomNumber(3001, 9000), showMessage);
            }
            //searching elements we know will be found
            for (int k = 0; k < M / 2; ++k) {
                search(table2, size2, v[k], showMessage);
            }
            avgMaxEffortOneSearchFound += maxEffortOneSearchFound;
            avgMaxEffortOneSearchNotFound += maxEffortOneSearchNotFound;
            maxEffortOneSearchFound = 0;
            maxEffortOneSearchNotFound = 0;
    }
    totalEffortFound /= NR_TESTS;
    totalEffortNotFound /= NR_TESTS;
    avgMaxEffortOneSearchFound /= NR_TESTS;
    avgMaxEffortOneSearchNotFound /= NR_TESTS;

    par1 = (1.0 * totalEffortFound) / (M / 2);
    par2 = avgMaxEffortOneSearchFound;
    par3 = (1.0 * totalEffortNotFound) / (M / 2);
    par4 = avgMaxEffortOneSearchNotFound;

    cout << "\nNon-uniform selection of elements for the search function:\n";
    buildAnalysisTable(fillFactorValues[0], par1, par2, par3, par4);
}

void perform1(boolean showMessage) {
    double* par1 = new double[fillFactorValuesSize];
    double* par2 = new double[fillFactorValuesSize];
    double* par3 = new double[fillFactorValuesSize];
    double* par4 = new double[fillFactorValuesSize];
    Entry table3[N];
    int size3 = N;
    resetIndexes();

    for (int j = 0; j < fillFactorValuesSize; ++j) {
        for (int i = 0; i < NR_TESTS; ++i) {
            setValuesToUninitialized(table3, size3);
            int* v = new int[M / 2];
            int k = 0, mod = 0;
            while (fillFactor < fillFactorValues[j]) {
                int temp = randomNumber(0, 100000);
                insert(table3, size3, temp);
                if (k < M / 2 && mod % randomNumber(1, 5) == 0) {
                    v[k] = temp;
                    ++k;
                    mod = 0;
                }
                ++mod;
            }
            //searching elements which we know will not be found
            for (int l = 0; l < M / 2; ++l) {
                search(table3, size3, randomNumber(101001, 102000), showMessage);
            }
            //uniformly searching elements which we know will be found
            for (int m = 0; m < M / 2; ++m) {
                int uni = randomNumber(0, (M / 2) - 1);
                search(table3, size3, v[uni], showMessage);
            }
            avgMaxEffortOneSearchFound += maxEffortOneSearchFound;
            avgMaxEffortOneSearchNotFound += maxEffortOneSearchNotFound;

            maxEffortOneSearchFound = 0;
            maxEffortOneSearchNotFound = 0;
        }
        totalEffortFound /= 1.0 * NR_TESTS;
        totalEffortNotFound /=  1.0 * NR_TESTS; 
        avgMaxEffortOneSearchFound /= 1.0 * NR_TESTS;
        avgMaxEffortOneSearchNotFound /= 1.0 * NR_TESTS;
        par1[j] = (1.0 * totalEffortFound) / (M / 2);
        par2[j] = avgMaxEffortOneSearchFound;
        par3[j] = (1.0 * totalEffortNotFound) / (M / 2);
        par4[j] = avgMaxEffortOneSearchNotFound;
        resetIndexes();
    }
    cout << "\nUniform selection of elements for the search function:\n";
    buildAnalysisTable(fillFactorValuesSize, par1, par2, par3, par4);
    perform2(showMessage);
}

void perform2(boolean showMessage) {
    double par1;
    double par2;
    double par3;
    double par4;
    Entry table4[N];
    int size4 = N;
    int v[10007];
    resetIndexes();


        for (int i = 0; i < NR_TESTS; ++i) {
            int* v = new int[2 * M];
            setValuesToUninitialized(table4, size4);
            int j = 0;
            while (fillFactor < 0.99) {
                int temp = randomNumber(0, 100000);
                insert(table4, size4, temp);
                if (j < 2*M) {
                    v[j] = temp;
                    ++j;
                }
            }
            int m = 2 * M;
            while (fillFactor > 0.8) {
                deleteEntry(table4, size4, v[m], showMessage);
                --m;
            }
            //not found
            for (int k = 0; k < M / 2; ++k) {
                search(table4, size4, randomNumber(101001, 102000), showMessage);
            }
            //found
            for (int k = 0; k < M / 2 + M / 5; ++k) {
                search(table4, size4, v[k], showMessage);
            }
            avgMaxEffortOneSearchFound += maxEffortOneSearchFound;
            avgMaxEffortOneSearchNotFound += maxEffortOneSearchNotFound;


            maxEffortOneSearchFound = 0;
            maxEffortOneSearchNotFound = 0;
        }
        totalEffortFound /= 1.0 * NR_TESTS;
        totalEffortNotFound /= 1.0 * NR_TESTS;
        avgMaxEffortOneSearchFound /= 1.0 * NR_TESTS;
        avgMaxEffortOneSearchNotFound /= 1.0 * NR_TESTS;
        par1 = (1.0 * totalEffortFound) / (M / 2);
        par2 = avgMaxEffortOneSearchFound;
        par3 = (1.0 * totalEffortNotFound) / (M / 2);
        par4 = avgMaxEffortOneSearchNotFound;
        resetIndexes();
    cout << "\nSearch operation after deletion of some elements:\n";
    buildAnalysisTable(0.8, par1, par2, par3, par4);
}

int main()
{
    srand(time(NULL));
    int choice = -1;
    cout << "Choose an option:\n";
    cout << "0 - Demo of hash table operations\n";
    cout << "1 - Non-uniform, single fill factor search operation\n";
    cout << "2 - Uniform, multiple fill factors search operation\n";
    cout << "3 - After deletion of some elements search operation\n";
    do {
        cout << "choice = ";
        cin >> choice;
    } while (choice < 0 || choice > 3);
        boolean printMessages;
    if (choice != 0) {
        int printMessagesDecision;
        cout << "Show couts from search and delete?(0/1)\n";
        cin >> printMessagesDecision;
        if (printMessagesDecision == 0) {
            printMessages = false;
        }
        else {
            printMessages = true;
        }
    }
    else {
        printMessages = true;
    }
    switch (choice) {
        case 0: {
            demo();
            break;
        }
        case 1: {
            perform0(printMessages);
            break;
        }
        case 2: {
            perform1(printMessages);
            break;
        }
        case 3: {
            perform2(printMessages);
            break;
        }
        default: {
            cout << "Invalid choice ID\n";
            break;
        }
    }
}

