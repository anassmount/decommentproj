/* decomment.c */
/* Anass Mountasser */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

/* enum represents possible states of the DFA */
enum State {
   NORMAL, MAYBE_COMMENT, COMMENT, MAYBE_NORMAL, STRING, CHARACTER,
   ESCAPE_STR, ESCAPE_CHAR };
/* keeps track of the line in which the last opened comment was. If
the commment terminates, it is updated to the line of the comment
end. This number will be used for standard error */
int lineComment = 0;
/* keeps track of the line count, and is incremented after every
      sighting of new line */
   int line = 1;
/* Normal State, is the starting state, and is accepting */
enum State handleNormal(int c) {
   enum State state;
   if(c == '/')
      state = MAYBE_COMMENT;
   else if(c == '\"') {
      state = STRING;
      putchar(c);
   }
   else if(c == '\'') {
      state = CHARACTER;
      putchar(c);
   }
   else if(c == '\n') {
      state = NORMAL;
      putchar(c);
      line++;
   }
   else{
      state = NORMAL;;
      putchar(c);
   }
   return state;
}
/* If a / is encountered, it may be a comment, so this state checks to
   be sure. It is an accepting state. */
enum State handleMaybeComment(int c) {
   enum State state;
   if(c == '*') {
      lineComment = line;
      state = COMMENT;
      putchar(' ');
   }
   else if(c == '/') {
      state = MAYBE_COMMENT;
      putchar(c);
   }
   else if(c == '\n') {
      state = NORMAL;
      putchar('/');
      putchar(c);
      line++;
   }
   else if(c == '\"') {
      state = STRING;
      putchar('/');
      putchar(c);
   }
   else if(c == '\'') {
      state = CHARACTER;
      putchar('/');
      putchar(c);
   }
   else{
      state = NORMAL;
      putchar('/');
      putchar(c);
   }
   return state;
}
/* This state governs the situation that the situation is in a comment
, and is in a rejecting state */
enum State handleComment(int c) {
   enum State state;
   if(c == '*') {
      state = MAYBE_NORMAL;
   }
   else if(c == '\n') {
      state = COMMENT;
      putchar(c);
      line++;
   }
   else{
      state = COMMENT;
   }
   return state;
}
/* If a * is encountered while within a comment, it may be the end of
 said comment, so this state checks to be sure. It is a rejecting 
state. */
enum State handleMaybeNormal(int c) {
   enum State state;
   if(c == '/') {
      lineComment = line;
      state = NORMAL;
   }
   else if(c == '*') {
      state = MAYBE_NORMAL;
   }
   else if(c == '\n') {
      state = COMMENT;
      putchar(c);
      line++;
   }
   else{
      state = COMMENT;
   }
   return state;
}
/* This case handles when a string literal is being processed. 
Everything is printed, and escape characters are handled
so that the character after it has no purpose. It is an
accepting state  */
enum State handleString(int c) {
   enum State state;
   if(c == '\"') {
      state = NORMAL;
      putchar(c);
   }
   else if(c == '\n') {
      state = STRING;
      putchar(c);
      line++;
   }
   else if(c == '\\') {
      state = ESCAPE_STR;
   }
   else{
      state = STRING;
      putchar(c);
   }
   return state;
}
/* This case handles when a character literal is being processed. 
Everything is printed, and escape characters are handled
so that the character after it has no purpose It is an 
accepting state  */
enum State handleCharacter(int c) {
   enum State state;
   if(c == '\'') {
      state = NORMAL;
      putchar(c);
   }
   else if(c == '\n') {
      state = CHARACTER;
      putchar(c);
      line++;
   }
   else if(c == '\\') {
      state = ESCAPE_CHAR;
   }
   else{
      state = CHARACTER;
      putchar(c);
   }
   return state;
}
/* This case handles when inside a string literal and an escape 
character was jsut encountered. Prints the escape character,
and the character following it, and doesn't worry about
newline as per our assumptions. It is an accepting state. */
enum State handleEscapeStr(int c) {
   enum State state;
   putchar('\\');
   putchar(c);
   state = STRING;
   return state;
}
/* This case handles when inside a character literal and an escape 
character was jsut encountered. Prints the escape character,
and the character following it, and doesn't worry about
newline as per our assumptions. It is an accepting state. */
enum State handleEscapeChar(int c) {
   enum State state;
   putchar('\\');
   putchar(c);
   state = CHARACTER;
   return state;
}

/* Will check each character, and if it not EOF, will be put through
the states in the DFA. If in the end it is in rejecting state, it 
will print to stderr accordingly */
int main(void) {
   /* keeps track of current character */
   int c = 0;
   /* current state */
   enum State current = NORMAL;
   while((c = getchar()) != EOF) {
      switch(current) {
         case NORMAL:
            current = handleNormal(c);
            break;
         case MAYBE_COMMENT:
            current = handleMaybeComment(c);
            break;
         case COMMENT:
            current = handleComment(c);
            break;
         case MAYBE_NORMAL:
            current = handleMaybeNormal(c);
            break;
         case STRING:
            current = handleString(c);
            break;
         case CHARACTER:
            current = handleCharacter(c);
            break;
         case ESCAPE_STR:
            current = handleEscapeStr(c);
            break;
         case ESCAPE_CHAR:
            current = handleEscapeChar(c);
            break;
      }
   }
   /* If in this state, that means that a backslash was not printed 
      when it was supposed to be printed */
   if ((current == ESCAPE_STR) ||
       (current == ESCAPE_CHAR))
      putchar('\\');
   if (current == MAYBE_COMMENT)
      putchar('/');
   /* State is rejecting if it is in a comment, and returns the line
number of the last opened comment */
   if ((current == COMMENT) || (current == MAYBE_NORMAL)) {
      fprintf(stderr, "Error: line %d: unterminated comment\n",
              lineComment);
      exit(EXIT_FAILURE);
   }
   else
      exit(EXIT_SUCCESS);
   return 0;
}
