#ifndef SIMILAR_LINES_C_WORD_TYPE_H
#define SIMILAR_LINES_C_WORD_TYPE_H

#include <stdbool.h>

//if first word in *beginning is a number, function puts its value into *number and returns true
extern bool is_a_number(char *beginning, char **end, long double *number);

//first word in *beginning is not a number, so function extracts the first word into **result
extern void extract_word(char *beginning, char **end, char **result);


#endif //SIMILAR_LINES_C_WORD_TYPE_H
