#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define SORT_NAME int32
#define SORT_TYPE int32_t
#define SORT_CMP(x, y) ((x) - (y))
#include "sort.h"

#include "mysort.h"
#include "../print.h"


typedef void (*Sort)(int v[], const size_t len);
typedef struct _sort_algo SortAlgo;
typedef void (*Test)(SortAlgo algo, int v[], const size_t len, int times);
struct _sort_algo {
    const int tmode;
    const char *fname;
    Sort sort;
};

void random_array(int v[], const size_t len);
void ordered_array(int v[], const size_t len);
void reversed_array(int v[], const size_t len);
void allsame_array(int v[], const size_t len);
void odd_even_array(int v[], const size_t len);
void updown_array(int v[], const size_t len);
void test_correctness(int v[], const size_t len, int times);
void test_efficiency(int v[], const size_t len, int times);

Sort init_array_funcs[] = {
    random_array, ordered_array, reversed_array,
    allsame_array, odd_even_array, updown_array
};
// gcc -O2/3 will yeild a little bug here.
const char *test_title_bar = "Test %s: elements number: %d, running times: %d\n";
const char *test_mode_name[] = {
    "RANDOM", "ORDERED", "REVERSED", "ALLSAME", "ODD-EVEN", "UP-DOWN", NULL
};

// The tmode is used to decide not test the worst case.
SortAlgo sort_algorithms[] = {
    {077, "BqSort",           BqSort},
    {077, "RadixSort",        RadixSort},
    {077, "MergeSort",        MergeSort},
    {077, "QuickSortMedian",  QuickSortMedian},
    {074, "QuickSortStable",  QuickSortStable},
    {044, "QuickSort3Way",    QuickSort3Way},
    {077, "HeapSort",         HeapSort},
    {077, "ShellSort",        ShellSort},
    {000, "newline_here",     NULL},
    {077, "int32_grail_sort", int32_grail_sort},
    {077, "int32_heap_sort",  int32_heap_sort},
    {077, "int32_merge_sort", int32_merge_sort},
    {074, "int32_quick_sort", int32_quick_sort},
    {077, "int32_shell_sort", int32_shell_sort},
    {077, "int32_sqrt_sort",  int32_sqrt_sort},
    {077, "int32_tim_sort",   int32_tim_sort},
    {077, "int32_merge_sort_in_place", int32_merge_sort_in_place}
};


#define WRITE_FILE 1


int main()
{

#if 1
    double cost;
    clock_t start, finish;
    int *v = (int *)malloc(sizeof(int) * 1000000);
    start = clock();

#if WRITE_FILE
    freopen("test_result.txt", "w+", stdout);
#endif
    test_correctness(v, 1000, 100);

#if 1
    test_efficiency(v, 10000, 20);
    test_efficiency(v, 100000, 20);
    test_efficiency(v, 1000000, 20);
#if 0
    test_efficiency(v, 10000000, 10);
    test_efficiency(v, 100000000, 10);
#endif
#endif

    finish = clock();
    cost = (finish - start) / 1000.0;
    printf("Totally elapsed time: %.2lfs\n", cost);

#if WRITE_FILE
    fclose(stdout);
#endif

    free(v);

#endif

    return 0;
}


// rand() 产生的整数范围不够
void random_array(int v[], const size_t len)
{
    srand(time(NULL));
    for (int i = 0; i < len; ++i)
        v[i] = rand();
}

void ordered_array(int v[], const size_t len)
{
    for (int i = 0; i < len; ++i)
        v[i] = i;
}

void reversed_array(int v[], const size_t len)
{
    for (int i = 0; i < len; ++i)
        v[i] = len - i;
}

void allsame_array(int v[], const size_t len)
{
    for (int i = 0; i < len; ++i)
        v[i] = len;
}


void odd_even_array(int v[], const size_t len)
{
    for (int i = 0; i < len; i += 2)
        v[i] = i;
    for (int i = 1; i < len; i += 2)
        v[i] = len - i;
}

void updown_array(int v[], const size_t len)
{
    for (int i = 0; i < len / 2; ++i)
        v[i] = i;
    for (int i = len / 2; i < len; ++i)
        v[i] = len - i;
}


int isordered(int v[], int len)
{
    while (--len > 0 && v[len] >= v[len - 1]);
    return len;
}

void correctness(SortAlgo algo, int v[], const size_t len, int times)
{
    printf("%25s:    ", algo.fname);

    for (int i = 0; i < times; ++i) {
        random_array(v, len);
        algo.sort(v, len);

        if (isordered(v, len) != 0) {
            print(v, len, 10);
            printf("un");
            break;
        }
    }
    printf("ordered!\n");
}


void efficiency(SortAlgo algo, int v[], const size_t len, int times)
{
    printf("%25s", algo.fname);

    clock_t start, finish;
    double average, totol;
    int n = sizeof(init_array_funcs) / sizeof(init_array_funcs[0]);
    for (int mode = 0, mode_bit = 1 << n; mode < n; ++mode) {
        mode_bit >>= 1;
        if (!(algo.tmode & mode_bit)) {
            printf("%15c", 32);
            continue;
        }

        for (int i = totol = 0; i < times; ++i) {
            init_array_funcs[mode](v, len);
            start = clock();
            algo.sort(v, len);
            finish = clock();
            totol += finish - start;

            if (isordered(v, len) != 0) {
                printf("Wrong sort!\n");
                exit(0);
            }
        }
        average = totol / times;
        printf(" %12.3lfms", average);

    }
    printf("\n");
}


void test_sort_all(Test test, int v[], const size_t len, int times)
{
    int num = sizeof(sort_algorithms) / sizeof(SortAlgo);
    for (int i = 0; i < num; ++i) {
        SortAlgo algo = sort_algorithms[i];
        if (algo.tmode)
            test(algo, v, len, times);
        else
            printf("\n");
    }
    printf("\n\n");
}


void test_correctness(int v[], const size_t len, int times)
{
    printf(test_title_bar, "correctness", len, times);
    test_sort_all(correctness, v, len, times);
}

void test_efficiency(int v[], const size_t len, int times)
{
    printf(test_title_bar, "efficiency", len, times);
    printf("%25s", "function");
    for (const char **t = test_mode_name; *t != NULL; ++t)
        printf("%15s", *t);
    printf("\n");
    test_sort_all(efficiency, v, len, times);
}
