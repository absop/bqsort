/////////////////////////
// running with Tinycc //
/////////////////////////
#include <stdio.h>
#include <memory.h>
#include "print.h"

#define swap(a, b) ({ typeof(a) tmp = a; a = b; b = tmp; })
#define cmpSwap(i, j) \
    if (A[i] > A[j]) swap(A[i], A[j])


void sort4e5c(int *A)
{
    cmpSwap(0, 1);
    cmpSwap(2, 3);
    cmpSwap(0, 2);
    cmpSwap(1, 3);
    cmpSwap(1, 2);
}

#define insert_A4_into_A012()       \
{                                   \
    int pos;                        \
    if (A[4] < A[1]) {              \
        if (A[4] < A[0])            \
            pos = 0;                \
        else                        \
            pos = 1;                \
    }                               \
    else {                          \
        if (A[4] < A[2])            \
            pos = 2;                \
        else                        \
            pos = 3;                \
                                    \
        }                           \
                                    \
        int a4 = A[4];              \
    for (int i = 4; i > pos; --i)   \
        A[i] = A[i - 1];            \
    A[pos] = a4;                    \
}

// Sort 5 elements with only 7 comparisons
void sort5e7c(int *A)
{
    cmpSwap(0, 1);
    cmpSwap(2, 3);

    int temp;
    if (A[1] > A[3]) {
        temp = A[0];
        A[0] = A[2];
        A[2] = A[1];
        A[1] = A[3];
        A[3] = temp;
    }
    else
        swap(A[2], A[3]);
    // insert A[4] into A[0, 1, 2], so that A[3] -> A[4]
    insert_A4_into_A012();
    // insert A[4] into A[0, 1, 2]
    insert_A4_into_A012();

}


int issorted(int A[], int len)
{
    for (int i = 0; i < len - 1; ++i)
        if (A[i] > A[i + 1])
            return 0;
    return 1;
}


void full_permutation(int *A, int *B, void sort(int *), int begin, int end, int p_size)
{
    if (begin >= p_size) {
        memcpy(B, A, p_size * sizeof(int));
        print(B, p_size, p_size);
        sort(B);
        if (!issorted(B, p_size)) {
            printf("Fault order:\t");
            print(B, p_size, p_size);
        }
    }
    else {
        for (int i = begin; i <= end; ++i) {
            swap(A[begin], A[i]);
            full_permutation(A, B, sort, begin + 1, end, p_size);
            swap(A[begin], A[i]);
        }
    }
}

void test_sort_algo(void sort(int *), int len)
{
    int A[len], B[len];
    for (int i = 0; i < len; ++i)
        A[i] = i + 1;
    full_permutation(A, B, sort, 0, len - 1, len);
}


int main()
{
    test_sort_algo(sort4e5c, 4);
    test_sort_algo(sort5e7c, 5);
    return 0;
}
