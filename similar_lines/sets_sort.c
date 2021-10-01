#include <stdlib.h>
#include <string.h>

#include "sets_sort.h"

int cmp_numbers(const void *a, const void *b)
{
    if (*(long double *) a < *(long double *) b) return -1;
    if (*(long double *) a > *(long double *) b) return 1;
    return 0;
}

static int cmp_non_numbers(const void *a, const void *b)
{
    const char **string_a = (const char **) a;
    const char **string_b = (const char **) b;
    return strcmp(*string_a, *string_b); //returns -1 if string_a < string_b, 1 if string_a > string_b, else 0
}

static void sort_non_numbers_arr(Set *set)
{
    qsort(set->non_numbers, set->how_many_non_numbers, sizeof(char *), cmp_non_numbers);
}

static void sort_numbers_arr(Set *set)
{
    qsort(set->numbers, set->how_many_numbers, sizeof(long double), cmp_numbers);
}

void sort_set(Set *set)
{
    sort_numbers_arr(set);
    sort_non_numbers_arr(set);
}



