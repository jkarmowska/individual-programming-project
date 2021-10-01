#include <stdio.h>
#include "output.h"

void print_the_result(Results result)
{
    for (size_t i = 0; i < result.how_many_different; i++)
    {
        //number of similar indexes in result.different_sets_arr[i] - how many indexes will be printed in line i of output
        size_t how_many_in_line = result.different_sets_arr[i].how_many_similar;

        for (size_t j = 0; j < how_many_in_line; j++)
        {
            printf("%zu", result.different_sets_arr[i].similar_sets_arr[j]);
            if (j < how_many_in_line - 1)
                printf(" ");
            else
                printf("\n"); //last index in line if followed by "\n" instead of " "
        }
    }
}
