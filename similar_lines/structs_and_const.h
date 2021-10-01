#ifndef SIMILAR_LINES_C_STRUCTS_AND_CONST_H
#define SIMILAR_LINES_C_STRUCTS_AND_CONST_H

#include <stddef.h>

#define INITIAL_ARRAY_SIZE 15

//Structure that keeps data about one line of input
struct Set {
    size_t size_of_numbers_arr;
    size_t how_many_numbers;
    long double *numbers;
    size_t size_of_non_numbers_arr;
    size_t how_many_non_numbers;
    char **non_numbers;

};
typedef struct Set Set;

//Structure with an array of all different sets from input
struct AllSets {
    Set *arr_of_sets;
    size_t how_many_sets;
    size_t size_of_sets_arr;
};
typedef struct AllSets AllSets;

//Structure that stores indexes of similar lines
struct Similar {
    size_t how_many_similar;
    size_t size_of_arr;
    size_t *similar_sets_arr;//array of numbers of similar lines, if line 1 & 2 are similar, then they are in the array together
};
typedef struct Similar Similar;

//This struct stores an array of Similar arrays, in order to have access when printing the result.
// It is sorted by default because we add indexes in increasing order
struct Results {
    Similar *different_sets_arr;
    size_t how_many_different;
    size_t size_of_different_arr;
};
typedef struct Results Results;

#endif //SIMILAR_LINES_C_STRUCTS_AND_CONST_H
