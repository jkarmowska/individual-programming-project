/** @file
 Implementacja kalkulatora działającego na wielomianach i stosującego ONP

 @author Julia Karmowska
 @date 2021
*/

#include <stdio.h>
#include <stdlib.h>
#include "stack.h"
#include "poly.h"
#include "parser.h"



/**
 * Wstawia na wierzchołek stosu wielomian zerowy.
 * @param[in] stack : stos
 */
void Zero(Stack **stack)
{
    Poly poly = PolyZero();
    StackPush(&poly, stack);
}

/**
 * Sprawdza, czy na stosie jest co najmniej jeden wielomian - jeśli nie, wypisuje komunikat o niedomiarze wielomianów.
 * @param[in] stack : stos
 * @param[in] line_number : numer wiersza
 * @param[in] how_many_needed : ile wielomianów jest potrzebnych do wykonania polecenia
 * @return Czy na stosie jest jakiś wielomian?
 */
bool EnoughInStack(Stack *stack, unsigned long line_number, size_t how_many_needed)
{
    if (StackCount(stack) < how_many_needed)
    {
        fprintf(stderr, "ERROR %lu STACK UNDERFLOW\n", line_number);
        return false;
    }
    return true;
}

/**
 * Sprawdza, czy wielomian na wierzchołku stosu jest współczynnikiem – wypisuje 0 lub 1
 * @param[in] stack : stos
 * @param[in] line_number : numer wiersza
 */
void IsCoeff(Stack **stack, unsigned long line_number)
{
    if (EnoughInStack(*stack, line_number, 1))
    {
        Poly top = StackTop(*stack);
        if (PolyIsCoeff(&top))
            printf("1\n");
        else printf("0\n");
    }
}

/**
 * Sprawdza, czy wielomian na wierzchołku stosu jest tożsamościowo równy zeru – wypisuje 0 lub 1.
 * @param[in] stack : stos
 * @param[in] line_number : numer wiersza
 */
void IsZero(Stack **stack, unsigned long line_number)
{
    if (EnoughInStack(*stack, line_number, 1))
    {
        Poly top = StackTop(*stack);
        if (PolyIsZero(&top))
            printf("1\n");
        else printf("0\n");
    }
}

/**
 * Wstawia na stos kopię wielomianu z wierzchołka.
 * @param[in] stack : stos
 * @param[in] line_number : numer wiersza
 */
void Clone(Stack **stack, unsigned long line_number)
{
    if (EnoughInStack(*stack, line_number, 1))
    {
        Poly top = StackTop(*stack);
        Poly new = PolyClone(&top);
        StackPush(&new, stack);
    }
}

/**
 * Dodaje dwa wielomiany z wierzchu stosu, usuwa je i wstawia na wierzchołek stosu ich iloczyn.
 * @param[in] stack : stos
 * @param[in] line_number : numer wiersza
 */
void Add(Stack **stack, unsigned long line_number)
{
    if (EnoughInStack(*stack, line_number, 2))
    {
        Poly top = StackTop(*stack); //trzeba sklonować, bo gdy wykonujemy Pop, to wielomian z wierzchołka jest usuwany
        Poly first = PolyClone(&top);
        StackPop(stack);

        top = StackTop(*stack);
        Poly second = PolyClone(&top);
        StackPop(stack);

        Poly res = PolyAdd(&first, &second);
        PolyDestroy(&first);
        PolyDestroy(&second);
        StackPush(&res, stack);
    }
}

/**
 * Mnoży dwa wielomiany z wierzchu stosu, usuwa je i wstawia na wierzchołek stosu ich iloczyn.
 * @param[in] stack : stos
 * @param[in] line_number : numer wiersza
 */
void Mul(Stack **stack, unsigned long line_number)
{
    if (EnoughInStack(*stack, line_number, 2))
    {
        Poly top = StackTop(*stack); //trzeba sklonować, bo gdy wykonujemy Pop, to wielomian z wierzchołka jest usuwany
        Poly first = PolyClone(&top);
        StackPop(stack);

        top = StackTop(*stack);
        Poly second = PolyClone(&top);
        StackPop(stack);

        Poly res = PolyMul(&first, &second);
        PolyDestroy(&first);
        PolyDestroy(&second);
        StackPush(&res, stack);
    }

}

/**
 * Neguje wielomian na wierzchołku stosu.
 * @param[in] stack : stos
 * @param[in] line_number : numer wiersza
 */
void Neg(Stack **stack, unsigned long line_number)
{
    if (EnoughInStack(*stack, line_number, 1))
    {
        Poly top = StackTop(*stack);
        Poly neg = PolyNeg(&top);
        StackPop(stack);
        StackPush(&neg, stack);
    }
}

/**
 * Odejmuje od wielomianu z wierzchołka wielomian pod wierzchołkiem, usuwa je i wstawia na wierzchołek stosu różnicę.
 * @param[in] stack : stos
 * @param[in] line_number : numer wiersza
 */
void Sub(Stack **stack, unsigned long line_number)
{
    if (EnoughInStack(*stack, line_number, 2))
    {
        Poly top = StackTop(*stack); //trzeba sklonować, bo gdy wykonujemy Pop, to wielomian z wierzchołka jest usuwany
        Poly first = PolyClone(&top);
        StackPop(stack);

        top = StackTop(*stack);
        Poly second = PolyClone(&top);
        StackPop(stack);

        Poly res = PolySub(&first, &second);
        PolyDestroy(&first);
        PolyDestroy(&second);
        StackPush(&res, stack);
    }
}

/**
 * Sprawdza, czy dwa wielomiany na wierzchu stosu są równe – wypisuje 0 lub 1.
 * @param[in] stack : stos
 * @param[in] line_number : numer wiersza
 */
void IsEq(Stack **stack, unsigned long line_number)
{
    if (EnoughInStack(*stack, line_number, 2))
    {
        Poly top = StackTop(*stack); //trzeba sklonować, bo gdy wykonujemy Pop, to wielomian z wierzchołka jest usuwany
        Poly first = PolyClone(&top);
        StackPop(stack);

        Poly second = StackTop(*stack);
        if (PolyIsEq(&first, &second))
            printf("1\n");
        else printf("0\n");

        StackPush(&first, stack);

    }
}

/**
 * Wypisuje stopień wielomianu z wierzchołka stosu.
 * @param[in] stack : stos
 * @param[in] line_number : numer wiersza
 */
void Deg(Stack **stack, unsigned long line_number)
{
    if (EnoughInStack(*stack, line_number, 1))
    {
        Poly top = StackTop(*stack);
        printf("%d\n", PolyDeg(&top));
    }
}

/**
 * Wypisuje stopień wielomianu z wierzchołka stosu ze względu na zmienną o podanym numerze.
 * @param[in] stack : stos
 * @param[in] deg_by_var : numer zmiennej
 * @param[in] line_number : numer wiersza
 */
void DegBy(Stack **stack, unsigned long deg_by_var, unsigned long line_number)
{
    if (EnoughInStack(*stack, line_number, 1))
    {
        Poly top = StackTop(*stack);
        poly_exp_t deg = PolyDegBy(&top, deg_by_var);
        printf("%d\n", deg);
    }
}


/**
 * Wylicza wartość wielomianu w punkcie, usuwa wielomian z wierzchołka i wstawia na stos wynik operacji.
 * @param[in] stack : stos
 * @param[in] at_val : punkt, dla którego wyliczana jest wartość wielomianu
 * @param[in] line_number : numer wiersza
 */
void At(Stack **stack, long at_val, unsigned long line_number)
{
    if (EnoughInStack(*stack, line_number, 1))
    {
        Poly top = StackTop(*stack);
        Poly new = PolyAt(&top, at_val);
        StackPop(stack);
        StackPush(&new, stack);
    }
}

/**
 * Wypisuje wielomian z wierzchołka stosu.
 * @param[in] stack : stos
 * @param[in] line_number : numer wiersza
 */
void Print(Stack **stack, unsigned long line_number)
{
    if (EnoughInStack(*stack, line_number, 1))
    {
        Poly top = StackTop(*stack);
        PolyPrint(&top);
        printf("\n");
    }
}

/**
 * Zdejmuje wielomian ze stosu.
 * @param[in] stack : stos
 * @param[in] line_number : numer wiersza
 */
void Pop(Stack **stack, unsigned long line_number)
{
    if (EnoughInStack(*stack, line_number, 1))
        StackPop(stack);
}

/**
 * Dodaje wielomian na stos.
 * @param[in] stack : stos
 * @param[in] poly : wielomian
 */
void NewPoly(Stack **stack, Poly *poly)
{
    StackPush(poly, stack);
}

/**
 * Zdejmuje z wierzchołka stosu najpierw wielomian p, a potem kolejno wielomiany
 * q[k - 1], q[k - 2], …, q[0] i umieszcza na stosie wynik operacji złożenia.
 * @param[in] stack : stos
 * @param[in] k : liczba wielomianów do zdjęcia ze stosu, nie wliczając pierwszego
 * @param[in] line_number : numer wiersza
 */
void Compose(Stack **stack, size_t k, unsigned long line_number)
{
    if (StackCount(*stack) == 0 || StackCount(*stack) - 1 < k)//nie używam EnoughInStack, bo k+1 może spowodować overflow
    {
        fprintf(stderr, "ERROR %lu STACK UNDERFLOW\n", line_number);
        return;
    }

    Poly top = StackTop(*stack);
    Poly p = PolyClone(&top);
    StackPop(stack);

    Poly *q = calloc(k, sizeof(Poly));
    assert(q);
    for (size_t i = 0; i < k; i++)
        q[i] = PolyZero();

    for (size_t i = 1; i <= k; i++)
    {
        top = StackTop(*stack);
        q[k - i] = PolyClone(&top);
        StackPop(stack);
    }
    Poly res = PolyCompose(&p, k, q);
    StackPush(&res, stack);
    for (size_t i = 0; i < k; i++)
        PolyDestroy(&(q[i]));
    PolyDestroy(&p);
    free(q);

}

/**
 * Wykonuje operację zgodną z typem wiersza. Jeśli wiersz był wielomianem, dodaje wielomian na stos.
 * Jeśli wiersz był poleceniem, wykonuje to polecenie.
 * Jeśli wiersz był błędny, wypisuje odpowiedni komunikat.
 * @param[in] line_number : numer wiersza
 * @param[in] type : typ wiersza
 * @param[in] stack : stos
 * @param[in] instruction_var : parametr polecenia DEG_BY, AT lub COMPOSE lub wielomian
 */
void Calculate(unsigned long line_number, LineType type, Stack **stack, InstructionVar instruction_var)
{
    switch (type)
    {
        case WRONG_POLY:
            fprintf(stderr, "ERROR %lu WRONG POLY\n", line_number);
            break;
        case WRONG_COMMAND:
            fprintf(stderr, "ERROR %lu WRONG COMMAND\n", line_number);
            break;
        case DEG_BY_WRONG_VARIABLE:
            fprintf(stderr, "ERROR %lu DEG BY WRONG VARIABLE\n", line_number);
            break;
        case AT_WRONG_VALUE:
            fprintf(stderr, "ERROR %lu AT WRONG VALUE\n", line_number);
            break;
        case COMPOSE_WRONG_PARAMETER:
            fprintf(stderr, "ERROR %lu COMPOSE WRONG PARAMETER\n", line_number);
            break;
        case ZERO:
            Zero(stack);
            break;
        case IS_COEFF:
            IsCoeff(stack, line_number);
            break;
        case IS_ZERO:
            IsZero(stack, line_number);
            break;
        case CLONE:
            Clone(stack, line_number);
            break;
        case ADD:
            Add(stack, line_number);
            break;
        case MUL:
            Mul(stack, line_number);
            break;
        case NEG:
            Neg(stack, line_number);
            break;
        case SUB:
            Sub(stack, line_number);
            break;
        case IS_EQ:
            IsEq(stack, line_number);
            break;
        case DEG:
            Deg(stack, line_number);
            break;
        case DEG_BY:
            DegBy(stack, instruction_var.deg_by_var, line_number);
            break;
        case AT:
            At(stack, instruction_var.at_val, line_number);
            break;
        case PRINT:
            Print(stack, line_number);
            break;
        case POP:
            Pop(stack, line_number);
            break;
        case POLY:
            NewPoly(stack, &instruction_var.poly);
            break;
        case COMPOSE:
            Compose(stack, instruction_var.compose_parameter, line_number);
            break;
        default:
            break;
    }
}

/**
 * Tworzy stos wielominów.
 * Wczytuje polecenia i wykonuje zgodne z nimi operacje na wielomianach.
 */
int main()
{
    Stack *stack;
    StackInit(&stack);
    ParseResult parse_res;
    parse_res.type = INITIAL_LINE_TYPE;
    unsigned long line_number = 1;
    while (parse_res.type != END_OF_FILE)
    {
        parse_res.variable.poly = PolyZero();
        parse_res = ParseLine();
        if (parse_res.type != IGNORED_LINE && parse_res.type != END_OF_FILE)
            Calculate(line_number, parse_res.type, &stack, parse_res.variable);
        line_number++;
    }

    StackClear(&stack);
    return 0;
}