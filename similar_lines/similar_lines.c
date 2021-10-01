#include <stdio.h>
#include "line_process.h"
#include "sets_sort.h"
#include "new_set_add.h"
#include "output.h"
#include "allocations_and_frees.h"

#define INITIAL_COUNT_OF_WORDS 1


void analyze_line(Set *line, Type type, AllSets *sets, size_t number_of_line, Results *results)
{
    if (type == ErrorLetter)
        fprintf(stderr, "ERROR %zu\n", number_of_line);
    else if (type == GoodLine)
    {
        sort_set(line); //sorting the numbers and non_numbers arrays
        add_new_set(sets, line, number_of_line,
                    results); //adding the line to an array of all sets and to array of results
    }
    //if type of word is different, then ignore the line
}

int main()
{
    AllSets sets;
    initialize_all_lines(&sets);
    enum Type type_of_word = InitialTypeOfWord;
    size_t count_lines = INITIAL_COUNT_OF_WORDS;
    Results results = initialize_results();

    while (type_of_word != EndOfFile)
    {
        Set new_line;
        type_of_word = turn_line_into_set(&new_line);
        analyze_line(&new_line, type_of_word, &sets, count_lines, &results);
        count_lines++;
    }

    print_the_result(results);
    free_everything(&results, &sets);

    return 0;
}


