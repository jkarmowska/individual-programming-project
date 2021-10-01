/** @file
 Implementacja parsera wiersza danych wejściowych

 @author Julia Karmowska
 @date 2021
*/

/**
 * Umożliwia działanie funkcji getline.
 */
#define _GNU_SOURCE

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include "parser.h"
#include "poly_parser.h"


/**
 * To jest stała reprezentująca brak wczytanego wiersza
 */
#define NO_INPUT -1

/**
 * To jest stała reprezentująca błąd funkcji getline
 */
#define GETLINE_ERROR -1

/**
 * To jest stała reprezentująca znak spacji
 */
#define SPACE ' '

/**
 * To jest stała reprezentująca długość wyrażenia 'DEG_BY'
 */
#define DEG_BY_LENGTH 6

/**
 * To jest stała reprezentująca długość wyrażenia 'AT'
*/
#define AT_LENGTH 2

/**
 * To jest stała reprezentująca długość wyrażenia 'COMPOSE'
*/
#define COMPOSE_LENGTH 7

/**
 * To jest stała reprezentująca długość wyrażenia ' '
*/
#define SPACE_LENGTH 1

/**
 * To jest stała reprezentująca długość najkrótszego możliwego poprawnego polecenia
*/
#define MINIMAL_INSTRUCTION_LENGTH 2



/**
 * Wczytuje wiersz wejścia.
 * @param[in] line wczytany wiersz
 * @return długość wiersza (lub NO_INPUT, jeśli nie ma wiersza do wczytania)
 */
static long read_line_into_string(char **line)
{
    size_t size = 0;
    errno = 0;
    long result;

    result = getline(line, &size, stdin); //getline zwraca długość linii lub GETLINE_ERRROR

    if (result == GETLINE_ERROR && errno == ENOMEM) //problem z pamięcią
        exit(1);

    else if (result == GETLINE_ERROR) //nie wczytano wiersza - koniec wejścia
        return NO_INPUT;

    return result;
}

/**
 * Sprawdza, czy wiersz powienien być zignorowany.
 * @param[in] line : wiersz
 * @param[in] length : długość wiersza
 * @return Czy wiersz powinien być zignorowany?
 */
static bool ignoredLine(const char *line, long length)
{
    return length == 0 || line[0] == '#' || line[0] == '\n';
}

/**
 * Sprawdza, czy wiersz zaczyna się literą.
 * @param[in] line : wiersz
 * @return Czy wiersz zaczyna się literą?
 */
static bool firstLetter(const char *line)
{
    return isalpha(line[0]);
}

/**
 * Sprawdza, czy na początku tablicy znaków jaest poprawna liczba typu long.
 * @param[in] line : tablica znaków
 * @param[in] end : pierwszy znak za ostatnim wczytanym
 * @param[in] value : wczytana liczba
 * @return Czy na początku tablicy znaków jaest poprawna liczba typu long?
 */
static bool isLong(char *line, char **end, poly_coeff_t *value)
{
    if (line[0] == '\n' || line[0] == '\0')
        return false;
    long number;
    errno = 0;
    number = strtol(line, end, 10);
    if (errno != 0)
        return false;
    *value = number;
    return true;
}

/**
 * Sprawdza, czy na początku tablicy znaków jaest poprawna liczba typu unsigned long.
 * @param[in] line : tablica znaków
 * @param[in] end : pierwszy znak za ostatnim wczytanym
 * @param[in] value : wczytana liczba
 * @return Czy na początku tablicy znaków jaest poprawna liczba typu unsigned long?
 */
static bool isUnsignedLong(char *line, char **end, unsigned long *value)
{
    if (line[0] == '\n' || line[0] == '\0')
        return false;
    unsigned long long number;
    errno = 0;
    number = strtoull(line, end, 10);
    if (errno != 0)
        return false;

    *value = number;
    return true;
}


/**
 * Sprawdza czy wszystkie znaki w wierszu są cyframi lub znakiem @f$-$@f$ lub znakiem '\n'.
 * @param[in] line : wiersz @f$p@f$
 * @param[in] length : długość wiersza @f$q@f$
 * @return  wszystkie znaki w @p line są cyframi lub minusem lub znakiem końca linii
 */
static bool AllNumbersOrMinus(const char *line, long length)
{
    for (int i = 0; i < length - 1; ++i)
    {
        if (!isdigit(line[i]) && line[i] != '-' && line[i] != '.')
        {
            return false;
        }
    }
    return isdigit(line[length - 1]) || line[length - 1] == '\n';
}


/**
 * Sprawdza czy wszystkie znaki w wierszu poza ostatnim są cyframi lub znakiem '\n'.
 * @param[in] line : wiersz @f$p@f$
 * @param[in] length : długość wiersza @f$q@f$
 * @return  wszystkie znaki w @p line są cyframi lub znakiem końca linii
 */
static bool AllNumbers(const char *line, long length)
{
    for (int i = 0; i < length - 1; ++i)
    {
        if (!isdigit(line[i]) && line[i] != '.')
        {
            return false;
        }
    }
    return isdigit(line[length - 1]) || line[length - 1] == '\n';
}

/**
 * Sprawdza, czy wiersz jest poprawnym poleceniem AT, jeśli tak, to wczytuje wartość zmiennej do @p value.
 * @param[in] line : wiersz
 * @param[in] length : długość wiersza
 * @param[in] value : wczytana wartość zmiennej
 * @return typ wiersza
 */
static LineType CheckAt(char *line, long length, long *value)
{
    if ((strcmp(line, "AT") == 0 || strcmp(line, "AT\n") == 0))//length of 'DEG_BY x'
        return AT_WRONG_VALUE;
    if (line[AT_LENGTH] != SPACE)
        return WRONG_COMMAND;
    if (length < AT_LENGTH + SPACE_LENGTH + 1)//length of 'AT x'
        return AT_WRONG_VALUE;
    if (!AllNumbersOrMinus(line + AT_LENGTH + SPACE_LENGTH, length - (AT_LENGTH + SPACE_LENGTH)))
        return AT_WRONG_VALUE;
    char *end;
    if (isLong(line + AT_LENGTH + SPACE_LENGTH, &end, value))
    {
        if (*end != '\0' && *end != '\n') //wystąpił błędny znak
            return AT_WRONG_VALUE;
        return AT;
    }
    return AT_WRONG_VALUE;
}

/**
 * Sprawdza, czy wiersz jest poprawnym poleceniem DEG_BY, jeśli tak, to wczytuje wartość zmiennej do @p value.
 * @param[in] line : wiersz
 * @param[in] length : długość wiersza
 * @param[in] value : wczytana wartość zmiennej
 * @return typ wiersza
 */
static LineType CheckDegBy(char *line, long length, unsigned long *value)
{
    if ((strcmp(line, "DEG_BY") == 0 || strcmp(line, "DEG_BY\n") == 0))
        return DEG_BY_WRONG_VARIABLE;
    if (line[DEG_BY_LENGTH] != SPACE)
        return WRONG_COMMAND;
    if (length < DEG_BY_LENGTH + SPACE_LENGTH + 1)//length of 'DEG_BY x'
        return DEG_BY_WRONG_VARIABLE;
    if (!AllNumbers(line + DEG_BY_LENGTH + SPACE_LENGTH, length - (DEG_BY_LENGTH + SPACE_LENGTH)))
        return DEG_BY_WRONG_VARIABLE;
    char *end;
    if (isUnsignedLong(line + DEG_BY_LENGTH + SPACE_LENGTH, &end, value))
    {
        if (*end != '\0' && *end != '\n') //wystąpił błędny znak
            return DEG_BY_WRONG_VARIABLE;
        return DEG_BY;
    }
    return DEG_BY_WRONG_VARIABLE;
}


/**
 * Sprawdza, czy wiersz jest poprawnym poleceniem COMPOSE, jeśli tak, to wczytuje wartość zmiennej do @p value.
 * @param[in] line : wiersz
 * @param[in] length : długość wiersza
 * @param[in] value : wczytana wartość zmiennej
 * @return typ wiersza
 */
static LineType CheckCompose(char *line, long length, unsigned long *value)
{
    if ((strcmp(line, "COMOPSE") == 0 || strcmp(line, "COMPOSE\n") == 0))
        return COMPOSE_WRONG_PARAMETER;
    if (line[COMPOSE_LENGTH] != SPACE)
        return WRONG_COMMAND;
    if (length < COMPOSE_LENGTH + SPACE_LENGTH + 1)//length of 'COMPOSE x'
        return COMPOSE_WRONG_PARAMETER;
    if (!AllNumbers(line + COMPOSE_LENGTH + SPACE_LENGTH, length - (COMPOSE_LENGTH + SPACE_LENGTH)))
        return COMPOSE_WRONG_PARAMETER;
    char *end;
    if (isUnsignedLong(line + COMPOSE_LENGTH + SPACE_LENGTH, &end, value))
    {
        if (*end != '\0' && *end != '\n') //wystąpił błędny znak
            return COMPOSE_WRONG_PARAMETER;
        return COMPOSE;
    }
    return COMPOSE_WRONG_PARAMETER;
}

/**
 * Sprawdza czy każdy znak wiersza jest cyfrą, literą, lub jednym ze znaków: @f$-@f$, @f$+@f$,
 * @f$(@f$, @f$)@f$, @f$_@f$, spacją lub znakiem końca linii.
 * @param[in] line : wiersz @f$p@f$
 * @param[in] length : długość wiersza @f$q@f$
 * @return  każdy znak w @p line jest cyfrą, literą, lub jednym ze znaków: @f$-@f$, @f$+@f$,
 * @f$(@f$, @f$)@f$, @f$_@f$, spacją lub znakiem końca linii
 */
static bool CheckCharacters(const char *line, long length)
{
    for (int i = 0; i < length; ++i)
    {
        if (!isalnum(line[i]) && line[i] != '(' && line[i] != ')' && line[i] != '+' && line[i] != '-' &&
            line[i] != ' ' && line[i] != '\n' && line[i] != '_')
            return false;
    }
    return true;
}

/**
 * Sprawdza, czy wiersz zaczyna się na AT
 * @param[in] line : wiersz
 * @return Czy wiersz zaczyna się na AT?
 */
static bool BeginsWithAt(const char *line)
{
    return line[0] == 'A' && line[1] == 'T';
}

/**
 * Sprawdza, czy wiersz zaczyna się na DEG_BY
 * @param[in] line : wiersz
 * @param[in] length : długość wiersza
 * @return Czy wiersz zaczyna się na DEG_BY?
 */
static bool BeginsWithDegBy(const char *line, long length)
{
    return length >= DEG_BY_LENGTH && line[0] == 'D' && line[1] == 'E' && line[2] == 'G' && line[3] == '_' &&
           line[4] == 'B' &&
           line[5] == 'Y';
}

/**
 * Sprawdza, czy wiersz zaczyna się na COMPOSE
 * @param[in] line : wiersz
 * @param[in] length : długość wiersza
 * @return Czy wiersz zaczyna się na COMPOSE?
 */
static bool BeginsWithCompose(const char *line, long length)
{
    return length >= COMPOSE_LENGTH && line[0] == 'C' && line[1] == 'O' && line[2] == 'M' && line[3] == 'P' &&
           line[4] == 'O' &&
           line[5] == 'S' && line[6] == 'E';
}

/**
 * Określa typ instrukcji w wierszu, jeśli instrukcja to AT lub DEG_BY, nadaje parametrom @p at_val lub @p deg_by_var
 * wczytaną wartość zmiennej.
 * @param[in] line : wiersz
 * @param[in] length : długość wiersza
 * @param[in] variable : parametr polecenia DEG_BY, AT lub COMPOSE lub wielomian
 * @return typ wiersza
 */
static LineType
TypeOfInstruction(char *line, long length, InstructionVar *variable)
{
    if (length < MINIMAL_INSTRUCTION_LENGTH)
        return WRONG_COMMAND;

    if (BeginsWithAt(line))
        return CheckAt(line, length, &variable->at_val);
    if (BeginsWithDegBy(line, length))
        return CheckDegBy(line, length, &variable->deg_by_var);
    if (BeginsWithCompose(line, length))
        return CheckCompose(line, length, &variable->compose_parameter);
    if (!CheckCharacters(line, length)) //sprawdzanie, czy są tylko dozwolone znaki (nie ma np. '\0')
        return WRONG_COMMAND;
    if (strcmp(line, "ADD") == 0 || strcmp(line, "ADD\n") == 0)
        return ADD;
    if (strcmp(line, "IS_ZERO") == 0 || strcmp(line, "IS_ZERO\n") == 0)
        return IS_ZERO;
    if (strcmp(line, "IS_COEFF") == 0 || strcmp(line, "IS_COEFF\n") == 0)
        return IS_COEFF;
    if (strcmp(line, "CLONE") == 0 || strcmp(line, "CLONE\n") == 0)
        return CLONE;
    if (strcmp(line, "MUL") == 0 || strcmp(line, "MUL\n") == 0)
        return MUL;
    if (strcmp(line, "NEG") == 0 || strcmp(line, "NEG\n") == 0)
        return NEG;
    if (strcmp(line, "SUB") == 0 || strcmp(line, "SUB\n") == 0)
        return SUB;
    if (strcmp(line, "DEG") == 0 || strcmp(line, "DEG\n") == 0)
        return DEG;
    if (strcmp(line, "PRINT") == 0 || strcmp(line, "PRINT\n") == 0)
        return PRINT;
    if (strcmp(line, "POP") == 0 || strcmp(line, "POP\n") == 0)
        return POP;
    if (strcmp(line, "ZERO") == 0 || strcmp(line, "ZERO\n") == 0)
        return ZERO;
    if (strcmp(line, "IS_EQ") == 0 || strcmp(line, "IS_EQ\n") == 0)
        return IS_EQ;

    return WRONG_COMMAND;
}


ParseResult ParseLine()
{
    ParseResult result;
    char *string = NULL;
    ssize_t length = read_line_into_string(&string);
    if (length == NO_INPUT)
        result.type = END_OF_FILE;
    else if (ignoredLine(string, length))
        result.type = IGNORED_LINE;
    else if (firstLetter(string))
        result.type = TypeOfInstruction(string, length, &result.variable);
    else
        result.type = GetPoly(string, length, &result.variable.poly);
    free(string);
    return result;
}
