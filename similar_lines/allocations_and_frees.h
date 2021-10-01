#ifndef SIMILAR_LINES_C_ALLOCATIONS_AND_FREEING_H
#define SIMILAR_LINES_C_ALLOCATIONS_AND_FREEING_H

#include "structs_and_const.h"

extern Results initialize_results();

extern void free_everything(Results *results, AllSets *sets);

extern void initialize_all_lines(AllSets *sets);


#endif //SIMILAR_LINES_C_ALLOCATIONS_AND_FREEING_H
