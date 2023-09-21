#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

/* All needed DFA states to execute decomment functionality according to
program specs.*/
enum Statetype {START, SN_QUOTES, DB_QUOTES, SN_QUOTE_ESC, DB_QUOTE_ESC, 
ONE_SLASH, ONE_STAR, TWO_STAR};

/*All state functions below takes in a character from Standard Input Stream
in main and makes a sequence of logical decisions about state transitions. */

/* The default starting state of the DFA, for when no special characters 
are seen. This is also for when the "special-ness" from previously seen
special states has been reset. This function takes in a char (represented as
an int) and decides whether there is a special state to transition to. */
enum Statetype handleStartState (int c) {
    enum Statetype state;
    /* single quote char to go into single quotes state */
    if (c == '\'') {
        putchar(c);
        state = SN_QUOTES;
    }
    /* double quote char to go into double quotes state */
    else if (c == '"') {
        putchar(c);
        state = DB_QUOTES;
    }
    /* slash to go into one-slash state */
    else if (c == '/') {
        state = ONE_SLASH;
    }
    /* all other chars */
    else {
        putchar(c);
        state = START;
    }
    return state;
}

/* This is the state for when a single quote has been opened. The quote may
or may not get closed. This function takes in a char (represented as an int)
and handles: regular characters inside the quote, the completion of the 
quote, and escape characters. */
enum Statetype handleSnQuotesState (int c) {
    enum Statetype state;
    putchar(c);
    /* for special escape character */
    if (c == '\\') {
        state = SN_QUOTE_ESC;
    }
    /* single quote char indicating completion of a single quote */
    else if (c == '\'') {
        state = START;
    }
    /* all other chars */
    else {
        state = SN_QUOTES;
    }
    return state;
}

/* This is the state for when there was an escape character seen since the
starting of a single quote. Regardless what char (represented as an int) is
passed in, it'll go directly back to the single quote state because the 
escape slash only applies to the character directly after it. */
enum Statetype handleSnQuoteEscState (int c) {
    enum Statetype state;
    putchar(c);
    state = SN_QUOTES;
    return state;
}

/* This is the state for when a double quote has been opened. The quote may
or may not get closed. This function takes in a char (represented as an int)
and handles: regular characters inside the quote, the completion of the 
quote, and escape characters. */
enum Statetype handleDbQuotesState (int c) {
    enum Statetype state;
    putchar(c);
    /* double quote or single quote char*/
    if (c == '\\') {
        state = DB_QUOTE_ESC;
    }
    else if (c == '"') {
        state = START;
    }
    else {
        state = DB_QUOTES;
    }
    return state;
}

/* This is the state for when there was an escape character seen since the
starting of a double quote. Regardless what char (represented as an int) is
passed in, it'll go directly back to the single quote state because the 
escape slash only applies to the character directly after it. */
enum Statetype handleDbQuoteEscState (int c) {
    enum Statetype state;
    putchar(c);
    state = DB_QUOTES;
    return state;
}

/* This is the state for when a comment may be beginning. This function takes
in a char (represented as an int). The DFA then makes decisions about the
next states it must transition to. */
enum Statetype handleOneSlashState (int c) {
    enum Statetype state;
    /* for star */
    if (c == '*') {
        /* DFA always prints space when a comment has begun */
        putchar(' ');
        state = ONE_STAR;
    }
    /* for slash */
    else if (c == '/') {
        putchar(c);
        state = ONE_SLASH;
    }
    /* all other characters */
    else {
        /* retroactively print / character*/
        putchar('/');
        putchar(c);
        /* go back to quotes state if needed */
        if (c == '"') {
            state = DB_QUOTES;
        }
        else if (c == '\'') {
            state = SN_QUOTES;
        }
        else {
            state = START;
        }
    }
    return state;
}

/* This state represents when the DFA is in a comment. It takes in a char
(represented as an int) and makes state transitions based on the DFA logic. */
enum Statetype handleOneStarState (int c) {
    enum Statetype state;
    /* for star */
    if (c == '*') {
        state = TWO_STAR;
    }
    /* for nl character */
    else if (c == '\n') {
        putchar(c);
        state = ONE_STAR;
    }
    /* all other characters */
    else {
        state = ONE_STAR;
    }
    return state;
}

/* This state represents when a comment might be closed. It takes in a char
(represented as an int) and makes state transitions based on the DFA logic. */
enum Statetype handleTwoStarState (int c) {
    enum Statetype state;
    /* for slash */
    if (c == '/') {
        state = START;
    }
    /* for star */
    else if (c == '*') {
        state = TWO_STAR;
    }
    /* for all other chars */
    else {
        /* for newline */
        if (c == '\n') {
            putchar(c);
        }
        state = ONE_STAR;
    }
    return state;
}

/* This is the main method for decommenting. There are no arguments that are
passed in, but this method reads from Std In. For an infinitely long input
passed through Std In, this method executes state transitions based on
decommenting DFA logic and handles line counting to print the correct 
line number if an error arises. This method writes to Std Out. */
int main (void) {
    int c; /* character read through Std In in int form */
    int lineCount = 1; /* count total lines of input */
    int commLine = 0; /* count number of lines in a given comment */
    enum Statetype state = START; /* establish start state */

    /* Loop to continue reading in characters from Std In as long as the 
    End of File has not been read */
    while ((c = getchar()) != EOF) {
        /* count lines of input */
        if (c == '\n') {
            lineCount++;
        }
        /* handle state transitions */
        switch (state){
            case START:
                state = handleStartState(c);
                /* If no longer in a comment, comment line counter variable
                must be reset */
                if (c == '\n') {
                    commLine = 0;
                }
                break;
            case SN_QUOTES:
                state = handleSnQuotesState(c);
                break;
            case SN_QUOTE_ESC:
                state = handleSnQuoteEscState(c);
                break;
            case DB_QUOTES:
                state = handleDbQuotesState(c);
                break;
            case DB_QUOTE_ESC:
                state = handleDbQuoteEscState(c);
                break;
            case ONE_SLASH:
                state = handleOneSlashState(c);
                break;
            case ONE_STAR:
                state = handleOneStarState(c);
                if (c == '\n') {
                    commLine++;
                }
                break;
            case TWO_STAR:
                state = handleTwoStarState(c);
                if (c == '\n') {
                    commLine++;
                }
                break;
        }
    }
    /* Handles End Of File situations, prints additional characters
    and errors as needed, and reports when DFA ends in a failed state. */
    if (state == ONE_SLASH) {
        putchar('/');
        return 0;
    }
    else if (state == ONE_STAR) {
        fprintf(stderr, "Error: line %d: unterminated comment\n",
        lineCount - commLine);
        return EXIT_FAILURE;
    }
    else if (state == TWO_STAR) {
        fprintf(stderr, "Error: line %d: unterminated comment\n",
        lineCount - commLine);
        return EXIT_FAILURE;
    }
    /* Report when DFA ends in an accepting state. */
    return 0;
}