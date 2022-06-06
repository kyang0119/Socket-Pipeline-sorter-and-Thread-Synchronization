

void printArray(int *arr,  int index,  int size);

//typedef struct task_data;
typedef struct {
    int* arr;
    int low;
    int high;
    int size;
    int layer;
    int threadNum;
} task_data;

// this header file include global functions shared across files. 
/*
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>
#include "function.h"

void printArray(int *arr,  int index,  int size);

typedef struct {
    int* arr;
    int low;
    int high;
    int size;
    int layer;
    int threadNum;
} task_data;
*/

void printResult(int *arr,  int index,  int size);

int merge(int *arr,  int low, int high);
int sort(int* arr, int low, int high);

void* mergeSort_thread(void* args);
void* merge_thread(void* args);

void* sorter(void* args);
void* merger(void* args);

//void merge_sort(int arr[], unsigned int first, unsigned int last);

//void merge(SortInfo *sortInfos,size_t threadNum);*/