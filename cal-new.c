#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <pthread.h>
#include <semaphore.h>
#include "function.h"

#define M 4 // M is the number of thread 

#define Debug 0
/*
//this is the structure to hold the data 
typedef struct {
    int* arr;
    int low;
    int high;
    int size;
    int layer;
    int threadNum;
} task_data;
*/
// this functin print out an array 
void printArray(int *arr,  int index,  int size) {   
    int i = 0;

    printf("Array[%d - %d] = [ ", index, (index + size - 1));
    for(i = index; i < (index + size); i++) {
        if((i - index) != 0 && ((i - index) % 10) == 0)
            printf("\n"); // also follow the format of print 10 numbers
        printf ("%d ", arr[i]);
    }
    
    printf("]\n");
}

// This function print result 
// will start a new line every 10 numbers
// index is the first item of the array
// size refer to the array size 

void printResult(int *arr,  int index,  int size) {
    int i = 0;
    for(i = index; i < (index + size); i++) {       
        if((i - index) != 0 && ((i - index) % 10) == 0)
            printf("\n");

        printf ("%d ", arr[i]);
    }
     printf("\n");
}


// this function Merge two sorted subarrays to one 
// low is the first index
// high is the last index

int merge(int *arr,  int low, int high) {
    if(Debug) {
        printf("[Data](file:%s funct:%s): Before merge - ", __FILE__, __FUNCTION__);
        printArray(arr, low, (high - low + 1));
    }

    int mid = low + (high - low) / 2;
    int i = low;
    int j = mid + 1;
    int k = 0;

    // create a new array, and intial to 0
    int tmp[high - low + 1];
    int n;
    memset(tmp, 0, sizeof(tmp));

    while(i <= mid && j <= high) {
        if(arr[i] <= arr[j]) {
            tmp[k] = arr[i];
            i++;
            k++;
        } else {
            tmp[k] = arr[j];
            j++;
            k++;
        }
    }

    //Copy the remaining elements to tmp, if there are any
    while(i <= mid) {
        tmp[k] = arr[i];
        i++;
        k++;
    }

    while(j <= high) {
        tmp[k] = arr[j];
        j++;
        k++;
    }

    for(i = low, k = 0; i <= high; i++, k++) {
        arr[i] = tmp[k];
    }

    if(Debug) {
        printf("[Data](file:%s funct:%s): After merge - ", __FILE__, __FUNCTION__);
        printArray(arr, low, (high - low + 1));
    }
}

// this function use recursion to sort an array
int sort(int* arr, int low, int high) {
 
    if(Debug) {
        printf("[Data](file:%s funct:%s): Before sort - ", __FILE__, __FUNCTION__);
        printArray(arr, low, (high - low + 1));
    }

    if(low < high) {
        int mid = low + (high - low) / 2;

        sort(arr, low, mid);
        sort(arr, mid + 1, high);
        merge(arr, low, high);
    }
}

pthread_mutex_t slock;
//sort lock for thread sort. 
// use mutex for mutual exclusion , so 1 thread use this function at a time
// lock used for mergeSort_thread


// this function is the merge sort function for thread to use
void* mergeSort_thread(void* args) {
    task_data* sorting_args = (task_data*) args;
    
    pthread_mutex_lock(&slock);
    
    if(Debug) {
        printf("[Data](file:%s funct:%s):- pthread num =  %d.\n", __FILE__, __FUNCTION__, pthread_self());
    }

    sort(sorting_args->arr, sorting_args->low, sorting_args->high);

    pthread_mutex_unlock(&slock);

    return NULL;
}


pthread_mutex_t mlock;
// use mutex for mutual exclusion , so 1 thread use this function at a time
// lock used for merge_thread


//this function is the merge function for thread to use
void* merge_thread(void* args) {
    task_data* merging_args = (task_data*) args;

    pthread_mutex_lock(&mlock);

    if(Debug) {
        printf("[Data](file:%s funct:%s):- pthread num =  %d.\n", __FILE__, __FUNCTION__, pthread_self());
    }

    merge(merging_args->arr, merging_args->low, merging_args->high);

    pthread_mutex_unlock(&mlock);
    
    return NULL;
}



//2nd layer . devide the array into 4 parts and sort each 4 parts independent. 
void* sorter(void* args) {

    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * M);

    int i;

    task_data* L2_sorting = (task_data*) args;
    task_data* sort_args = (task_data*)malloc(sizeof(task_data) * M);

    int n = L2_sorting->size / M;
    
    for(i = 0; i < M; i++) {
        sort_args[i].arr = L2_sorting->arr;
        sort_args[i].low = i * n;
        sort_args[i].high = (i + 1) * n - 1;

        pthread_create(&threads[i], NULL, mergeSort_thread, &sort_args[i]);
    }

    for(i = 0; i < M; i++) {
        pthread_join(threads[i], NULL);
    }

    free(sort_args);
    free(threads);  // free the threads
    if(Debug)
        printf("[Data](file:%s funct:%s): free sort_args & threads\n", __FILE__, __FUNCTION__);

    printf("==================== L=%d\n", L2_sorting->layer);
    printResult(L2_sorting->arr, 0, L2_sorting->size);
    
    if(Debug) 
        printArray(L2_sorting->arr, 0, L2_sorting->size);

    return NULL;
}



// this is Layer 3 merger function , it will create different threads to 
//merge the data of the previous layer
// this function will be call 2 times , since the last 2 layers are mergers. 
 
void* merger(void* args) {
    task_data* mergingLayer = (task_data*) args;

   int layer = mergingLayer->layer;
   int pthreadNum = M / pow(2, (layer - 2));
   int n = mergingLayer->size / pthreadNum;

    pthread_t* threads = (pthread_t*)malloc(sizeof(pthread_t) * pthreadNum);
    task_data* merge_args = (task_data*)malloc(sizeof(task_data) * pthreadNum);

    // if(Debug) {
    /*if(1) {
         printf("[Data](file:%s funct:%s):- Layer = %d, #pthreads =  %d.\n", __FILE__, __FUNCTION__, layer, pthreadNum);
    } */

    if(1) {
         printf("Merge Layer = %d,  number of threads =  %d.\n", layer, pthreadNum);
    }

    // task_data merge[pthreadNum];
    int i = 0;
    for(i = 0; i < pthreadNum; i++) {
        merge_args[i].arr = mergingLayer->arr;
        merge_args[i].low = i * n;
        merge_args[i].high = (i + 1) * n - 1;

        pthread_create(&threads[i], NULL, merge_thread, &merge_args[i]);
    }

    for(i = 0; i < pthreadNum; i++) {
        pthread_join(threads[i], NULL);
    }

    free(merge_args);
    free(threads);
    if(Debug)
        printf("[Data](file:%s funct:%s): free merge_args & threads\n", __FILE__, __FUNCTION__);

    printf("==================== L=%d\n", mergingLayer->layer);
    printResult(mergingLayer->arr, 0, mergingLayer->size);
    
    if(Debug)
        printArray(mergingLayer->arr, 0, mergingLayer->size);

    return NULL;
}


/*int main()
{
    // input data, #element = 20
    // int array_test[16] = {1, 41, 33, 12, 1, 8, 9, 28, 47, 66, 35, 44, 53, 12, 21, 60};
    int array_test[16] = {18, 32, 30, 23, 31, 58, 67, 35, 58, 11, 96, 90, 33, 85, 39, 12};

    printArray(array_test, 0, 16);

    task_data arrTest;
    arrTest.arr = &array_test[0];
    arrTest.low = 0;
    arrTest.high = 15;
    arrTest.size = 16;
    arrTest.layer = 2;

    sorter(&arrTest);

    int l = log(M)/log(2);// change base to get layer = log of M on base of 2
    
    int i = 0;
    for(i = 0; i < l; i++) {
        arrTest.layer = i + 3;
        merger(&arrTest);
    }
    return 0;
}*/

