/** @file
 Struktury danych używane w parserze

 @author Julia Karmowska
 @date 2021
*/

#ifndef POLYNOMIALS_PARSER_STRUCTS_H
#define POLYNOMIALS_PARSER_STRUCTS_H
/**
 * Typ opisujący rodzaj wiersza. Poszczególne wartości odpowiadają nazwom poleceń lub błędów.
 */
typedef enum LineType {
    INITIAL_LINE_TYPE,
    IGNORED_LINE,
    ZERO,
    IS_COEFF,
    IS_ZERO,
    CLONE,
    ADD,
    MUL,
    NEG,
    SUB,
    IS_EQ,
    DEG,
    DEG_BY,
    AT,
    PRINT,
    POP,
    COMPOSE,
    WRONG_COMMAND,
    DEG_BY_WRONG_VARIABLE,
    AT_WRONG_VALUE,
    COMPOSE_WRONG_PARAMETER,
    WRONG_POLY,
    POLY,
    END_OF_FILE
} LineType;

/**
 * Unia reprezentująca parametr wczytanej instrukcji lub wczytany wielomian.
 */
typedef union InstructionVar{
    unsigned long deg_by_var; ///< parametr polecenia DEG_BY
    unsigned long compose_parameter; ///<parametr polecenia COMPOSE
    long at_val; ///<parametr polecenia AT
    Poly poly; ///<wczytany wielomian
}InstructionVar;

/**
 * Struktura reprezentująca wynik parsowania wiersza.
 */
typedef struct ParseResult{
    LineType type; ///<typ wczytanego wiersza
    InstructionVar variable; ///<parametr polecenia lub wczytany wielomian
}ParseResult;

#endif //POLYNOMIALS_PARSER_STRUCTS_H
