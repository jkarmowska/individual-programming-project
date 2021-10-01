#!/bin/bash

if (($# != 2)); then
    echo "Wrong number of arguments. Pass exactly two arguments. First must be an executable file and second a directory." >&2
    exit 1
fi

if [ ! -x "$1" ] || [ ! -d "$2" ]; then
    if [ ! -x "$1" ]; then
        echo "File not found or not executable. First argument must be an executable file." >&2
    fi
    if [ ! -d "$2" ]; then
        echo "Directory not found. Second argument must be a path to directory." >&2
    fi
    exit 1;
fi


#Getting full path to executable file to be able to run it if it is in a different directory.
PROG_PATH="$(cd "$(dirname "$1")" && pwd)/$(basename "$1")"
DIRECTORY=$2

FLAGS="--error-exitcode=123 --leak-check=full --show-leak-kinds=all --errors-for-leak-kinds=all -q"

for f in "$DIRECTORY"/*.in; do

    OUT=$(mktemp)
    ERR=$(mktemp)

    #Running the program with valgrind to detect memory leaks.
    valgrind $FLAGS "$PROG_PATH" <"$f" 1>$OUT 2>$ERR
    VALGRIND_EXIT=$?

    if [ $VALGRIND_EXIT -ne 0 ]; then
        echo "Problem with memory."

    else
        #If no memory leaks were detected, running the program again without valgrind, to compare results.
        #Rerunning is necessary as valgrind does not support 80 bit arithmetic so the results may differ.
        "$PROG_PATH" <"$f" 1>$OUT 2>$ERR

        diff "${f%in}"out $OUT >/dev/null
        OUT_EXIT=$?

        diff "${f%in}"err $ERR >/dev/null
        ERR_EXIT=$?

        echo -n "${f#$DIRECTORY/}: "
        if [ $ERR_EXIT -eq 0 ] && [ $OUT_EXIT -eq 0 ]; then
            echo "SUCCESS!"

        else
            if [ $OUT_EXIT -ne 0 ]; then
            echo "Wrong output."
            fi

            if [ $ERR_EXIT -ne 0 ]; then
            echo "Wrong diagnostic output."
            fi
        fi
    fi

    rm $OUT $ERR

done
