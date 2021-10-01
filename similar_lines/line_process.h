#ifndef SIMILAR_LINES_C_LINE_PROCESS_H
#define SIMILAR_LINES_C_LINE_PROCESS_H

#include "structs_and_const.h"

enum Type {
    InitialTypeOfWord,
    Comment,
    ErrorLetter,
    GoodLine,
    EndOfFile,
    EmptyLine,
};
typedef enum Type Type;

//reads a line from input, returns type of line, if it is a valid line then creates a set out of it
extern Type turn_line_into_set(Set *new_line);

#endif //SIMILAR_LINES_C_LINE_PROCESS_H
