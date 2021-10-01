#include <stdlib.h>
#include "allocations_and_frees.h"


static void initialize_similar_sets_arr(Similar *similar_sets)
{
    similar_sets->how_many_similar = 0;
    similar_sets->size_of_arr = INITIAL_ARRAY_SIZE;

    similar_sets->similar_sets_arr = calloc(INITIAL_ARRAY_SIZE, sizeof(size_t));
    if (!similar_sets->similar_sets_arr)
        exit(1);
}

Results initialize_results()
{
    Results results;
    results.how_many_different = 0;
    results.size_of_different_arr = INITIAL_ARRAY_SIZE;

    results.different_sets_arr = calloc(INITIAL_ARRAY_SIZE, sizeof(Similar));
    if (!results.different_sets_arr)
        exit(1);

    for (size_t i = 0; i < INITIAL_ARRAY_SIZE; i++)
    {
        initialize_similar_sets_arr(&(results.different_sets_arr[i]));
    }

    return results;
}

void initialize_all_lines(AllSets *sets)
{
    sets->how_many_sets = 0;
    sets->size_of_sets_arr = INITIAL_ARRAY_SIZE;

    sets->arr_of_sets = calloc(INITIAL_ARRAY_SIZE, sizeof(Set));
    if (!sets->arr_of_sets)
        exit(1);
}

static void free_non_numbers(Set *line_to_free)
{
    for (size_t i = 0; i < line_to_free->how_many_non_numbers; i++)
    {
        free(line_to_free->non_numbers[i]);
    }
    free(line_to_free->non_numbers);
}

static void free_line(Set *line)
{
    free(line->numbers);
    free_non_numbers(line);
}

static void free_results(Results *results)
{
    for (size_t i = 0; i < results->size_of_different_arr; i++)
    {
        free(results->different_sets_arr[i].similar_sets_arr);
    }
    free(results->different_sets_arr);
}

static void free_all_sets(AllSets *all_lines)
{
    for (size_t i = 0; i < all_lines->how_many_sets; i++)
    {
        free_line(&(all_lines->arr_of_sets[i]));
    }
    free(all_lines->arr_of_sets);
}

void free_everything(Results *results, AllSets *sets)
{
    free_results(results);
    free_all_sets(sets);
}



