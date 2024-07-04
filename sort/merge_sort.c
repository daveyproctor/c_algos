#include <stdio.h>

#define SIZE (1 << 5)

/* mergeSort
*  sort an array between from_idx and to_idx
*  extra space required is linear (tmp_arr) rather than n log n.
*/
void mergeSort(int *arr, int from_idx, int to_idx, int *tmp_arr){
    // for (int i = from_idx; i <= to_idx; i++){
    //     printf("%d ", arr[i]);
    // }
    // printf("\n");

    // base case: arrays of length 1 are already trivially sorted.
    if (from_idx == to_idx) return;

    int midpoint = (from_idx + to_idx) / 2;
    // mergeSort the left half
    mergeSort(arr, from_idx, midpoint, tmp_arr);
    // mergeSort the right half
    mergeSort(arr, midpoint + 1, to_idx, tmp_arr);

    // merge left and right halves into tmp_arr
    for (int i = from_idx, j = midpoint+1, k = from_idx; i <= midpoint || j <= to_idx; k++){
        if (j > to_idx || arr[i] <= arr[j]){
            // take from LHS
            tmp_arr[k] = arr[i];
            i++;
        }
        else if (i > midpoint || arr[j] < arr[i]){
            // take from RHS
            tmp_arr[k] = arr[j];
            j++;
        }
    }

    // copy back tmp_arr into arr
    for (int k = from_idx; k <= to_idx; k++){
        arr[k] = tmp_arr[k];
    }

    return;
}

int main(void){

    // make a reverse list for testing
    int arr[SIZE] = {0};
    for (int i = 0; i < SIZE; i++){
        arr[i] = SIZE - i;
    }
    
    // print the list
    for (int i = 0; i < SIZE; i++){
        printf("%d ", arr[i]);
    }
    printf("\n");

    int to_idx = SIZE-1;
    int tmp_arr[SIZE] = {0};
    mergeSort(arr, 0, to_idx, tmp_arr);

    printf("Final: ");
    for (int i = 0; i <= to_idx; i++){
        printf("%d ", arr[i]);
    }
    printf("\n");
    
    return 0;
}
