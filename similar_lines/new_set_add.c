#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "new_set_add.h"


static size_t increase_size(size_t n)
{
    if (n == SIZE_MAX) //SIZE_MAX can be reached as n = 2^m - 1
        exit(1); //if it is reached, then n can no longer be increased thus exit(1)

    return 1 + 2 * n;
}

//Checks if line1 and line 2 are similar (if they have the same arrays of numbers and non numbers)
static bool similar_lines(Set line1, Set line2)
{
    if (line1.how_many_numbers != line2.how_many_numbers)
        return false;
    if (line1.how_many_non_numbers != line2.how_many_non_numbers)
        return false;

    //arrays of numbers and non numbers are sorted so checking if both sets have the same number/non_number on each index
    for (size_t i = 0; i < line1.how_many_numbers; i++)
    {
        if (line1.numbers[i] != line2.numbers[i])
            return false;
    }
    for (size_t i = 0; i < line1.how_many_non_numbers; i++)
    {
        if (strcmp(line1.non_numbers[i], line2.non_numbers[i]) != 0)
            return false;
    }
    return true;
}

static void reallocate_similar_sets_arr(size_t **similar_sets_arr, size_t *size)
{
    *size = increase_size(*size);

    *similar_sets_arr = realloc(*similar_sets_arr, *size * sizeof(size_t));
    if (!*similar_sets_arr)
        exit(1);
}

static void reallocate_arr_of_lines(Set **arr_of_lines, size_t *size)
{
    *size = increase_size(*size);

    *arr_of_lines = realloc(*arr_of_lines, *size * sizeof(Set));
    if (!*arr_of_lines)
        exit(1);
}

static void initialize_similar_sets_arr(Similar *similar_sets)
{
    similar_sets->how_many_similar = 0;
    similar_sets->size_of_arr = INITIAL_ARRAY_SIZE;

    similar_sets->similar_sets_arr = calloc(INITIAL_ARRAY_SIZE, sizeof(size_t));
    if (!similar_sets->similar_sets_arr)
        exit(1);
}

static void maybe_reallocate_results(Results *results)
{
    if (results->how_many_different >= results->size_of_different_arr)
    {
        size_t old_size = results->size_of_different_arr;
        results->size_of_different_arr = increase_size(results->size_of_different_arr);

        results->different_sets_arr = realloc(results->different_sets_arr,
                                              results->size_of_different_arr * sizeof(Similar));
        if (!results->different_sets_arr)
            exit(1);

        //initializing different_sets_array on indexes that were added during reallocation
        for (size_t i = old_size; i < results->size_of_different_arr; i++)
        {
            initialize_similar_sets_arr(&(results->different_sets_arr[i]));
        }
    }
}

static void add_number_to_result(size_t number_of_line, size_t index, Results *results)
{
    size_t how_many = results->different_sets_arr[index].how_many_similar;
    size_t size = results->different_sets_arr[index].size_of_arr;

    if (how_many >= size)//reallocating if necessary
    {
        reallocate_similar_sets_arr(&(results->different_sets_arr[index].similar_sets_arr),
                                    &(results->different_sets_arr[index].size_of_arr));
    }

    (results->different_sets_arr[index].similar_sets_arr)[how_many] = number_of_line; //adding new number of line to results
    (results->different_sets_arr[index].how_many_similar)++; //increasing count of similar sets
}

static void add_new_line_to_sets(AllSets *sets, Set *new_line)
{
    if (sets->how_many_sets >= sets->size_of_sets_arr)
        reallocate_arr_of_lines(&(sets->arr_of_sets),
                                &(sets->size_of_sets_arr)); //reallocating the array if necessary
    sets->arr_of_sets[sets->how_many_sets] = *new_line; //adding new_line to array of all sets
    (sets->how_many_sets)++;
}

static void free_non_numbers(Set *line_to_free)
{
    for (size_t i = 0; i < line_to_free->how_many_non_numbers; i++)
    {
        free(line_to_free->non_numbers[i]);
    }
    free(line_to_free->non_numbers);
}

//freeing the numbers and non_numbers arrays in line
static void free_line(Set *line)
{
    free(line->numbers);
    free_non_numbers(line);
}

static void
add_new_line_not_similar_to_any_previous(AllSets *sets, Set *new_set, size_t number_of_new_line, Results *results)
{
    //none of the previous lines were similar, so we need to add new_set to the array of all sets
    //and we also need to create new array in results to store indexes of lines similar to new_line
    add_new_line_to_sets(sets, new_set);
    maybe_reallocate_results(results);
    add_number_to_result(number_of_new_line, sets->how_many_sets - 1, results);
    (results->how_many_different)++;
}

void add_new_set(AllSets *sets, Set *new_set, size_t number_of_new_line, Results *results)
{
    //checking if new_set is similar to any of the previous ones
    for (size_t i = 0; i < sets->how_many_sets; i++)
    {
        if (similar_lines(*new_set, sets->arr_of_sets[i]))
        {
            //new_set is similar to a previous one so we add number of the new line to the results,
            //we also don't need to store new_set anymore because similar set is already stored, thus we can free the memory
            add_number_to_result(number_of_new_line, i, results);
            free_line(new_set);
            return;
        }
    }
    //none of the previous lines were similar to new_set
    add_new_line_not_similar_to_any_previous(sets, new_set, number_of_new_line, results);
}
