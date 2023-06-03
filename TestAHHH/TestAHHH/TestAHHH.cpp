#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <cmath>

void print_array(int v[], int size_array);
void maxheapify(int array[], int poz, int a_size, int heap_size);
void maxheap(int arr[], int fst, int lst, int heap_size);
void heap_sort(int array[], int a_size, int heap_size);
void demo();


void print_array(int v[], int size_array)
{
	for (int i = 0; i < size_array; i++)
	{
		printf("%d, ", v[i]);
	}
	printf("\n");
}

void maxheapify(int array[], int poz, int a_size, int heap_size)
{
	int max = poz, left = poz * 2, right = poz * 2 + 1, aux;
	if (left <= heap_size && array[left] > array[poz])
		max = left;
	else
		max = poz;

	if (right <= heap_size && array[right] > array[max])
		max = right;

	if (max != poz)
	{
		aux = array[poz];
		array[poz] = array[max];
		array[max] = aux;
		maxheapify(array, max, a_size, heap_size);
	}
}

void maxheap(int arr[], int a_size, int heap_size) //bottomUp
{
	heap_size = a_size;
	for (int i = floor(a_size / 2); i >= 0; i--)
	{
		maxheapify(arr, a_size, i, heap_size);
	}
}

void heap_sort(int array[], int a_size, int heap_size)
{
	maxheap(array, a_size, heap_size);

	int aux;
	for (int i = a_size - 1; i >= 1; i--)
	{
		aux = array[0];
		array[0] = array[i];
		array[i] = aux;

		heap_size--;
		maxheapify(array, 0, a_size, heap_size);
	}
}

void demo()
{
	int v[] = { 7,2,8,5,9,1,6, -5, 100, 0, 143 };
	int n = sizeof(v) / sizeof(int);
	print_array(v, n);
	printf("\n");
	maxheap(v, n, n);
	//heap_sort(v, n ,n);
	print_array(v, n);
}

int main()
{
	demo();
	return 0;
}