/** @file
 Interfejs operacji na stosie wielominów

 @author Julia Karmowska
 @date 2021
*/
#ifndef POLYNOMIALS_STACK_H
#define POLYNOMIALS_STACK_H

#include "poly.h"

/**
 * Struktura opisująca stos.
 */
typedef struct Stack Stack;

/**
 * Zdejmuje wielomian z wierzchołka stosu, usuwa go z pamięci.
 * @param[in] stack : stos
 */
extern void StackPop(Stack **stack);

/**
 * Przekazuje wielomian z wierzchołka stosu
 * @param[in] stack : stos
 * @return wielomian z wierzchołka stosu
 */
extern Poly StackTop(Stack *stack);

/**
 * Sprawdza, czy stos jest pusty.
 * @param[in] stack : stos
 * @return Czy stos jest pusty?
 */
extern bool StackIsEmpty(Stack *stack);

/**
 * Dodaje wielomian do stosu, modyfikuje wierzchołek stosu.
 * @param[in] poly : wielomian
 * @param[in] stack : stos
 */
extern void StackPush(Poly *poly, Stack **stack);

/**
 * Tworzy nowy pusty stos.
 * @param[in] stack : stos
 */
extern void StackInit(Stack **stack);

/**
 * Usuwa stos z pamięci (niszczy wszystkie wielomiany na stosie).
 * @param[in] stack : stos
 */
extern void StackClear(Stack **stack);

/**
 * Zwaraca liczbę wielomianów na stosie
 * @param[in] stack : stos
 * @return liczba wielomianów na stosie
 */
extern size_t StackCount(Stack *stack);


#endif //POLYNOMIALS_STACK_H
