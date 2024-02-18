#include <stdlib.h>
#include <string.h>

#include "domainlogic.h"
#include "appdata.h"
#include "lib.h"

static int isOutOfRange(long long number)
{
    return (number >> 32) != 0;
}

static int getCharValue(char c)
{
    if (c >= '0' && c <= '9') {
        return c - '0';
    } else if (c >= 'A' && c <= 'F') {
        return c - 'A' + 10;
    } else if (c >= 'a' && c <= 'f') {
        return c - 'a' + 10;
    } else {
        return -1;
    }
}

static int isValidChar(char c, int radix)
{
    int charValue = getCharValue(c);
    return charValue == -1 || charValue >= radix ? false : true;
}

static Error stringToInteger(const char* string, int radix, int* result)
{
    long long accumulator = 0;
    int sign = 1;
    int i = 0;
    Error error = NoError;

    if (isEmptyString(string)) {
        error = EmptyString;
        return error;
    }

    if (radix == 10 && *string == '-') {
        sign = -1;
        i++;
    }

    for (; string[i]; ++i) {
        if (!isValidChar(string[i], radix)) {
            error = InvalidChar;
            break;
        }
        accumulator = accumulator * radix + getCharValue(string[i]);
        if (isOutOfRange(accumulator)) {
            error = OutOfRange;
            break;
        }
    }

    if (error == NoError && result != NULL) {
        *result = ((int) accumulator) * sign;
    }

    return error;
}

static Error validateInput(const char* input, int radix)
{
    return stringToInteger(input, radix, NULL);
}

static void swapChar(char* a, char* b)
{
    char c = *a;
    *a = *b;
    *b = c;
}

static void reverseString(char* string)
{
    for (int i = 0, j = strlen(string) - 1; i < j; ++i, --j) {
        swapChar(string + i, string + j);
    }
}

static int getHexBaseCode(int number)
{
    if (number >= 10 && number <= 15) {
        return 'A';
    } else {
        return -1;
    }
}

static int getDecimalBaseCode(int number)
{
    if (number >= 0 && number <= 9) {
        return '0';
    } else {
        return -1;
    }
}

static int getBaseCode(int number)
{
    if (number >= 0 && number <= 9) {
        return getDecimalBaseCode(number);
    } else if (number >= 10 && number <= 15) {
        return getHexBaseCode(number);
    } else {
        return -1;
    }
}

static int transformRemainder(int remainder)
{
    if (remainder >= 0 && remainder <= 9) {
        return remainder;
    } else if (remainder >= 10 && remainder <= 15) {
        return remainder - 10;
    } else {
        return -1;
    }
}

static char* integerToString(int number, int radix, char* string)
{
    int i = 0;
    unsigned num = radix == 10 && number < 0 ?
                    ~number + 1 : number;
    do {
        unsigned remainder = num % radix;
        int transformedRemainder = transformRemainder(remainder);
        int baseCode = getBaseCode(remainder);
        if (transformedRemainder == -1 || baseCode == -1) {
            return NULL;
        }
        string[i++] = transformedRemainder + baseCode;
        num /= radix;
    } while (num != 0);

    if (radix == 10 && number < 0) {
        string[i++] = '-';
    }

    string[i] = '\0';
    reverseString(string);

    return string;
}

static void swapInt(int* a, int* b)
{
    int c = *a;
    *a = *b;
    *b = c;
}

static void swapRadix(int* a, int* b)
{
    swapInt(a, b);
}

static void swapString(char* a, char* b)
{
    size_t i;

    for (i = 0; a[i] && b[i]; ++i) {
        swapChar(a + i, b + i);
    }
    if (b[i]) {
        int j = i;
        for (; b[i]; ++i) {
            a[i] = b[i];
        }
        a[i] = '\0';
        b[j] = '\0';
    } else if (a[i]) {
        int j = i;
        for (; a[i]; ++i) {
            b[i] = a[i];
        }
        b[i] = '\0';
        a[j] = '\0';
    }
}

static void performInputSwap(char* input, char* output)
{
    swapString(input, output);
}

void convertInput(AppData* appData)
{
    int convertedInput;
    appData->error = stringToInteger(appData->input.begin, appData->inputRadix, &convertedInput);
    if (appData->error == NoError) {
        integerToString(convertedInput, appData->outputRadix, appData->output);
    }
}

void setInputRadix(AppData* appData, int radix)
{
    appData->inputRadix = radix;
    appData->error = validateInput(appData->input.begin, appData->inputRadix);
}

void setOutputRadix(AppData* appData, int radix)
{
    appData->outputRadix = radix;
    appData->error = validateInput(appData->input.begin, appData->inputRadix);
}

void setInput(AppData* appData, const char* input)
{
    assignCharVector(&appData->input, input);
    appData->error = validateInput(appData->input.begin, appData->inputRadix);
}

void swapInputToOutput(AppData* appData)
{
    appData->error = validateInput(appData->input.begin, appData->inputRadix);
    if (appData->error == NoError) {
        swapRadix(&appData->inputRadix, &appData->outputRadix);
        performInputSwap(appData->input.begin, appData->output);
    }
}
