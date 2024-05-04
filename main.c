#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#define MAX_IDENTIFIER_LENGTH 30
#define INITIAL_VALUE_SIZE 1000

char* intToString(int number) {     // convert int to string
    int length = snprintf(NULL, 0, "%d", number);
    char* str = (char*) malloc((length + 1) * sizeof(char));
    sprintf(str, "%d", number);
    return str;
}


char* bracket(char ch){     // bracket control
    if(ch == '('){
        return("LeftPar");
    }
    else if(ch == ')'){
        return("RightPar");
    }
    else{
        return 0;
    }
}
// Function that check if identifier is keyword or not
int is_keyword(char buffer[]) {
    char keywords[16][10] = {"new", "int", "text", "size", "subs", "locate", "insert",
                             "override", "read", "write", "from", "to", "input",
                             "output", "asText", "asString"};

    for (int i = 0; i < 16; ++i) {  // compare all keywords with given string
        if (strcmp(keywords[i], buffer) == 0) {
            return 1;   // keyword
        }
    }
    return 0;   // not keyword, it is normal identifier
}


// Token types
typedef enum {
    INTCONST,
    TEXT,
    OPERATOR,
    IDENTIFIER,
    KEYWORD,
    END_OF_LINE,
    COMMA,
    BRACKET,
} TokenType;

// Token structure
typedef struct {
    TokenType type;
    char *value;    // VALUE OR NAME BASED ON THE TYPE
} Token;

// Variable structure
typedef struct {
    char *name;     // NAME
    TokenType type;
    char *value;   // VALUE;
    int isAssigned;
} Variable;

int tokenSize = 0;
int varSize = 0;
// Token array and size
Token *tokens;
// Variable array and size
Variable *variables;

void addToken(TokenType type, char *value) {
    // Memory allocation for tokens
    tokenSize++;
    tokens = realloc(tokens, tokenSize * sizeof(Token));
    // Add new token
    tokens[tokenSize - 1].type = type;
    tokens[tokenSize - 1].value = malloc(strlen(value) + 1);
    strcpy(tokens[tokenSize - 1].value, value);
}

void addVariable(char *name, TokenType type) {
    // Check if variable is already defined
    for (int i = 0; i < varSize; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            printf("Variable %s is already defined!\n", name);
            exit(1);
        }
    }

    // Add new variable
    variables = realloc(variables, (varSize + 1) * sizeof(Variable));
    variables[varSize].name = malloc(strlen(name) + 1);
    strcpy(variables[varSize].name, name);
    variables[varSize].type = type;
    // Allocate memory for value and initialize with empty string
    variables[varSize].value = malloc(INITIAL_VALUE_SIZE * sizeof(char));
    strcpy(variables[varSize].value, "");  // Initialize with empty string
    variables[varSize].isAssigned = 0; // identifier declared but not assigned yet.
    varSize++;
}

int isVariableExist(char *name){    // check whether variable is exit in variables array. isDeclared?
    for (int i = 0; i < varSize; i++) {
        if (strcmp(variables[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void parse() {
    for (int i = 0; i < tokenSize; i++) {
        Token currentToken = tokens[i];
        // Check syntax based on the token type
        switch (currentToken.type) {
            case KEYWORD:
                if (strcmp(currentToken.value, "new") == 0) { //If the first token of the line is "new":
                    // Next token should be a data type
                    if (tokens[i + 1].type != KEYWORD ||
                        (strcmp(tokens[i + 1].value, "text") != 0 && strcmp(tokens[i + 1].value, "int") != 0)) {
                        printf("Expected data type such as int or text after 'new', but got '%s'\n", tokens[i + 1].value);
                        exit(1);
                    }

                    // Following token should be an identifier
                    if (tokens[i + 2].type != IDENTIFIER) {
                        printf("Expected an identifier after data type, but got '%s'\n", tokens[i + 2].value);
                        exit(1);
                    }

                    // Then, the line should end
                    if (tokens[i + 3].type != END_OF_LINE) {
                        printf("Expected end of line after declaration, but got '%s'\n", tokens[i + 3].value);
                        exit(1);
                    }

                    // Add the new variable
                    if (tokens[i + 1].type == KEYWORD && strcmp(tokens[i + 1].value, "text") == 0) {
                        addVariable(tokens[i + 2].value, TEXT);
                    } else {
                        addVariable(tokens[i + 2].value, INTCONST);
                    }
                    i += 3;  // Skip to the next line
                    continue;
                }
                else if(strcmp(currentToken.value, "read") == 0){
                    if(tokens[i+1].type != IDENTIFIER){
                        printf("Identifier expected after read keyword.");
                        exit(1);
                    }
                    int index = isVariableExist(tokens[i+1].value);     // tokens.value -->name (for identifier)
                    if(index == -1){
                        printf("Variable is not declared %s\n",tokens[i+1].value);
                        exit(1);
                    }
                    if(variables[index].type != TEXT){
                        printf("Read command's argument cannot be int type %s\n",tokens[i+1].value);
                        exit(1);
                    }

                    if(tokens[i + 2].type != KEYWORD || strcmp(tokens[i + 2].value, "from") != 0){
                        printf("from keyword expected after identifier, but got '%s' \n", tokens[i + 2].value);
                        exit(1);
                    }
                    if(tokens[i+3].type != IDENTIFIER) {
                        printf("txt name as Identifier expected as a filename, but can't find after 'read' keyword.\n");
                        exit(1);
                    }
                    if(tokens[i+4].type != END_OF_LINE){
                        printf("EndOfLine character is missing\n");
                        exit(1);
                    }
                    char *filename = tokens[i+3].value;
                    strcat(filename, ".txt");
                    FILE *file;
                    char character;
                    char *buffer = NULL;
                    size_t buffer_size = 0;
                    size_t string_length = 0;

                    file = fopen(filename, "r"); // Open the file to read


                    if (file == NULL) {
                        printf("Failed to open the file the text file to read.\n");
                        exit(1);
                    }

                    // Read and print each character in the file until reaching the end
                    while ((character = fgetc(file)) != EOF) {
                        if (string_length + 1 >= buffer_size) {
                            buffer_size += 100; // increase buffer size if it is not enough.
                            buffer = (char *)realloc(buffer, buffer_size * sizeof(char));
                            if (buffer == NULL) {
                                printf("Memory error.\n");
                                exit(1);
                            }
                        }
                        buffer[string_length++] = character;
                    }
                    buffer[string_length] = '\0';
                    strcpy(variables[index].value, buffer);
                    fclose(file);
                    free(buffer);
                    variables[index].isAssigned = 1;
                    i += 4;
                    continue;
                }

                else if(strcmp(currentToken.value, "write") == 0){
                    char* writeValue = NULL;
                    if (!(tokens[i+1].type == IDENTIFIER || tokens[i+1].type == TEXT)){
                        printf(" String Identifier or Text expected after write keyword.");
                        exit(1);
                    }

                    if(tokens[i+1].type == IDENTIFIER){
                        int index = isVariableExist(tokens[i+1].value);
                        if(index == -1){
                            printf("Variable is not declared %s\n",tokens[i+1].value);
                            exit(1);
                        }
                        if(variables[index].type != TEXT){
                            printf("Error! You can only use Text type variable with write command");
                            exit(1);
                        }
                        if(variables[index].isAssigned == 0){
                            printf("Error! You are trying to write non-assigned variable on txt file. ");
                            exit(1);
                        }
                        writeValue = malloc(strlen(variables[index].value) + 1); // +1 for the null-terminator
                        if(writeValue == NULL) {
                            printf("Error: Unable to allocate memory for writeValue.\n");
                            exit(1);
                        }
                        strcpy(writeValue, variables[index].value);
                    }

                    if(tokens[i+1].type == TEXT){
                        writeValue = malloc(strlen(tokens[i+1].value) + 1); // +1 for the null-terminator
                        if(writeValue == NULL) {
                            printf("Error: Unable to allocate memory for writeValue.\n");
                            exit(1);
                        }
                        strcpy(writeValue, tokens[i+1].value);
                    }

                    if(tokens[i+2].type != KEYWORD || strcmp(tokens[i + 2].value, "to") != 0){
                        printf("'to' keyword expected.");
                        exit(1);
                    }
                    if(tokens[i+3].type != IDENTIFIER){
                        printf("Identifier expected as txt file.");
                        exit(1);
                    }
                    if(tokens[i+4].type != END_OF_LINE){
                        printf("End Of Line character expected.");
                        exit(1);
                    }
                    char *filename = tokens[i+3].value;
                    strcat(filename, ".txt");

                    FILE *file = fopen(filename, "w");
                    if (file == NULL) {
                        fprintf(stderr, "Failed to open file \"%s\"\n", filename);
                        exit(EXIT_FAILURE);
                    }
                    fputs(writeValue, file);

                    fclose(file);
                    free(writeValue); // Free the memory allocated for writeValue
                    i += 4;
                    continue;
                }

                else if(strcmp(currentToken.value, "input") == 0){

                    if (tokens[i+1].type != IDENTIFIER){
                        printf("Error, identifier expected after input keyword.");
                        exit(1);
                    }
                    int index = isVariableExist(tokens[i + 1].value);
                    if (index == -1){
                        printf("Error, identifier %s was not declared!", tokens[i+1].value);
                        exit(1);
                    }
                    if(variables[index].type != TEXT){
                        printf("Error, text type argument needed for input command %s\n",tokens[i+1].value);
                        exit(1);
                    }

                    if(tokens[i+2].type != KEYWORD && strcmp(tokens[i + 2].value, "prompt") != 0){
                        printf("'prompt' keyword expected.");
                        exit(1);
                    }
                    if(tokens[i+3].type != IDENTIFIER && tokens[i+3].type != TEXT){
                        printf("Identifier or text expected as prompt text.");
                        exit(1);
                    }
                    if(tokens[i+3].type == IDENTIFIER){
                        int indexPrompt = isVariableExist(tokens[i+3].value);
                        if(indexPrompt == -1){
                            printf("Variable is not declared %s\n",tokens[i+3].value);
                            exit(1);
                        }
                        if(variables[indexPrompt].type != TEXT){
                            printf("Error! You can only use Text type variable for prompt");
                            exit(1);
                        }
                        if(variables[indexPrompt].isAssigned == 0){
                            printf("Error! You are trying to prompt user null text.");
                            exit(1);
                        }
                        printf("%s", variables[indexPrompt].value);  // print prompt to user to display that we want user to enter something from keyboard
                    }

                    if(tokens[i+3].type == TEXT){
                        printf("%s", tokens[i+3].value);  // print prompt to user to display that we want user to enter something from keyboard
                    }

                    int MAX_INPUT_LENGTH = 100;
                    char inputString[MAX_INPUT_LENGTH + 1];
                    int overflow = 0;
                    char ch;

                    // Read characters into inputString
                    for (int i = 0; i <= MAX_INPUT_LENGTH; i++) {
                        ch = getc(stdin);
                        if (ch == '\n' || ch == EOF) {
                            inputString[i] = '\0';
                            break;
                        } else if (i == MAX_INPUT_LENGTH) {
                            overflow = 1;
                        } else {
                            inputString[i] = ch;
                        }
                    }

                    // If too many characters were entered, throw an error and consume the rest
                    if (overflow) {
                        while ((ch = getc(stdin)) != '\n' && ch != EOF);
                        printf("Input size error: too many characters. Maximum keyboard input length is 100.\n");
                        exit(1);
                    }

                    // Allocate enough memory for variables[index].value to hold inputString, and copy the string
                    variables[index].value = malloc((strlen(inputString) + 1) * sizeof(char));
                    if (variables[index].value == NULL) {
                        printf("Error: Unable to allocate memory.\n");
                        exit(1);
                    }
                    strcpy(variables[index].value, inputString);

                    i += 4;
                    variables[index].isAssigned = 1;
                    continue;
                }

                else if(strcmp(currentToken.value, "output") == 0){
                    if (tokens[i+1].type != IDENTIFIER && tokens[i+1].type != TEXT){
                        printf("Error, identifier or text expected after output keyword.");
                        exit(1);
                    }
                    if(tokens[i+2].type != END_OF_LINE){
                        printf("Error, EndOfLine character is missing.");
                        exit(1);
                    }

                    if(tokens[i+1].type == IDENTIFIER){
                        int index = isVariableExist(tokens[i + 1].value);
                        if (index == -1){
                            printf("Error, identifier %s was not declared!", tokens[i+1].value);
                            exit(1);
                        }
                        if(variables[index].type != TEXT){
                            printf("Text type argument needed for output command");
                            exit(1);
                        }
                        if(variables[index].isAssigned == 0){
                            printf("Error, identifier %s was not assigned!", tokens[i+1].value);
                            exit(1);
                        }
                        printf("%s\n",variables[index].value);
                    }

                    if(tokens[i+1].type == TEXT){
                        printf("%s\n",tokens[i+1].value);
                    }
                    i += 2;
                    continue;
                }

                break;
            case IDENTIFIER: // IDENTIFIER :=
                if(1){

                }
                int index = isVariableExist(tokens[i].value);
                if (index == -1){
                    printf("Error, identifier %s was not declared!", tokens[i].value);
                    exit(1);
                }

                if (tokens[i + 1].type != OPERATOR || strcmp(tokens[i + 1].value, ":=") != 0) {
                    printf("Expected ':=' after identifier, but got '%s'\n", tokens[i + 1].value);
                    exit(1);
                }

                // Following token should be an identifier, integer constant, or text
                if (tokens[i + 2].type != IDENTIFIER && tokens[i + 2].type != INTCONST && tokens[i + 2].type != TEXT && tokens[i + 2].type != KEYWORD) {
                    printf("Expected an identifier, integer constant, or text after ':=', but got '%s'\n", tokens[i + 2].value);
                    exit(1);
                }
                // a := 2;
                if(tokens[i + 2].type == INTCONST && tokens[i + 3].type == END_OF_LINE){    // only one operand
                    if(variables[index].type != INTCONST){
                        printf("ERROR, Type of the identifier and the value that you are trying to assign is not the same. ");
                        exit(1);
                    }
                    variables[index].value = malloc(strlen(tokens[i + 2].value) + 1); // +1 null-terminating character için
                    if (variables[index].value != NULL) {
                        strcpy(variables[index].value, tokens[i + 2].value);
                    }
                    i += 3;
                    variables[index].isAssigned = 1;
                    continue;
                }
                // a := "b";
                if(tokens[i + 2].type == TEXT && tokens[i + 3].type == END_OF_LINE){   // only one operand
                    if(variables[index].type != TEXT){
                        printf("ERROR, Type of the identifier and the value that you are trying to assign is not the same. ");
                        exit(1);
                    }
                    variables[index].value = malloc(strlen(tokens[i + 2].value) + 1); // +1 null-terminating character için
                    if (variables[index].value != NULL) {
                        strcpy(variables[index].value, tokens[i + 2].value);
                    }
                    i += 3;
                    variables[index].isAssigned = 1;
                    continue;
                }
                // a := b;
                if(tokens[i + 2].type == IDENTIFIER && tokens[i + 3].type == END_OF_LINE){   // only one operand

                    int operandIndex = isVariableExist(tokens[i + 2].value);
                    if (operandIndex == -1){
                        printf("Operand that you are trying to use was not declared! ");
                        exit(1);
                    }
                    if(variables[operandIndex].isAssigned == 0){    //sdasdasdsad
                        printf("Error, operand identifier was not assigned yet.");
                        exit(1);
                    }

                    if(variables[operandIndex].type != variables[index].type){
                        printf("ERROR, Type of the identifier and the value that you are trying to assign is not the same. ");
                        exit(1);
                    }
                    variables[index].value = malloc(strlen(variables[operandIndex].value) + 1); // +1 null-terminating character için
                    if (variables[index].value != NULL) {
                        strcpy(variables[index].value, variables[operandIndex].value);
                    }
                    variables[index].isAssigned = 1;
                    i += 3;
                    continue;
                }

                // 3 + 3
                if(tokens[i+2].type == INTCONST && tokens[i+3].type == OPERATOR && tokens[i+4].type == INTCONST && tokens[i+5].type == END_OF_LINE){
                    if(tokens[i+2].type != variables[index].type){
                        printf("ERROR, Type of the identifier and the value that you are trying to assign is not the same. ");
                        exit(1);
                    }
                    if(strcmp(tokens[i+3].value, "+") == 0){
                        int sum = atoi(tokens[i+2].value) + atoi(tokens[i+4].value);

                        char* sumStr = intToString(sum);
                        variables[index].value = realloc(variables[index].value, strlen(sumStr) + 1);
                        if(variables[index].value == NULL) {
                            printf("Error: Unable to allocate memory.\n");
                            exit(1);
                        }
                        strcpy(variables[index].value, sumStr);
                        free(sumStr);  // Free memory if intToString allocated it
                    }
                    if(strcmp(tokens[i+3].value, "-") == 0){
                        if(atoi(tokens[i+2].value) < atoi(tokens[i+4].value)){
                            printf("Error, our language don't support negative values, so you cant subtract a larger value from a smaller value ");
                            exit(1);
                        }
                        int diff = atoi(tokens[i+2].value) - atoi(tokens[i+4].value);

                        char* diffStr = intToString(diff);
                        variables[index].value = realloc(variables[index].value, strlen(diffStr) + 1);
                        if(variables[index].value == NULL) {
                            printf("Error: Unable to allocate memory.\n");
                            exit(1);
                        }
                        strcpy(variables[index].value, diffStr);
                        free(diffStr);  // Free memory if intToString allocated it
                    }
                    variables[index].isAssigned = 1;
                    i += 5;
                    continue;
                }

                // "asdasd" + "asdasd"
                if(tokens[i+2].type == TEXT && tokens[i+3].type == OPERATOR && tokens[i+4].type == TEXT && tokens[i+5].type == END_OF_LINE){
                    if(strcmp(tokens[i+3].value, ":=") == 0){
                        printf("you can only use + or - operator with expression. ");
                        exit(1);
                    }
                    if(tokens[i+2].type != variables[index].type){
                        printf("ERROR, Type of the identifier and the value that you are trying to assign is not the same. ");
                        exit(1);
                    }
                    if(strcmp(tokens[i+3].value, "+") == 0){
                        size_t len1 = strlen(tokens[i+2].value);
                        size_t len2 = strlen(tokens[i+4].value);
                        char *result = malloc((len1 + len2 + 1) * sizeof(char)); // +1 for the null-terminator
                        strcpy(result, tokens[i+2].value);
                        strcat(result, tokens[i+4].value);

                        variables[index].value = realloc(variables[index].value, strlen(result) + 1);
                        strcpy(variables[index].value, result);

                        free(result);
                    }
                    if(strcmp(tokens[i+3].value, "-") == 0){
                        if(strlen(tokens[i+4].value) > strlen(tokens[i+2].value)) {   // if str2(desired string) is longer than str1(main string) give an error message.
                            fprintf(stderr, "Error: Desired string is longer than the Main string\n");
                            exit(EXIT_FAILURE);
                        }

                        char *result = strdup(tokens[i+2].value); // Duplicate str1

                        char *subStrPos = strstr(result, tokens[i+4].value);

                        // Check if str2 is a substring of str1
                        if(subStrPos) {
                            size_t len = strlen(tokens[i+4].value);
                            memmove(subStrPos, subStrPos + len, 1 + strlen(subStrPos + len));
                        }

                        variables[index].value = realloc(variables[index].value, strlen(result) + 1);
                        strcpy(variables[index].value, result);
                        free(result);
                    }

                    variables[index].isAssigned = 1;
                    i += 5;
                    continue;
                }
                // myInt + 5;
                if(tokens[i+2].type == IDENTIFIER && tokens[i+3].type == OPERATOR && tokens[i+4].type == INTCONST && tokens[i+5].type == END_OF_LINE){
                    int operandIndex = isVariableExist(tokens[i + 2].value);
                    if (operandIndex == -1){
                        printf("Operand that you are trying to use was not declared! ");
                        exit(1);
                    }
                    if(variables[operandIndex].isAssigned == 0){
                        printf("Operand that you are trying to use was not assigned! ");
                        exit(1);
                    }
                    if(variables[index].type != INTCONST || variables[operandIndex].type != INTCONST){
                        printf("ERROR, Type of the identifier and the value that you are trying to assign is not the same. ");
                        exit(1);
                    }

                    char *result;

                    if(strcmp(tokens[i+3].value, "+") == 0){
                        int sum = atoi(variables[operandIndex].value) + atoi(tokens[i+4].value);
                        result = intToString(sum);
                        variables[index].value = realloc(variables[index].value, strlen(result) + 1);
                        strcpy(variables[index].value, result);
                    }
                    if(strcmp(tokens[i+3].value, "-") == 0){
                        if(atoi(variables[operandIndex].value) < atoi(tokens[i+4].value)){
                            printf("Error, our language don't support negative values, so you cant subtract a larger value from a smaller value ");
                            exit(1);
                        }
                        int diff = atoi(variables[operandIndex].value) - atoi(tokens[i+4].value);
                        result = intToString(diff);
                        variables[index].value = realloc(variables[index].value, strlen(result) + 1);
                        strcpy(variables[index].value, result);
                    }
                    if(strcmp(tokens[i+3].value, ":=") == 0){
                        printf("you can only use + or - operator with expression. ");
                        exit(1);
                    }
                    free(result);
                    variables[index].isAssigned = 1;
                    i += 5;
                    continue;
                }
                // 5 + myInt;
                if(tokens[i+2].type == INTCONST && tokens[i+3].type == OPERATOR && tokens[i+4].type == IDENTIFIER && tokens[i+5].type == END_OF_LINE){
                    int operandIndex = isVariableExist(tokens[i+4].value);
                    if (operandIndex == -1){
                        printf("Operand that you are trying to use was not declared! ");
                        exit(1);
                    }
                    if(variables[operandIndex].isAssigned == 0){
                        printf("Operand that you are trying to use was not assigned! ");
                        exit(1);
                    }
                    if(variables[index].type != INTCONST || variables[operandIndex].type != INTCONST){
                        printf("ERROR, Type of the identifier and the value that you are trying to assign is not the same. ");
                        exit(1);
                    }

                    char *result;

                    if(strcmp(tokens[i+3].value, "+") == 0){
                        int sum = atoi(variables[operandIndex].value) + atoi(tokens[i+2].value);
                        result = intToString(sum);
                        variables[index].value = realloc(variables[index].value, strlen(result) + 1);
                        strcpy(variables[index].value, result);
                    }
                    if(strcmp(tokens[i+3].value, "-") == 0){
                        if(atoi(variables[operandIndex].value) > atoi(tokens[i+2].value)){
                            printf("Error, our language don't support negative values, so you cant subtract a larger value from a smaller value ");
                            exit(1);
                        }
                        int diff = atoi(tokens[i+2].value) - atoi(variables[operandIndex].value);
                        result = intToString(diff);
                        variables[index].value = realloc(variables[index].value, strlen(result) + 1);
                        strcpy(variables[index].value, result);
                    }
                    if(strcmp(tokens[i+3].value, ":=") == 0){
                        printf("you can only use + or - operator with expression. ");
                        exit(1);
                    }
                    free(result);
                    i += 5;
                    variables[index].isAssigned = 1;
                    continue;
                }
                // myVar + myVar
                if(tokens[i+2].type == IDENTIFIER && tokens[i+3].type == OPERATOR && tokens[i+4].type == IDENTIFIER && tokens[i+5].type == END_OF_LINE){
                    int operandIndex = isVariableExist(tokens[i+2].value);
                    int otheroperandIndex = isVariableExist(tokens[i+4].value);
                    if (operandIndex == -1){
                        printf("Operand that you are trying to use was not declared! ");
                        exit(1);
                    }
                    if (otheroperandIndex == -1){
                        printf("Operand that you are trying to use was not declared! ");
                        exit(1);
                    }
                    if(variables[operandIndex].type != variables[otheroperandIndex].type || variables[index].type != variables[operandIndex].type){
                        printf("ERROR, The types of the identifiers in this myVar + myVar expression are not the same! ");
                        exit(1);
                    }
                    if(variables[operandIndex].isAssigned == 0 || variables[otheroperandIndex].isAssigned == 0 ){
                        printf("Operand that you are trying to use was not assigned! ");
                        exit(1);
                    }
                    if(variables[index].type == TEXT){
                        if(variables[index].type != TEXT || variables[operandIndex].type != TEXT || variables[otheroperandIndex].type != TEXT){
                            printf("ERROR, Type of the identifier and the value that you are trying to assign is not the same. ");
                            exit(1);
                        }

                        char *result;

                        if(strcmp(tokens[i+3].value, "+") == 0){
                            size_t len1 = strlen(variables[operandIndex].value);
                            size_t len2 = strlen(variables[otheroperandIndex].value);
                            result = malloc((len1 + len2 + 1) * sizeof(char)); // +1 for the null-terminator
                            strcpy(result, variables[operandIndex].value);
                            strcat(result, variables[otheroperandIndex].value);
                            variables[index].value = realloc(variables[index].value, strlen(result) + 1);
                            strcpy(variables[index].value, result);
                        }
                        if(strcmp(tokens[i+3].value, "-") == 0){
                            if(strlen(variables[otheroperandIndex].value) > strlen(variables[operandIndex].value)) {   // if str2(desired string) is longer than str1(main string) give an error message.
                                fprintf(stderr, "Error: Desired string is longer than the Main string\n");
                                exit(EXIT_FAILURE);
                            }

                            result = strdup(variables[operandIndex].value); // Duplicate str1

                            char *subStrPos = strstr(result, variables[otheroperandIndex].value);

                            // Check if str2 is a substring of str1
                            if(subStrPos) {
                                size_t len = strlen(variables[otheroperandIndex].value);
                                memmove(subStrPos, subStrPos + len, 1 + strlen(subStrPos + len));
                            }
                            variables[index].value = realloc(variables[index].value, strlen(result) + 1);
                            strcpy(variables[index].value, result);
                        }
                        if(strcmp(tokens[i+3].value, ":=") == 0){
                            printf("you can only use + or - operator with expression. ");
                            exit(1);
                        }
                        free(result);
                    }
                    if(variables[index].type == INTCONST){
                        char *result;

                        if(strcmp(tokens[i+3].value, "+") == 0){
                            int sum = atoi(variables[operandIndex].value) + atoi(variables[otheroperandIndex].value);
                            result = intToString(sum);
                            variables[index].value = realloc(variables[index].value, strlen(result) + 1);
                            strcpy(variables[index].value, result);
                        }
                        if(strcmp(tokens[i+3].value, "-") == 0){
                            if(atoi(variables[operandIndex].value) < atoi(variables[otheroperandIndex].value)){
                                printf("Error, our language don't support negative values, so you cant subtract a larger value from a smaller value ");
                                exit(1);
                            }
                            int diff = atoi(variables[operandIndex].value) - atoi(variables[otheroperandIndex].value);
                            result = intToString(diff);
                            variables[index].value = realloc(variables[index].value, strlen(result) + 1);
                            strcpy(variables[index].value, result);
                        }
                        if(strcmp(tokens[i+3].value, ":=") == 0){
                            printf("you can only use + or - operator with expression. ");
                            exit(1);
                        }
                        free(result);
                    }
                    i += 5;
                    variables[index].isAssigned = 1;
                    continue;
                }



                // mytext + "asdad";
                if(tokens[i+2].type == IDENTIFIER && tokens[i+3].type == OPERATOR && tokens[i+4].type == TEXT && tokens[i+5].type == END_OF_LINE){
                    int operandIndex = isVariableExist(tokens[i+2].value);
                    if (operandIndex == -1){
                        printf("Operand that you are trying to use was not declared! ");
                        exit(1);
                    }
                    if(variables[index].type != TEXT || variables[operandIndex].type != TEXT){
                        printf("ERROR, Type of the identifier and the value that you are trying to assign is not the same. ");
                        exit(1);
                    }
                    if(variables[operandIndex].isAssigned == 0){
                        printf("Operand that you are trying to use was not assigned! ");
                        exit(1);
                    }
                    char *result;

                    if(strcmp(tokens[i+3].value, "+") == 0){
                        size_t len1 = strlen(variables[operandIndex].value);
                        size_t len2 = strlen(tokens[i+4].value);
                        result = malloc((len1 + len2 + 1) * sizeof(char)); // +1 for the null-terminator
                        strcpy(result, variables[operandIndex].value);
                        strcat(result, tokens[i+4].value);
                        variables[index].value = realloc(variables[index].value, strlen(result) + 1);
                        strcpy(variables[index].value, result);
                    }
                    if(strcmp(tokens[i+3].value, "-") == 0){
                        if(strlen(tokens[i+4].value) > strlen(variables[operandIndex].value)) {   // if str2(desired string) is longer than str1(main string) give an error message.
                            fprintf(stderr, "Error: Desired string is longer than the Main string\n");
                            exit(EXIT_FAILURE);
                        }

                        result = strdup(variables[operandIndex].value); // Duplicate str1

                        char *subStrPos = strstr(result, tokens[i+4].value);

                        // Check if str2 is a substring of str1
                        if(subStrPos) {
                            size_t len = strlen(tokens[i+4].value);
                            memmove(subStrPos, subStrPos + len, 1 + strlen(subStrPos + len));
                        }
                        variables[index].value = realloc(variables[index].value, strlen(result) + 1);
                        strcpy(variables[index].value, result);
                    }
                    if(strcmp(tokens[i+3].value, ":=") == 0){
                        printf("you can only use + or - operator with expression. ");
                        exit(1);
                    }
                    free(result);
                    i += 5;
                    variables[index].isAssigned = 1;
                    continue;
                }
                // "asdasd" + myText;
                if(tokens[i+2].type == TEXT && tokens[i+3].type == OPERATOR && tokens[i+4].type == IDENTIFIER && tokens[i+5].type == END_OF_LINE){
                    int operandIndex = isVariableExist(tokens[i+4].value);
                    if (operandIndex == -1){
                        printf("Operand that you are trying to use was not declared! ");
                        exit(1);
                    }
                    if(variables[operandIndex].isAssigned == 0){
                        printf("Operand that you are trying to use was not assigned! ");
                        exit(1);
                    }
                    if(variables[index].type != TEXT || variables[operandIndex].type != TEXT){
                        printf("ERROR, Type of the identifier and the value that you are trying to assign is not the same. ");
                        exit(1);
                    }
                    char *result;

                    if(strcmp(tokens[i+3].value, "+") == 0){
                        size_t len1 = strlen(tokens[i+2].value );
                        size_t len2 = strlen(variables[operandIndex].value);
                        result = malloc((len1 + len2 + 1) * sizeof(char)); // +1 for the null-terminator
                        strcpy(result, tokens[i+2].value);
                        strcat(result, variables[operandIndex].value);
                        variables[index].value = realloc(variables[index].value, strlen(result) + 1);
                        strcpy(variables[index].value, result);
                    }
                    if(strcmp(tokens[i+3].value, "-") == 0){
                        if(strlen(variables[operandIndex].value) > strlen(tokens[i+2].value)) {   // if str2(desired string) is longer than str1(main string) give an error message.
                            fprintf(stderr, "Error: Desired string is longer than the Main string\n");
                            exit(EXIT_FAILURE);
                        }

                        result = strdup(tokens[i+2].value); // Duplicate str1

                        char *subStrPos = strstr(result, variables[operandIndex].value);

                        // Check if str2 is a substring of str1
                        if(subStrPos) {
                            size_t len = strlen(variables[operandIndex].value);
                            memmove(subStrPos, subStrPos + len, 1 + strlen(subStrPos + len));
                        }
                        variables[index].value = realloc(variables[index].value, strlen(result) + 1);
                        strcpy(variables[index].value, result);
                    }
                    if(strcmp(tokens[i+3].value, ":=") == 0){
                        printf("you can only use + or - operator with expression. ");
                        exit(1);
                    }
                    free(result);
                    variables[index].isAssigned = 1;
                    i += 5;
                    continue;
                }

                if(tokens[i+2].type == KEYWORD){
                    if(strcmp(tokens[i+2].value, "size") == 0){
                        if (!(tokens[i+3].type == BRACKET && strcmp(tokens[i+3].value, "LeftPar") == 0)){
                            printf("Error! '(' expected after a function call.");
                            exit(1);
                        }
                        if (!(tokens[i+5].type == BRACKET && strcmp(tokens[i+5].value, "RightPar") == 0)){
                            printf("Error! ')' expected");
                            exit(1);
                        }
                        if (tokens[i + 6].type != END_OF_LINE){
                            printf("Error! EndOfLine expected after a function call.");
                            exit(1);
                        }
                        if(variables[index].type != INTCONST){
                            printf("size function could only be used with integer identifier.");
                            exit(1);
                        }

                        if(tokens[i+4].type == IDENTIFIER){
                            int parameterIndex = isVariableExist(tokens[i+4].value);
                            if (parameterIndex == -1){
                                printf("Error, identifier %s was not declared! You can't use it as a parameter", tokens[i+4].value);
                                exit(1);
                            }
                            if(variables[parameterIndex].isAssigned == 0){
                                printf("Error, identifier %s was declared but was not assigned to anything yet!", variables[parameterIndex].name);
                                exit(1);
                            }

                            if(variables[parameterIndex].type != TEXT){
                                printf("Error, parameter argument that is used with size function must be text type identifier.");
                                exit(1);
                            }
                            char* val = intToString(strlen(variables[parameterIndex].value));
                            variables[index].value = realloc(variables[index].value, strlen(val) + 1);
                            strcpy(variables[index].value, val);
                            free(val); // Remember to free allocated memory
                        }

                        else if(tokens[i+4].type == TEXT){
                            variables[index].value = realloc(variables[index].value, strlen(tokens[i+4].value) + 1);
                            strcpy(variables[index].value, tokens[i+4].value);
                        }
                        else{
                            printf("Unwanted type argument for size function.");
                            exit(1);
                        }
                        variables[index].isAssigned = 1;
                        i+= 6;
                        continue;
                    }

                    if(strcmp(tokens[i+2].value, "subs") == 0) {
                        if (!(tokens[i + 3].type == BRACKET && strcmp(tokens[i + 3].value, "LeftPar") == 0)) {
                            printf("Error! '(' expected after a function call.");
                            exit(1);
                        }
                        if (!(tokens[i + 9].type == BRACKET && strcmp(tokens[i + 9].value, "RightPar") == 0)) {
                            printf("Error! ')' expected");
                            exit(1);
                        }
                        if (tokens[i + 10].type != END_OF_LINE) {
                            printf("Error! EndOfLine expected after a function call.");
                            exit(1);
                        }
                        if (variables[index].type != TEXT) {
                            printf("subs function could only be used with text identifier.");
                            exit(1);
                        }
                        if (!(tokens[i + 5].type == COMMA && tokens[i + 7].type == COMMA)) {
                            printf("Error! 'COMMA' expected between the ");
                            exit(1);
                        }

                        char *returnedText = NULL;
                        int startIndex;
                        int endIndex;

                        if (tokens[i + 4].type == IDENTIFIER) {

                            int parameterIndex = isVariableExist(tokens[i + 4].value);
                            if (parameterIndex == -1) {
                                printf("Error, parameter argument, identifier %s was not declared!",
                                       tokens[i + 4].value);
                                exit(1);
                            }
                            if (variables[parameterIndex].type != TEXT) {
                                printf("first argument of the subs function must be text value.");
                                exit(1);
                            }
                            if (variables[parameterIndex].isAssigned == 0) {
                                printf("Error, Parameter  %s text was not assigned yet.",
                                       variables[parameterIndex].name);
                                exit(1);
                            }
                            returnedText = malloc((strlen(variables[parameterIndex].value) + 1) * sizeof(char));
                            if (returnedText == NULL) {
                                printf("Memory allocation error.\n");
                                exit(1);
                            }
                            strcpy(returnedText, variables[parameterIndex].value);
                        }
                        if (tokens[i + 4].type == TEXT) {
                            // Allocate memory for returnedText
                            returnedText = malloc((strlen(tokens[i + 4].value) + 1) * sizeof(char));
                            if (returnedText == NULL) {
                                printf("Memory allocation error.\n");
                                exit(1);
                            }
                            strcpy(returnedText, tokens[i + 4].value);
                        }
                        if (tokens[i + 6].type == IDENTIFIER) {
                            int parameterIndex = isVariableExist(tokens[i + 6].value);
                            if (parameterIndex == -1) {
                                printf("Error, parameter starting argument, identifier %s was not declared!",
                                       tokens[i + 6].value);
                                exit(1);
                            }
                            if (variables[parameterIndex].type != INTCONST) {
                                printf("Error, parameter begin argument, identifier %s must be int type!",
                                       tokens[i + 6].value);
                                exit(1);
                            }
                            if (variables[parameterIndex].isAssigned == 0) {
                                printf("Error, Parameter starting argument was not assigned to something yet.");
                                exit(1);
                            }
                            startIndex = atoi(variables[parameterIndex].value);
                        }
                        if (tokens[i + 6].type == INTCONST) {
                            startIndex = atoi(tokens[i + 6].value);
                        }
                        if (tokens[i + 8].type == IDENTIFIER) {
                            int parameterIndex = isVariableExist(tokens[i + 8].value);
                            if (parameterIndex == -1) {
                                printf("Error, parameter end argument, identifier %s was not declared!",
                                       tokens[i + 8].value);
                                exit(1);
                            }
                            if (variables[parameterIndex].type != INTCONST) {
                                printf("Error, parameter end argument, identifier %s must be int type!",
                                       tokens[i + 8].value);
                                exit(1);
                            }
                            if (variables[parameterIndex].isAssigned == 0) {
                                printf("Error, Parameter end argument was not declared yet.");
                                exit(1);
                            }
                            endIndex = atoi(variables[parameterIndex].value);
                        }
                        if (tokens[i + 8].type == INTCONST) {
                            endIndex = atoi(tokens[i + 8].value);
                        }
                        if (endIndex < startIndex || startIndex < 0 || endIndex < 0) {
                            printf("Start index can't be higher than end index. Also, index values should be valid.");
                            free(returnedText); // Do not forget to free memory before exiting
                            exit(1);
                        }
                        if(endIndex >= strlen(returnedText)){
                            printf("Error, invalid index, End index exceed the parameter length.");
                            free(returnedText); // Do not forget to free memory before exiting
                            exit(1);
                        }

                        // Allocate memory for subString
                        char *subString = malloc((endIndex - startIndex + 2) * sizeof(char));
                        if (subString == NULL) {
                            printf("Memory allocation error.\n");
                            free(returnedText); // Do not forget to free memory before exiting
                            exit(1);
                        }

                        strncpy(subString, returnedText + startIndex, endIndex - startIndex + 1);
                        subString[endIndex - startIndex + 1] = '\0';

                        // Memory allocation for variables[index].value
                        variables[index].value = realloc(variables[index].value, strlen(subString) + 1);
                        if (variables[index].value == NULL) {
                            printf("Memory allocation error.\n");
                            free(returnedText); // Do not forget to free memory before exiting
                            free(subString);
                            exit(1);
                        }

                        strcpy(variables[index].value, subString);

                        // Do not forget to free memory when you're done with it
                        free(returnedText);
                        free(subString);

                        i += 10;
                        variables[index].isAssigned = 1;
                        continue;
                    }
                    if(strcmp(tokens[i+2].value, "locate") == 0){
                        if (!(tokens[i+3].type == BRACKET && strcmp(tokens[i+3].value, "LeftPar") == 0)){
                            printf("Error! '(' expected after a function call.");
                            exit(1);
                        }
                        if (!(tokens[i+9].type == BRACKET && strcmp(tokens[i+9].value, "RightPar") == 0)){
                            printf("Error! ')' expected");
                            exit(1);
                        }
                        if (tokens[i + 10].type != END_OF_LINE){
                            printf("Error! EndOfLine expected after a function call.");
                            exit(1);
                        }
                        if(variables[index].type != INTCONST){
                            printf("locate function could only be used with integer type identifier.");
                            exit(1);
                        }
                        if (!(tokens[i+5].type == COMMA && tokens[i+7].type == COMMA )){
                            printf("Error! 'COMMA' expected between the parameters.");
                            exit(1);
                        }

                        char* bigText = NULL;
                        char* smallText = NULL;
                        int startIndex = 0;

                        if(tokens[i+4].type == IDENTIFIER){
                            int parameterIndex = isVariableExist(tokens[i+4].value);
                            if (parameterIndex == -1){
                                printf("Error, parameter argument, identifier %s was not declared!", tokens[i+4].value);
                                exit(1);
                            }
                            if(variables[parameterIndex].type != TEXT){
                                printf("Error, first parameter of locate function should be text type.");
                                exit(1);
                            }
                            if(variables[parameterIndex].isAssigned == 0){
                                printf("Error, Parameter text was not assigned yet.");
                                exit(1);
                            }
                            bigText = malloc((strlen(variables[parameterIndex].value) + 1) * sizeof(char));
                            if(bigText == NULL) {
                                printf("Memory allocation error.");
                                exit(1);
                            }
                            strcpy(bigText, variables[parameterIndex].value);
                        }
                        if(tokens[i+4].type == TEXT){
                            bigText = malloc((strlen(tokens[i+4].value) + 1) * sizeof(char));
                            if(bigText == NULL) {
                                printf("Memory allocation error.");
                                exit(1);
                            }
                            strcpy(bigText, tokens[i+4].value);
                        }
                        if(tokens[i+6].type == IDENTIFIER){
                            int parameter2Index = isVariableExist(tokens[i+6].value);
                            if (parameter2Index == -1){
                                printf("Error, parameter argument, identifier %s was not declared!", tokens[i+6].value);
                                exit(1);
                            }
                            if(variables[parameter2Index].type != TEXT){
                                printf("Error, second parameter of locate function should be text type.");
                                exit(1);
                            }
                            if(variables[parameter2Index].isAssigned == 0){
                                printf("Error, Parameter text was not assigned yet.");
                                exit(1);
                            }
                            smallText = malloc((strlen(variables[parameter2Index].value) + 1) * sizeof(char));
                            if(smallText == NULL) {
                                printf("Memory allocation error.");
                                free(bigText);
                                exit(1);
                            }
                            strcpy(smallText, variables[parameter2Index].value);
                        }
                        if(tokens[i+6].type == TEXT){
                            smallText = malloc((strlen(tokens[i+6].value) + 1) * sizeof(char));
                            if(smallText == NULL) {
                                printf("Memory allocation error.");
                                free(bigText);
                                exit(1);
                            }
                            strcpy(smallText, tokens[i+6].value);
                        }
                        if(tokens[i+8].type == IDENTIFIER){
                            int parameterIndex = isVariableExist(tokens[i+8].value);
                            if (parameterIndex == -1){
                                printf("Error, parameter starting argument, identifier %s was not declared!", tokens[i+8].value);
                                exit(1);
                            }
                            if(variables[parameterIndex].type != INTCONST){
                                printf("Error, third parameter of locate function should be integer type.");
                                exit(1);
                            }
                            if(variables[parameterIndex].isAssigned == 0){
                                printf("Error, Parameter starting argument was not assigned yet.");
                                exit(1);
                            }
                            startIndex = atoi(variables[parameterIndex].value);
                        }
                        if(tokens[i+8].type == INTCONST){
                            startIndex = atoi(tokens[i+8].value);
                        }

                        char *position = strstr(bigText + startIndex, smallText);
                        if(position == NULL){
                            variables[index].value = realloc(variables[index].value, 2);
                            if (variables[index].value == NULL) {
                                printf("Memory allocation error.\n");
                                free(bigText);
                                free(smallText);
                                exit(1);
                            }
                            strcpy(variables[index].value, "0");
                        } else {
                            int resultSize = snprintf(NULL, 0, "%ld", position - bigText) + 1;
                            variables[index].value = realloc(variables[index].value, resultSize);
                            if (variables[index].value == NULL) {
                                printf("Memory allocation error.\n");
                                free(bigText);
                                free(smallText);
                                exit(1);
                            }
                            sprintf(variables[index].value, "%ld", position - bigText);
                        }

                        free(bigText);
                        free(smallText);
                        variables[index].isAssigned = 1;
                        i+= 10;
                        continue;
                    }

                    if(strcmp(tokens[i+2].value, "insert") == 0){

                        if (!(tokens[i+3].type == BRACKET && strcmp(tokens[i+3].value, "LeftPar") == 0)){
                            printf("Error! '(' expected after a function call.");
                            exit(1);
                        }
                        if (!(tokens[i+9].type == BRACKET && strcmp(tokens[i+9].value, "RightPar") == 0)){
                            printf("Error! ')' expected");
                            exit(1);
                        }
                        if (tokens[i + 10].type != END_OF_LINE){
                            printf("Error! EndOfLine expected after a function call.");
                            exit(1);
                        }
                        if(variables[index].type != TEXT){
                            printf("insert function could only be used with text type identifier.");
                            exit(1);
                        }
                        if (!(tokens[i+5].type == COMMA && tokens[i+7].type == COMMA )){
                            printf("Error! 'COMMA' expected between the parameters.");
                            exit(1);
                        }

                        char* myText = NULL;
                        int location = 0;

                        if(tokens[i+4].type == IDENTIFIER){
                            int parameterIndex = isVariableExist(tokens[i+4].value);
                            if (parameterIndex == -1){
                                printf("Error, parameter argument, identifier %s was not declared!", tokens[i+4].value);
                                exit(1);
                            }
                            if(variables[parameterIndex].type != TEXT){
                                printf("Error, first parameter of insert function should be text type value.");
                                exit(1);
                            }
                            if(variables[parameterIndex].isAssigned == 0){
                                printf("Error, Parameter text was not assigned yet.");
                                exit(1);
                            }
                            myText = realloc(myText, (strlen(variables[parameterIndex].value) + 1) * sizeof(char));
                            strcpy(myText, variables[parameterIndex].value);
                        }

                        if(tokens[i+4].type == TEXT){
                            myText = realloc(myText, (strlen(tokens[i+4].value) + 1) * sizeof(char));
                            strcpy(myText, tokens[i+4].value);
                        }

                        if(tokens[i+6].type == IDENTIFIER){
                            int parameterIndex = isVariableExist(tokens[i+6].value);
                            if (parameterIndex == -1){
                                printf("Error, parameter starting argument, identifier %s was not declared!", tokens[i+6].value);
                                exit(1);
                            }
                            if(variables[parameterIndex].type != INTCONST){
                                printf("Error, second parameter of insert function should be int type value.");
                                exit(1);
                            }
                            if(variables[parameterIndex].isAssigned == 0){
                                printf("Error, Parameter starting argument was not declared yet.");
                                exit(1);
                            }
                            location = atoi(variables[parameterIndex].value);
                        }

                        if(tokens[i+6].type == INTCONST){
                            location = atoi(tokens[i+6].value);
                        }

                        char* insertText = NULL;

                        if(tokens[i+8].type == IDENTIFIER){
                            int parameterIndex = isVariableExist(tokens[i+8].value);
                            if (parameterIndex == -1){
                                printf("Error, parameter argument, identifier %s was not declared!", tokens[i+8].value);
                                exit(1);
                            }
                            if(variables[parameterIndex].type != TEXT){
                                printf("Error, third parameter of insert function should be text type value.");
                                exit(1);
                            }
                            if(variables[parameterIndex].isAssigned == 0){
                                printf("Error, Parameter text was not assigned yet.");
                                exit(1);
                            }
                            insertText = realloc(insertText, (strlen(variables[parameterIndex].value) + 1) * sizeof(char));
                            strcpy(insertText, variables[parameterIndex].value);
                        }

                        if(tokens[i+8].type == TEXT){
                            insertText = realloc(insertText, (strlen(tokens[i+8].value) + 1) * sizeof(char));
                            strcpy(insertText, tokens[i+8].value);
                        }

                        if (location < 0 || location > strlen(myText)) {
                            printf("Error! invalid location argument for insert function.");
                            free(myText);
                            free(insertText);
                            exit(1);
                        }

                        int newLength =  strlen(insertText) + strlen(myText) + 1;

                        char *newText = (char *) malloc(newLength * sizeof(char));
                        if (newText == NULL) {
                            printf("Memory Error! Memory could not be allocated!");
                            free(myText);
                            free(insertText);
                            exit(1);
                        }

                        strncpy(newText, myText, location);
                        newText[location] = '\0';
                        strcat(newText, insertText);
                        strcat(newText, myText + location);

                        free(variables[index].value);
                        variables[index].value = newText;
                        free(myText);
                        free(insertText);
                        i += 10;
                        variables[index].isAssigned = 1;
                        continue;
                    }


                    if(strcmp(tokens[i+2].value, "override") == 0){

                        if (!(tokens[i+3].type == BRACKET && strcmp(tokens[i+3].value, "LeftPar") == 0)){
                            printf("Error! '(' expected after a function call.");
                            exit(1);
                        }
                        if (!(tokens[i+9].type == BRACKET && strcmp(tokens[i+9].value, "RightPar") == 0)){
                            printf("Error! ')' expected");
                            exit(1);
                        }
                        if (tokens[i + 10].type != END_OF_LINE){
                            printf("Error! EndOfLine expected after a function call.");
                            exit(1);
                        }
                        if(variables[index].type != TEXT){
                            printf("insert function could only be used with text type identifier.");
                            exit(1);
                        }
                        if (!(tokens[i+5].type == COMMA && tokens[i+7].type == COMMA )){
                            printf("Error! 'COMMA' expected between the parameters.");
                            exit(1);
                        }

                        char* myText = NULL;
                        int location = 0;

                        if(tokens[i+4].type == IDENTIFIER){
                            int parameterIndex = isVariableExist(tokens[i+4].value);
                            if (parameterIndex == -1){
                                printf("Error, parameter argument, identifier %s was not declared!", tokens[i+4].value);
                                exit(1);
                            }
                            if(variables[parameterIndex].type != TEXT){
                                printf("Error, first parameter of insert function should be text type value.");
                                exit(1);
                            }
                            if(variables[parameterIndex].isAssigned == 0){
                                printf("Error, Parameter text was not assigned yet.");
                                exit(1);
                            }
                            myText = realloc(myText, (strlen(variables[parameterIndex].value) + 1) * sizeof(char));
                            strcpy(myText, variables[parameterIndex].value);
                        }

                        if(tokens[i+4].type == TEXT){
                            myText = realloc(myText, (strlen(tokens[i+4].value) + 1) * sizeof(char));
                            strcpy(myText, tokens[i+4].value);
                        }

                        if(tokens[i+6].type == IDENTIFIER){
                            int parameterIndex = isVariableExist(tokens[i+6].value);
                            if (parameterIndex == -1){
                                printf("Error, parameter starting argument, identifier %s was not declared!", tokens[i+6].value);
                                exit(1);
                            }
                            if(variables[parameterIndex].type != INTCONST){
                                printf("Error, second parameter of insert function should be int type value.");
                                exit(1);
                            }
                            if(variables[parameterIndex].isAssigned == 0){
                                printf("Error, Parameter starting argument was not declared yet.");
                                exit(1);
                            }
                            location = atoi(variables[parameterIndex].value);
                        }

                        if(tokens[i+6].type == INTCONST){
                            location = atoi(tokens[i+6].value);
                        }

                        char* ovrText = NULL;

                        if(tokens[i+8].type == IDENTIFIER){
                            int parameterIndex = isVariableExist(tokens[i+8].value);
                            if (parameterIndex == -1){
                                printf("Error, parameter argument, identifier %s was not declared!", tokens[i+8].value);
                                exit(1);
                            }
                            if(variables[parameterIndex].type != TEXT){
                                printf("Error, third parameter of insert function should be text type value.");
                                exit(1);
                            }
                            if(variables[parameterIndex].isAssigned == 0){
                                printf("Error, Parameter text was not assigned yet.");
                                exit(1);
                            }
                            ovrText = realloc(ovrText, (strlen(variables[parameterIndex].value) + 1) * sizeof(char));
                            strcpy(ovrText, variables[parameterIndex].value);
                        }

                        if(tokens[i+8].type == TEXT){
                            ovrText = realloc(ovrText, (strlen(tokens[i+8].value) + 1) * sizeof(char));
                            strcpy(ovrText, tokens[i+8].value);
                        }

                        // FONKSIYONUN NE YAPTIGI
                        if (location < 0 || location >= strlen(myText)) {
                            printf("Error! invalid location argument for override function.");
                            free(myText);
                            free(ovrText);
                            exit(1);
                        }

                        for (int i = 0; ovrText[i] != '\0' && myText[location + i] != '\0'; i++) {
                            myText[location + i] = ovrText[i];
                        }

                        free(variables[index].value);  // You must free the old value before overwriting it
                        variables[index].value = myText;
                        free(ovrText);
                        i += 10;
                        variables[index].isAssigned = 1;
                        continue;
                    }
                    if(strcmp(tokens[i+2].value, "asString") == 0){
                        if (!(tokens[i+3].type == BRACKET && strcmp(tokens[i+3].value, "LeftPar") == 0)){
                            printf("Error! '(' expected after a function call.");
                            exit(1);
                        }
                        if (!(tokens[i+5].type == BRACKET && strcmp(tokens[i+5].value, "RightPar") == 0)){
                            printf("Error! ')' expected");
                            exit(1);
                        }
                        if (tokens[i + 6].type != END_OF_LINE){
                            printf("Error! EndOfLine expected after a function call.");
                            exit(1);
                        }
                        if(variables[index].type != TEXT){
                            printf("Error! asString function could only be used with text identifier.");
                            exit(1);
                        }

                        if(tokens[i+4].type == IDENTIFIER){
                            int parameterIndex = isVariableExist(tokens[i+4].value);
                            if (parameterIndex == -1){
                                printf("Error, identifier %s was not declared! You can't use it as a parameter", tokens[i+4].value);
                                exit(1);
                            }
                            if(variables[parameterIndex].type != INTCONST){
                                printf("asString function could only be used with int type identifier.");
                                exit(1);
                            }
                            if(variables[parameterIndex].isAssigned == 0){
                                printf("Error, parameter of asString function was not assigned yet.");
                                exit(1);
                            }
                            variables[index].value = realloc(variables[index].value, (strlen(variables[parameterIndex].value) + 1) * sizeof(char));
                            strcpy(variables[index].value, variables[parameterIndex].value);
                        }

                        if(tokens[i+4].type == INTCONST){
                            char intString[12];
                            sprintf(intString, "%d", atoi(tokens[i+4].value));
                            variables[index].value = realloc(variables[index].value, (strlen(intString) + 1) * sizeof(char));
                            strcpy(variables[index].value, intString);
                        }
                        i+= 6;
                        variables[index].isAssigned = 1;
                        continue;
                    }

                    if(strcmp(tokens[i+2].value, "asText") == 0){
                        if (!(tokens[i+3].type == BRACKET && strcmp(tokens[i+3].value, "LeftPar") == 0)){
                            printf("Error! '(' expected after a function call.");
                            exit(1);
                        }
                        if (!(tokens[i+5].type == BRACKET && strcmp(tokens[i+5].value, "RightPar") == 0)){
                            printf("Error! ')' expected");
                            exit(1);
                        }
                        if (tokens[i + 6].type != END_OF_LINE){
                            printf("Error! EndOfLine expected after a function call.");
                            exit(1);
                        }
                        if(variables[index].type != INTCONST){
                            printf("asText function could only be used with int type identifier.");
                            exit(1);
                        }

                        if(tokens[i+4].type == IDENTIFIER){
                            int parameterIndex = isVariableExist(tokens[i+4].value);
                            if (parameterIndex == -1){
                                printf("Error, identifier %s was not declared! You can't use it as a parameter", tokens[i+4].value);
                                exit(1);
                            }
                            if(variables[parameterIndex].type != TEXT){
                                printf("Parameter of asText function should be text value");
                                exit(1);
                            }
                            if(variables[parameterIndex].isAssigned == 0){
                                printf("Parameter of asText function was not assigned yet.");
                                exit(1);
                            }
                            variables[index].value = realloc(variables[index].value, (strlen(variables[parameterIndex].value) + 1) * sizeof(char));
                            strcpy(variables[index].value, variables[parameterIndex].value);
                        }

                        if(tokens[i+4].type == TEXT){
                            variables[index].value = realloc(variables[index].value, (strlen(tokens[i+4].value) + 1) * sizeof(char));
                            strcpy(variables[index].value, tokens[i+4].value);
                        }
                        i+= 6;
                        variables[index].isAssigned = 1;
                        continue;
                    }

                    break;
                    case END_OF_LINE:
                        // Nothing to do here
                        break;

                    default:
                        printf("Unexpected token. Does not fit our grammar.'\n");
                    exit(1);
                    break;
                }
        }
    }
}


int main(int argc, char *argv[]) {
    if(argc < 2){
        printf("Usage: TextJedi.exe<filename>\n");
        return 1;
    }
    const char* filename = argv[1];

    FILE *file = fopen(filename, "r");    //read mode

    if (file == NULL) { // myProg.tj  file doesn't exist
        printf("Failed to open myProg.tj file.\n");
        return 1;
    }




    char ch;
    char *buffer = (char*)malloc(sizeof(char));  // Dynamic memory allocation
    int i = 0;  // buffer index tracker
    int in_comment = 0;     // flag that indicate whether we are in comment

    int in_string = 0;     // flag that indicate whether we are in string constants

    while((ch = fgetc(file)) != EOF){   // reading character by character

        if (ch == '/' && !in_comment) {  // Check if we are entering a comment
            char next_ch = fgetc(file);
            // If the next character is *, we are entering a comment
            if (next_ch == '*') {
                in_comment = 1;
                continue;
            } else {
                // If not, this is a division operator
                ungetc(next_ch, file);
            }
        }
        // Check if we are exiting a comment
        if (ch == '*' && in_comment){
            char next_ch = fgetc(file);
            // If the next character is /, we are exiting a comment
            if (next_ch == '/'){
                in_comment = 0;
                continue;
            }
            else {
                if(next_ch == EOF){
                    printf("Lexical Error: Comment is not properly closed.\n");
                    exit(1);
                }
                ungetc(next_ch, file);
            }
        }
        // Check if the file ended before the comment was closed
        if(in_comment && ch == EOF) {
            printf("Lexical Error: Comment is not properly closed.\n");
            exit(1);
        }
        // If we are inside a comment, ignore the characters
        if (in_comment){
            char next_ch = fgetc(file);
            if(next_ch == EOF){
                printf("Lexical Error: Comment is not properly closed.\n");
                exit(1);
            }
            else{
                ungetc(next_ch, file);
                continue;
            }

        }
            // IF WE ARE NOT IN COMMENT BELOW LINES CAN BE REACHED. OTHERWISE, IT CANNOT BE REACHED BECAUSE
            // UPPER LINE WE SAID THAT IF(IN COMMENT) --> THEN CONTINUE.
            // EndOfLıne Token
        else if(ch == ';') {
            addToken(END_OF_LINE, ";");
        }
        else if(ch == ','){
            addToken(COMMA, ",");
        }

            // String constant handling
        else if (ch == '\"') {    // starting of the string
            in_string = 1;  // make the flag ON

            while ((ch = fgetc(file)) != '\"') {   // reading characters from a code.psi until double quote is found
                if (ch == EOF) {    // String closing error!
                    printf("Lexical Error: String is not properly closed \n");
                    return 1;
                }

                // Dynamic Memory Allocation for buffer, when it is needed.
                buffer = realloc(buffer, (i+1)*sizeof(char));
                buffer[i++] = ch;
            }

            // Dynamic Memory Allocation for buffer for adding null character at the end of the char array
            buffer = realloc(buffer, (i+1)*sizeof(char));
            buffer[i] = '\0';
            addToken(TEXT, buffer);
            free(buffer);  // Free the memory

            buffer = malloc(sizeof(char));  // Allocating new memory space for the next string
            i = 0;  // make the buffer index tracker 0
        }

            // Operator Tokens Handling
        else if(ch == '+' || ch == '-' ||  ch == ':') {
            char next_ch = fgetc(file);

            if(ch == ':' && next_ch == '=') {
                addToken(OPERATOR, ":=");
            } else {
                ungetc(next_ch, file);  // put back to char that was read from the code.psi
                char str[2];

                str[0] = ch;
                str[1] = '\0';
                addToken(OPERATOR, str);
            }
            continue;
        }

            // Space doesn't count, it does not produce token like comments.
        else if (isspace(ch)){
            continue;
        }

            // Integer constants token handling
        else if (isdigit(ch)) {
            unsigned int maxInt = UINT_MAX;
            buffer[i++] = ch;

            while (isdigit(ch = fgetc(file))) { // Reading characters from code.psi until a non-digit character is found
                buffer[i++] = ch;
            }
            buffer[i] = '\0';  // put termination char at the end of the character array

            int num = atoi(buffer);
            if(num > maxInt){
                printf("Lexical Error: Integer %s is too big, it cannot be higher than MAXINT OF C.\n", buffer);
                exit(1);      // DOSYA KAPANDI BİTTİ
            }
            else {
                addToken(INTCONST, buffer);
            }
            i = 0;  // make the buffer index tracker 0
            ungetc(ch, file); // Put back the last read character
            // because there is one more extra read for terminating while loop.
        }

            // Identifiers and keywords Tokens handling
        else if (isalpha(ch)) {        // my_var1;
            buffer[i++] = ch;
            // Identifiers can only contain alphabetic chars, digits and underscore characters.
            while (isalpha(ch = fgetc(file)) || ch == '_' || isdigit(ch)) {
                buffer[i++] = (ch);
            }
            buffer[i] = '\0';  // put termination char at the end of the character array

            if (strlen(buffer) > MAX_IDENTIFIER_LENGTH) {   // Check if the identifier is too long
                printf("Identifier %s is too long. It cannot contain more than 30 characters.\n", buffer);
                exit(1);
            } else if (is_keyword(buffer)) {    // check whether it is keyword or identifier.
                addToken(KEYWORD, buffer);
            } else {
                addToken(IDENTIFIER, buffer);
            }
            i = 0;
            ungetc(ch, file); // Put back the last read character
            // because there is one more extra read for terminating while loop.
        }
        else if (bracket(ch)){      // Bracket Token Handling
            addToken(BRACKET, bracket(ch));
        }
            // Undefined Character Handling. (Note: if there is a code in psi like that :  _myvariable_1
            // our code takes first _ character as a undefined character,
            // because strings cannot start with underscore character. However, second underscore character is legal.
        else if(!isspace(ch) && !isalnum(ch) &&  ch != '(' && ch != ')' &&  ch !=',') {
            printf("Error: Undefined character: %c\n", ch);
            exit(1);
        }

    }
    free(buffer); // Free the memory
    // Closing the input and output files' pointers
    fclose(file);


    parse();

    // Cleanup
    for (int i = 0; i < tokenSize; i++) {
        free(tokens[i].value);
    }
    free(tokens);
    for (int i = 0; i < varSize; i++) {
        free(variables[i].name);
        free(variables[i].value);
    }
    free(variables);
    return 0;
}

