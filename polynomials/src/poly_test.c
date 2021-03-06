#ifdef NDEBUG
#undef NDEBUG
#endif

#include "poly.h"
#include <assert.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CHECK_PTR(p)  \
do {                \
if (p == NULL) {  \
exit(1);        \
}                 \
} while (0)

#define C PolyFromCoeff

static Mono M(Poly p, poly_exp_t n) {
    return MonoFromPoly(&p, n);
}

static Poly MakePolyHelper(poly_exp_t dummy, ...) {
    va_list list;
    va_start(list, dummy);
    size_t count = 0;
    while (true) {
        va_arg(list, Poly);
        if (va_arg(list, poly_exp_t) < 0)
            break;
        count++;
    }
    va_start(list, dummy);
    Mono *arr = calloc(count, sizeof (Mono));
    CHECK_PTR(arr);
    for (size_t i = 0; i < count; ++i) {
        Poly p = va_arg(list, Poly);
        arr[i] = MonoFromPoly(&p, va_arg(list, poly_exp_t));
        assert(i == 0 || MonoGetExp(&arr[i]) > MonoGetExp(&arr[i - 1]));
    }
    va_end(list);
    Poly res = PolyAddMonos(count, arr);
    free(arr);
    return res;
}

#define P(...) MakePolyHelper(0, __VA_ARGS__, PolyZero(), -1)

static bool TestOp(Poly a, Poly b, Poly res,
                   Poly (*op)(const Poly *, const Poly *)) {
    Poly c = op(&a, &b);
    bool is_eq = PolyIsEq(&c, &res);
    PolyDestroy(&a);
    PolyDestroy(&b);
    PolyDestroy(&c);
    PolyDestroy(&res);
    return is_eq;
}

static bool TestAdd(Poly a, Poly b, Poly res) {
    return TestOp(a, b, res, PolyAdd);
}

static bool TestAddMonos(size_t count, Mono monos[], Poly res) {
    Poly b = PolyAddMonos(count, monos);
    bool is_eq = PolyIsEq(&b, &res);
    PolyDestroy(&b);
    PolyDestroy(&res);
    return is_eq;
}

// Autor: Maurycy Wojda
// (chociaż niczym się nie różni od AddMonos)
static bool TestOwnMonos(size_t count, Mono monos[], Poly res) {
    Poly b = PolyOwnMonos(count, monos);
    bool is_eq = PolyIsEq(&b, &res);
    PolyDestroy(&b);
    PolyDestroy(&res);
    return is_eq;
}

// Autor: Maurycy Wojda
// Robimy głęboką kopię tablicy.
// Potem wołamy PolyCloneMonos.
// Sprawdzamy poprawność wyniku i zachowanie zawartości tablicy.
// Zwalniamy kopię tablicy.
// Niszczymy elementy tablicy wejściowej, bo nigdzie indziej
// to nie jest robione, w przeciwieństwie do AddMonos i OwnMonos.
static bool TestCloneMonos(size_t count, Mono monos[], Poly res) {
    Mono *monosCopy = malloc(count * sizeof(Mono));
    for (size_t i = 0; i < count; i++)
        monosCopy[i] = MonoClone(&monos[i]);

    Poly b = PolyCloneMonos(count, monos);
    bool is_eq = PolyIsEq(&b, &res);

    for (size_t i = 0; i < count; i++)
    {
        is_eq &= monosCopy[i].exp == monos[i].exp;
        is_eq &= PolyIsEq(&monosCopy[i].p, &monos[i].p);
    }

    for (size_t i = 0; i < count; i++)
        MonoDestroy(&monosCopy[i]);
    free(monosCopy);

    for (size_t i = 0; i < count; i++)
        MonoDestroy(&monos[i]);

    PolyDestroy(&b);
    PolyDestroy(&res);
    return is_eq;
}


static bool TestMul(Poly a, Poly b, Poly res) {
    return TestOp(a, b, res, PolyMul);
}

static bool TestSub(Poly a, Poly b, Poly res) {
    return TestOp(a, b, res, PolySub);
}

static bool TestDegBy(Poly a, size_t var_idx, poly_exp_t res) {
    bool is_eq = PolyDegBy(&a, var_idx) == res;
    PolyDestroy(&a);
    return is_eq;
}

static bool TestDeg(Poly a, poly_exp_t res) {
    bool is_eq = PolyDeg(&a) == res;
    PolyDestroy(&a);
    return is_eq;
}

static bool TestEq(Poly a, Poly b, bool res) {
    bool is_eq = PolyIsEq(&a, &b) == res;
    PolyDestroy(&a);
    PolyDestroy(&b);
    return is_eq;
}

static bool TestAt(Poly a, poly_coeff_t x, Poly res) {
    Poly b = PolyAt(&a, x);
    bool is_eq = PolyIsEq(&b, &res);
    PolyDestroy(&a);
    PolyDestroy(&b);
    PolyDestroy(&res);
    return is_eq;
}

static bool SimpleAddTest(void) {
    bool res = true;
    // Różne przypadki wielomian/współczynnik
    res &= TestAdd(C(1),
                   C(2),
                   C(3));
    res &= TestAdd(P(C(1), 1),
                   C(2),
                   P(C(2), 0, C(1), 1));
    res &= TestAdd(C(1),
                   P(C(2), 2),
                   P(C(1), 0, C(2), 2));
    res &= TestAdd(P(C(1), 1),
                   P(C(2), 2),
                   P(C(1), 1, C(2), 2));
    res &= TestAdd(C(0),
                   P(C(1), 1),
                   P(C(1), 1));
    // Upraszczanie wielomianu
    res &= TestAdd(P(C(1), 1),
                   P(C(-1), 1),
                   C(0));
    res &= TestAdd(P(C(1), 1, C(2), 2),
                   P(C(-1), 1),
                   P(C(2), 2));
    res &= TestAdd(P(C(2), 0, C(1), 1),
                   P(C(-1), 1),
                   C(2));
    // Dodawanie współczynnika i upraszczanie
    res &= TestAdd(C(1),
                   P(C(-1), 0, C(1), 1),
                   P(C(1), 1));
    res &= TestAdd(C(1),
                   P(P(C(-1), 0, C(1), 1), 0),
                   P(P(C(1), 1), 0));
    res &= TestAdd(C(1),
                   P(C(1), 0, C(2), 2),
                   P(C(2), 0, C(2), 2));
    res &= TestAdd(C(1),
                   P(P(C(1), 0, C(1), 1), 0, C(2), 2),
                   P(P(C(2), 0, C(1), 1), 0, C(2), 2));
    res &= TestAdd(C(1),
                   P(P(C(-1), 0, C(1), 1), 0, C(2), 2),
                   P(P(C(1), 1), 0, C(2), 2));
    // Dodawanie wielomianów wielu zmiennych
    res &= TestAdd(P(P(C(1), 2), 0, P(C(2), 1), 1, C(1), 2),
                   P(P(C(1), 2), 0, P(C(-2), 1), 1, C(1), 2),
                   P(P(C(2), 2), 0, C(2), 2));
    res &= TestAdd(P(P(C(1), 2), 0, P(C(2), 1), 1, C(1), 2),
                   P(P(C(-1), 2), 0, P(C(1), 0, C(2), 1, C(1), 2), 1, C(-1), 2),
                   P(P(C(1), 0, C(4), 1, C(1), 2), 1));
    // Redukcja do współczynnika
    res &= TestAdd(P(P(C(1), 0, C(1), 1), 0, C(1), 1),
                   P(P(C(1), 0, C(-1), 1), 0, C(-1), 1),
                   C(2));
    res &= TestAdd(P(P(P(C(1), 0, C(1), 1), 0, C(1), 1), 1),
                   P(P(P(C(1), 0, C(-1), 1), 0, C(-1), 1), 1),
                   P(C(2), 1));
    // Dodawanie wielomianu do siebie
    Poly a = P(C(1), 1);
    Poly b = PolyAdd(&a, &a);
    Poly c = P(C(2), 1);
    res &= PolyIsEq(&b, &c);
    PolyDestroy(&a);
    PolyDestroy(&b);
    PolyDestroy(&c);
    return res;
}

static bool SimpleAddMonosTest(void) {
    bool res = true;
    {
        Mono m[] = {M(C(-1), 0), M(C(1), 0)};
        res &= TestAddMonos(2, m, C(0));
    }
    {
        Mono m[] = {M(C(-1), 1), M(C(1), 1)};
        res &= TestAddMonos(2, m, C(0));
    }
    {
        Mono m[] = {M(C(1), 0), M(C(1), 0)};
        res &= TestAddMonos(2, m, C(2));
    }
    {
        Mono m[] = {M(C(1), 1), M(C(1), 1)};
        res &= TestAddMonos(2, m, P(C(2), 1));
    }
    {
        Mono m[] = {M(P(C(-1), 1), 0), M(P(C(1), 1), 0)};
        res &= TestAddMonos(2, m, C(0));
    }
    {
        Mono m[] = {M(P(C(-1), 0), 1),
                    M(P(C(1), 0), 1),
                    M(C(2), 0),
                    M(C(1), 1),
                    M(P(C(2), 1), 2),
                    M(P(C(2), 2), 2)};
        res &= TestAddMonos(6, m, P(C(2), 0, C(1), 1, P(C(2), 1, C(2), 2), 2));
    }
    return res;
}

// Autor: Maurycy Wojda
// Funkcja pomocnicza do SimpleAddMonosTest
static void prepM(Mono **m, Mono t[], size_t c)
{
    *m = malloc(c * sizeof(Mono));
    memcpy(*m, t, c * sizeof(Mono));
}


// Autor: Maurycy Wojda
// Należy się upewnić, że OwnMonos przyjęło tablicę na własność.
// Sprawdzać z valgrindem!
static bool SimpleOwnMonosTest(void) {
    bool res = true;
    Mono *m = NULL;
    {
        Mono t[] = {M(C(-1), 0), M(C(1), 0)};
        prepM(&m, t, 2);
        res &= TestOwnMonos(2, m, C(0));
    }
    {
        Mono t[] = {M(C(-1), 1), M(C(1), 1)};
        prepM(&m, t, 2);
        res &= TestOwnMonos(2, m, C(0));
    }
    {
        Mono t[] = {M(C(1), 0), M(C(1), 0)};
        prepM(&m, t, 2);
        res &= TestOwnMonos(2, m, C(2));
    }
    {
        Mono t[] = {M(C(1), 1), M(C(1), 1)};
        prepM(&m, t, 2);
        res &= TestOwnMonos(2, m, P(C(2), 1));
    }
    {
        Mono t[] = {M(P(C(-1), 1), 0), M(P(C(1), 1), 0)};
        prepM(&m, t, 2);
        res &= TestOwnMonos(2, m, C(0));
    }
    {
        Mono t[] = {M(P(C(-1), 0), 1),
                    M(P(C(1), 0), 1),
                    M(C(2), 0),
                    M(C(1), 1),
                    M(P(C(2), 1), 2),
                    M(P(C(2), 2), 2)};
        prepM(&m, t, 6);
        res &= TestOwnMonos(6, m, P(C(2), 0, C(1), 1, P(C(2), 1, C(2), 2), 2));
    }
    return res;
}

// Autor: Maurycy Wojda
// Należy się upewnić, że CloneMonos zrobiło pełną głęboką kopię.
// Większość pracy jest wykonywane przez TestCloneMonos,
// tam można znaleźć więcej wyjaśnień.
// Sprawdzać z valgrindem!
static bool SimpleCloneMonosTest(void) {
    bool res = true;
    {
        Mono t[] = {M(C(-1), 0), M(C(1), 0)};
        res &= TestCloneMonos(2, t, C(0));
    }
    {
        Mono t[] = {M(C(-1), 1), M(C(1), 1)};
        res &= TestCloneMonos(2, t, C(0));
    }
    {
        Mono t[] = {M(C(1), 0), M(C(1), 0)};
        res &= TestCloneMonos(2, t, C(2));
    }
    {
        Mono t[] = {M(C(1), 1), M(C(1), 1)};
        res &= TestCloneMonos(2, t, P(C(2), 1));
    }
    {
        Mono t[] = {M(P(C(-1), 1), 0), M(P(C(1), 1), 0)};
        res &= TestCloneMonos(2, t, C(0));
    }
    {
        Mono t[] = {M(P(C(-1), 0), 1),
                    M(P(C(1), 0), 1),
                    M(C(2), 0),
                    M(C(1), 1),
                    M(P(C(2), 1), 2),
                    M(P(C(2), 2), 2)};
        res &= TestCloneMonos(6, t, P(C(2), 0, C(1), 1, P(C(2), 1, C(2), 2), 2));
    }
    return res;
}

static bool SimpleMulTest(void) {
    bool res = true;
    res &= TestMul(C(2),
                   C(3),
                   C(6));
    res &= TestMul(P(C(1), 1),
                   C(2),
                   P(C(2), 1));
    res &= TestMul(C(3),
                   P(C(2), 2),
                   P(C(6), 2));
    res &= TestMul(P(C(1), 1),
                   P(C(2), 2),
                   P(C(2), 3));
    res &= TestMul(P(C(-1), 0, C(1), 1),
                   P(C(1), 0, C(1), 1),
                   P(C(-1), 0, C(1), 2));
    res &= TestMul(P(P(C(1), 2), 0, P(C(1), 1), 1, C(1), 2),
                   P(P(C(1), 2), 0, P(C(-1), 1), 1, C(1), 2),
                   P(P(C(1), 4), 0, P(C(1), 2), 2, C(1), 4));
    return res;
}

static bool SimpleNegTest(void) {
    Poly a = P(P(C(1), 0, C(2), 2), 0, P(C(1), 1), 1, C(1), 2);
    Poly b = PolyNeg(&a);
    Poly c = P(P(C(-1), 0, C(-2), 2), 0, P(C(-1), 1), 1, C(-1), 2);
    bool is_eq = PolyIsEq(&b, &c);
    PolyDestroy(&a);
    PolyDestroy(&b);
    PolyDestroy(&c);
    return is_eq;
}

static bool SimpleSubTest(void) {
    return TestSub(P(P(C(1), 2), 0, P(C(2), 1), 1, C(1), 2),
                   P(P(C(1), 2), 0, P(C(-1), 0, C(-2), 1, C(-1), 2), 1, C(1), 2),
                   P(P(C(1), 0, C(4), 1, C(1), 2), 1));
}

#define POLY_P P(P(C(1), 3), 0, P(C(1), 2), 2, C(1), 3)

static bool SimpleDegByTest(void) {
    bool res = true;
    res &= TestDegBy(C(0), 1, -1);
    res &= TestDegBy(C(1), 0, 0);
    res &= TestDegBy(P(C(1), 1), 1, 0);
    res &= TestDegBy(POLY_P, 0, 3);
    res &= TestDegBy(POLY_P, 1, 3);
    return res;
}

static bool SimpleDegTest(void) {
    bool res = true;
    res &= TestDeg(C(0), -1);
    res &= TestDeg(C(1), 0);
    res &= TestDeg(P(C(1), 1), 1);
    res &= TestDeg(POLY_P, 4);
    return res;
}

static bool SimpleIsEqTest(void) {
    bool res = true;
    res &= TestEq(C(0), C(0), true);
    res &= TestEq(C(0), C(1), false);
    res &= TestEq(C(1), C(1), true);
    res &= TestEq(P(C(1), 1), C(1), false);
    res &= TestEq(P(C(1), 1), P(C(1), 1), true);
    res &= TestEq(P(C(1), 1), P(C(1), 2), false);
    res &= TestEq(P(C(1), 1), P(C(2), 1), false);
    res &= TestEq(POLY_P, POLY_P, true);
    Poly a = C(1);
    Poly b = C(2);
    Poly p = POLY_P;
    res &= TestEq(PolyAdd(&p, &a), PolyAdd(&p, &b), false);
    PolyDestroy(&a);
    PolyDestroy(&b);
    PolyDestroy(&p);
    return res;
}

static bool SimpleAtTest(void) {
    bool res = true;
    res &= TestAt(C(2), 1, C(2));
    res &= TestAt(P(C(1), 0, C(1), 18), 10, C(1000000000000000001L));
    res &= TestAt(P(C(3), 1, C(2), 3, C(1), 5), 10, C(102030));
    res &= TestAt(P(P(C(1), 4), 0, P(C(1), 2), 2, C(1), 3), 2,
                  P(C(8), 0, C(4), 2, C(1), 4));
    return res;
}

static bool OverflowTest(void) {
    bool res = true;
    res &= TestMul(P(C(1L << 32), 1), C(1L << 32), C(0));
    res &= TestAt(P(C(1), 64), 2, C(0));
    res &= TestAt(P(C(1), 0, C(1), 64), 2, C(1));
    res &= TestAt(P(P(C(1), 1), 64), 2, C(0));
    return res;
}

/** URUCHAMIANIE TESTÓW **/

// Liczba elementów tablicy x
#define SIZE(x) (sizeof (x) / sizeof (x)[0])

/**
 * Struktura jednego elementu listy testów.
 */
typedef struct {
    /**
	 * Nazwa testu.
	 */
    char const *name;
    /**
     * Funkcja do wywołania.
     */
    bool (*function)(void);
} test_list_t;

#define TEST(t) {#t, t}

/**
 * Lista testów.
 */
// Zmodyfikowano SimpleAddMonosTest (Maurycy Wojda)
// Dodano SimpleOwnMonosTest (Maurycy Wojda)
// Dodano SimpleCloneMonosTest (Maurycy Wojda)
static const test_list_t test_list[] = {
        TEST(SimpleAddTest),
        TEST(SimpleAddMonosTest),
        TEST(SimpleOwnMonosTest),
        TEST(SimpleCloneMonosTest),
        TEST(SimpleMulTest),
        TEST(SimpleNegTest),
        TEST(SimpleSubTest),
        //TEST(SimpleNegGroup),
        TEST(SimpleDegByTest),
        TEST(SimpleDegTest),
        //TEST(SimpleDegGroup),
        TEST(SimpleIsEqTest),
        TEST(SimpleAtTest),
        TEST(OverflowTest),
        /*TEST(SimpleArithmeticTest),
        TEST(LongPolynomialTest),
        TEST(AtTest1),
        TEST(AtTest2),
        TEST(AtGroup),
        TEST(DegreeOpChangeTest),
        TEST(DegTest),
        TEST(DegByTest),
        TEST(DegGroup),
        TEST(MulTest1),
        TEST(MulTest2),
        TEST(AddTest1),
        TEST(AddTest2),
        TEST(SubTest1),
        TEST(SubTest2),
        TEST(ArithmeticGroup),
        TEST(IsEqTest),
        TEST(RarePolynomialTest),
        TEST(MemoryThiefTest),
        TEST(MemoryFreeTest),
        TEST(MemoryGroup),*/
        };

int main() {

    bool OK = true;
    for (size_t i = 0; i < SIZE(test_list); ++i)
    {
        fprintf(stderr, "\r%ld/%ld", i, SIZE(test_list));
        OK &= test_list[i].function();
    }
    fprintf(stderr, "\r       \r%s\n", OK ? "OK!" : "BŁĄD!");

    return 0;
}
