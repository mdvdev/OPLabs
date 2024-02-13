#include <stdlib.h>
#include <string.h>

#include "domainlogic.h"
#include "appdata.h"

#define MAX_DECIMAL_LEN 10
#define MAX_OCTAL_LEN   11
#define MAX_BINARY_LEN  32

int isOutOfRange(long long number)
{
    return (number >> 32) != 0;
}

int isValidChar(char c, int radix)
{
    return (radix == 10 && c >= '0' && c <= '9') ||
           (radix == 8 && c >= '0' && c <= '7') ||
           (radix == 2 && (c == '0' || c == '1'));
}

Error stringToInteger(const char* string, int radix, int* result)
{
    long long accumulator = 0;
    int sign = 1;
    int i = 0;
    Error error = NoError;

    if (radix == 10 && *string == '-') {
        sign = -1;
        i++;
    }

    for (; string[i]; ++i) {
        if (!isValidChar(string[i], radix)) {
            error = InvalidChar;
            break;
        }
        accumulator = accumulator * radix + string[i] - '0';
        if (isOutOfRange(accumulator)) {
            error = OutOfRange;
            break;
        }

    }

    if (error == NoError && result != NULL)
        *result = ((int) accumulator) * sign;

    return error;
}

void reverseString(char* string)
{
    char c;
    for (int i = 0, j = strlen(string) - 1; i < j; ++i, --j) {
        c = string[i];
        string[i] = string[j];
        string[j] = c;
    }
}

// assumes input is correct
char* integerToString(int number, int radix)
{
    int len;

    if (radix == 10)
        len = MAX_DECIMAL_LEN;
    else if (radix == 8)
        len = MAX_OCTAL_LEN;
    else
        len = MAX_BINARY_LEN;

    char* string = (char*) malloc(len + 2); // 1 for '\0' and 1 for '-' if necessary
    if (string == NULL)
        return NULL;

    int i = 0;
    unsigned num = radix == 10 && number < 0 ?
                    ~number + 1 : number;
    do {
        string[i++] = num % radix + '0';
        num /= radix;
    } while (num != 0);

    if (radix == 10 && number < 0)
        string[i++] = '-';

    string[i] = '\0';
    reverseString(string);

    return string;
}

char* convertInput(const AppData* appData, const char* input)
{
    int convertedInput;
    (void) stringToInteger(input, appData->inputRadix, &convertedInput);
    return integerToString(convertedInput, appData->outputRadix);
}

Error validateInput(const AppData* appData, const char* input)
{
    return stringToInteger(input, appData->inputRadix, NULL);
}

void setInputRadix(AppData* appData, int radix)
{
    appData->inputRadix = radix;
}

void setOutputRadix(AppData* appData, int radix)
{
    appData->outputRadix = radix;
}
