#ifndef MYSORT_H
#define MYSORT_H


#include <stdlib.h>
#include <memory.h>


#define SMALL_THRESHOLD 16
#define swap2(x, y) ({ typeof(x) temp = (x); (x) = (y); (y) = temp; })
#define swap3(x, y, z) ({ typeof(x) temp = (x); (x) = (y); (y) = (z); (z) = temp; })


// 采用位运算的不便之处在于浮点数的处理
// 其优势在于排序个数超过范围的整数数组
void BqSort(int v[], const size_t len);
void HeapSort(int v[], const size_t len);
void MergeSort(int v[], const size_t len);
void QuickSort3Way(int v[], const size_t len);
void QuickSortMedian(int v[], const size_t len);
void QuickSortStable(int v[], const size_t len);
void RadixSort(int v[], const size_t len);
void ShellSort(int v[], const size_t len);

static __inline void insert_sort(int v[], int lo, int hi);
static __inline void binary_insert_sort(int v[], int lo, int hi);
static __inline int flp2(int n)
{
    n |= (n >> 1); n |= (n >> 2);
    n |= (n >> 4); n |= (n >> 8);
    n |= (n >> 16);
    return n - (n >> 1);
}


static __inline int bit_partition(int v[], int lo, int hi, int bit)
{
    while (lo <= hi && !(v[lo] & bit)) lo += 1;
    while (lo <= hi &&  (v[hi] & bit)) hi -= 1;
    while (lo < hi) {
        swap2(v[lo], v[hi]);
        while (!(v[++lo] & bit));
        while ( (v[--hi] & bit));
    }
    return lo;
}

// The BqSort algorithm aux function
static __inline void bqsort_recursive(int v[], int lo, int hi, int hbit)
{
    if (lo < hi && hbit > 0) {
        int j = bit_partition(v, lo, hi, hbit);
        bqsort_recursive(v, lo, j - 1, hbit >> 1);
        bqsort_recursive(v, j, hi, hbit >> 1);
    }
}
// Just for uint's sorting
void BqSort(int v[], const size_t len)
{
    int issorted = 1, bits = v[0];
    for (int i = 1; i < len; ++i) {
        bits |= v[i];
        issorted &= (v[i - 1] <= v[i]);
    }
    if (issorted) return;

    bqsort_recursive(v, 0, len - 1, flp2(bits));
}

// 或考虑不用额外的内存，直接通过对每一位的（0，1）进行计数来构建结果
// 基于二进制的基数排序
void RadixSort(int v[], const size_t len)
{
    int issorted = 1, bits = v[0];

    for (int i = 1; i < len; ++i) {
        bits |= v[i];
        issorted &= (v[i - 1] <= v[i]);
    }
    if (issorted) return;

    int lt, lv;
    int lo, hi;
    int mi = len >> 1;
    int *temp = (int *)malloc(mi * sizeof(int));

    for (int bit = 1; bit < bits; bit <<= 1) {
        if (!(bit & bits))
            continue;

        lo = lt = lv = 0;
        for (; lt < mi && lo < len; ++lo) {
            if (v[lo] & bit)
                temp[lt++] = v[lo];
            else
                v[lv++] = v[lo];
        }
        for (hi = mi; lo < len; ++lo) {
            if (v[lo] & bit)
                v[hi++] = v[lo];
            else
                v[lv++] = v[lo];
        }
        while (hi > mi)
            v[--lo] = v[--hi];

        if (lv > 0 && lt > 0)
            memcpy(&v[lv], temp, lt * sizeof(int));
    }
    free(temp);
}


static __inline void quicksortstable(int v[], int lo, int hi, int temp[])
{
    int gt = 0, eq = hi, lt = lo;
    int pv = v[(lo + hi) / 2];

    for (int i = lo; i <= hi; ++i) {
        if (v[i] < pv)
            v[lt++] = v[i];
        else if (v[i] > pv)
            temp[gt++] = v[i];
        else
            temp[eq--] = v[i];
    }
    pv = lt - 1;
    for (int i = hi; i > eq; --i)
        v[lt++] = temp[i];

    if (gt > 0) {
        memcpy(&v[lt], temp, gt * sizeof(int));
        if (hi > lt)
            quicksortstable(v, lt, hi, temp);
    }
    if (pv > lo)
        quicksortstable(v, lo, pv, temp);
}

void QuickSortStable(int v[], const size_t len)
{
    if (len > 1) {
        int *temp = (int *)malloc(len * sizeof(int));
        quicksortstable(v, 0, len - 1, temp);
        free(temp);
    }
}


// 作用： 使 v[mi] <= v[lo] <= v[hi]
// 并返回 v[lo] 作为三值取中的结果
static __inline int selectpivot(int v[], int lo, int hi)
{
    int mi = (lo + hi) >> 1;

    if (v[lo] < v[mi]) swap2(v[lo], v[mi]);

    if (v[hi] < v[lo]) {    // v[lo] >= v[mi], v[lo] > v[hi]
        if (v[mi] > v[hi])  // v[lo] >= v[mi] > v[hi]
            swap3(v[lo], v[mi], v[hi]);
        else                // v[lo] > v[hi] >= v[mi]
            swap2(v[lo], v[hi]);
    }
    return v[lo];
}

static __inline int partition(int v[], int lo, int hi)
{
    int vx = lo;
    int pivot = selectpivot(v, lo, hi);

    while (1) {
        while (v[++vx] < pivot);
        while (v[--hi] > pivot);
        if (vx >= hi) break;
        swap2(v[vx], v[hi]);
    }
    v[lo] = v[hi];
    v[hi] = pivot;

    // v[hi] == pivot
    // v[i] <= pivot for i < hi
    // v[j] >= pivot for j > hi
    return hi;
}

static __inline void quicksortmedian_recursive(int v[], int lo, int hi)
{
    while (lo < hi) {
        if (hi - lo <= SMALL_THRESHOLD) {
            binary_insert_sort(v, lo, hi);
            return;
        }
        int pivot = partition(v, lo, hi);
        if (pivot - lo >= hi - pivot) {
            quicksortmedian_recursive(v, lo, pivot - 1);
            lo = pivot + 1;
        }
        else {
            quicksortmedian_recursive(v, pivot + 1, hi);
            hi = pivot - 1;
        }
    }
}

void QuickSortMedian(int v[], const size_t len)
{
    quicksortmedian_recursive(v, 0, len - 1);
}


static void quicksort3way_recursive(int v[], int lo, int hi)
{
    if (hi <= lo) return;
    int lt = lo, i = lo + 1, gt = hi;
    int temp = v[lo];
    while (i <= gt) {
        if (v[i] < temp) {
            swap2(v[lt], v[i]);
            ++lt; ++i;
        }
        else if (v[i] > temp) {
            swap2(v[i], v[gt]);
            --gt;
        }
        else ++i;
    }
    quicksort3way_recursive(v, lo, lt - 1);
    quicksort3way_recursive(v, gt + 1, hi);
}

void QuickSort3Way(int v[], const size_t len)
{
    quicksort3way_recursive(v, 0, len - 1);
}


static __inline void merge(int v[], int lo, int mi, int hi, int temp[])
{
    if (v[mi - 1] > v[mi]) {
        int lt = mi - lo;
        int it = 0, ia = mi;
        memcpy(temp, &v[lo], lt * sizeof(int));

        while (it < lt && ia < hi) {
            if (temp[it] <= v[ia])
                v[lo++] = temp[it++];
            else
                v[lo++] = v[ia++];
        }
        // memcpy(&v[lo], &temp[it], (lt - it) * sizeof(int));
        while (it < lt)
            v[lo++] = temp[it++];
    }
}

static __inline void mergesort_recursive(int v[], int lo, int hi, int temp[])
{
    if (lo + 1 < hi) {
        int mi = (lo + hi) >> 1;
        mergesort_recursive(v, lo, mi, temp);
        mergesort_recursive(v, mi, hi, temp);
        merge(v, lo, mi, hi, temp);
    }
}

void MergeSort(int v[], const size_t len)
{
    int *temp = (int *)malloc((len >> 1) * sizeof(int));
    mergesort_recursive(v, 0, len, temp);
    free(temp);
}


static void MaxHeapify(int v[], int i, int size)
{
    int temp = v[i];
    int rchild, child = (i << 1) + 1;

    for (; child < size; child = (i << 1) + 1) {
        rchild = child + 1;
        if (rchild < size && v[rchild] > v[child])
            child = rchild;
        if (temp < v[child]) {
            v[i] = v[child];
            i = child;
        }
        else
            break;
    }
    v[i] = temp;
}

void HeapSort(int v[], const size_t len)
{
    for (int i = len / 2; i >= 0; --i)
        MaxHeapify(v, i, len);

    for (int i = len - 1; i > 0; --i) {
        swap2(v[i], v[0]);
        MaxHeapify(v, 0, i);
    }
}


static __inline int binary_insert_search(int v[], int lo, int hi, int x)
{
    while (lo <= hi) {
        int mi = (lo + hi) / 2;
        if (v[mi] <= x)
            lo = mi + 1;
        else
            hi = mi - 1;
    }
    return lo;
}

static __inline void binary_insert_sort(int v[], int lo, int hi)
{
    int i, j, p, temp;
    for (i = lo + 1; i <= hi; ++i) {
        temp = v[i];
        p = binary_insert_search(v, lo, i, temp);
        j = i;
        while (j > p) {
            v[j] = v[j - 1];
            --j;
        }
        v[p] = temp;
    }
}


static __inline void insert_sort(int v[], int lo, int hi)
{
    int i, j;
    int x;
    for (i = lo + 1; i <= hi; ++i) {
        j = i;
        x = v[j];
        while (j > lo && v[j - 1] > x) {
            v[j] = v[j - 1];
            j -= 1;
        }
        v[j] = x;
    }
}


void ShellSort(int v[], const size_t len)
{
    int i, j, k, inc;
    int temp;
    // Sedgewick 增量序列
    const int INCS[] = {
        1, 5, 19, 41, 109, 209, 505, 2161,
        8929, 36289, 146305, 260609, 587521
    };

    for (k = sizeof(INCS) / sizeof(int); --k >= 0; ) {
        inc = INCS[k];
        for (i = inc; i < len; ++i) {
            temp = v[i];
            for (j = i; j >= inc; j -= inc) {
                if (temp < v[j - inc])
                    v[j] = v[j - inc];
                else
                    break;
            }
            v[j] = temp;
        }
    }
}

#undef swap2
#undef swap3
#undef SMALL_THRESHOLD

#endif /* mysort.h */
