# TextJedi-Interpreter
An interpreter for the made-up language TextJedi.

# TextJedi Language Overview

TextJedi Language is a programming language designed primarily for editing text files using cutting and pasting operations. This language supports only two data types and includes various operations and built-in functions tailored to text manipulation.

## Data Types

- Strings (text): Represented by double quotes, e.g., "Hello World".
- Integers: Natural numbers (zero or positive whole numbers).

Variables must be declared before use using the 'new' keyword:

    new text myText;
    new int myInt; /* This is a comment */

## Assignment, Operations, and Functions

- The assignment operator is ':='.
- Arithmetic expressions use infix notation with '+' (addition) and '-' (subtraction) operators.
  
Examples:
    myText := "Hello world";
    myInt := 0;
    myInt := 2 + yourInt;
    myText := FirstText + SecondText;

### Special Operations

- Addition on texts concatenates two strings.
  
      s := "Hello" + " world"; // Assigns "Hello world" to s

- Subtraction on texts removes the first occurrence of the subtrahend from the minuend.
  
      myString := "Hello world" - "wo"; // Assigns "Hello rld" to myString

### Functions

- size(myText): Returns the size of myText as an integer.
- subs(myText, begin, end): Returns a substring of myText from begin to end.
- locate(bigText, smallText, start): Returns the position of the first occurrence of smallText in bigText starting from start.
- insert(myText, location, insertText): Inserts insertText into myText at location.
- override(myText, location, ovrText): Overwrites myText at location with ovrText.
- asString(myInt): Converts an integer to a string.
- asText(myString): Converts a string to an integer.

## Commands

    read myString from myTextFile; // Reads from myTextFile.txt
    write myText to yourTextFile; // Writes myText to yourTextFile
    input myText prompt promptText; // Inputs from keyboard
    output myText; // Displays myText on screen

## Lexical Rules

1. Identifiers:
   - Max size: 30 characters.
   - Must start with a letter and may include digits and underscores.
   - Case sensitive.

2. Integer constants:
   - Max size: equivalent to a C unsigned int.
   - Only zero or positive values.

3. Operators: '+', '-', ':='.

4. String constants:
   - Delimited by double quotes.
   - Unlimited size but cannot contain double quotes.

5. Keywords:
   - new, int, text, size, subs, locate, insert, override, read, write, from, to, input, output, asText, asString.

6. End of Line Mark: ';'.

7. Comments:
   - Anything between /* and */.
   - Must terminate before file end.
