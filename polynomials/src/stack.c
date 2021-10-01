/** @file
 Implementacja operacji na stosie wielominów

 @author Julia Karmowska
 @date 2021
*/

#include <stdlib.h>
#include "stack.h"
#include "memory.h"


/**
 * Struktura opisująca stos wielomianów, implementacja tablicowa.
 */
struct Stack {
    Poly *polys;        ///< wskaźnik do tablicy wielomianów
    size_t size;        ///< rozmiar tablicy
    size_t used;        ///< liczba wielomianów w tablicy
};

void StackInit(Stack **stack)
{
    *stack = calloc(1, sizeof(Stack));
    CHECK_PTR(*stack);
    (*stack)->used = 0;
    ((*stack)->polys) = calloc(INITIAL_ARRAY_SIZE, sizeof(Poly));
    CHECK_PTR((*stack)->polys);
    (*stack)->size = INITIAL_ARRAY_SIZE;
}

void StackPop(Stack **stack)
{
    assert((*stack)->used > 0);
    PolyDestroy(&((*stack)->polys[(*stack)->used - 1]));
    ((*stack)->used)--;
}

Poly StackTop(Stack *stack)
{
    assert(stack->used > 0);
    return stack->polys[stack->used - 1];
}


/**
 * Zwiększa rozmiar tablicy.
 * @param[in] n rozmiar
 * @return @f$ 1 + 2 * n @f$
 */
static size_t increase_size(size_t n)
{
    if (n == SIZE_MAX) //SIZE_MAX może zostać osiągnięty, bo n = 2^m - 1
        exit(1); //jeśli zostanie osiągnięty, to nie możemy już zwiększyć rozmiaru
    return 1 + 2 * n;
}

/**
 * Realokuje stos, jeśli to konieczne
 * @param[in] stack : stos
 */
static void MaybeReallocArr(Stack **stack)
{
    if ((*stack)->used >= (*stack)->size)
    {
        (*stack)->size = increase_size((*stack)->size);
        (*stack)->polys = realloc((*stack)->polys, sizeof(Poly) * (*stack)->size);
        CHECK_PTR((*stack)->polys);
    }
}

void StackPush(Poly *poly, Stack **stack)
{
    MaybeReallocArr(stack);
    (*stack)->polys[(*stack)->used] = *poly;
    ((*stack)->used)++;
}

bool StackIsEmpty(Stack *stack)
{
    return stack->used == 0;
}

void StackClear(Stack **stack)
{
    while (!StackIsEmpty(*stack))
    {
        StackPop(stack);
    }
    free((*stack)->polys);
    free(*stack);
}

size_t StackCount(Stack *stack)
{
    return stack->used;
}