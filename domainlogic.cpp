#include <stdlib.h>
#include <string.h>

#include "domainlogic.h"
#include "appdata.h"

static int isOutOfRange(long long number)
{
    return (number >> 32) != 0;
}

static int isValidChar(char c, int radix)
{
    static char table[] = "0123456789ABCDEF";
    for (int i = 0; i < (int) sizeof(table) && i < radix; ++i)
        if (table[i] == c)
            return true;
    return false;
}

static int isEmptyString(const char* string)
{
    return *string == '\0';
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
        accumulator = accumulator * radix + string[i] - '0';
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

static void swapChars(char* a, char* b)
{
    char c = *a;
    *a = *b;
    *b = c;
}

static void reverseString(char* string)
{
    for (int i = 0, j = strlen(string) - 1; i < j; ++i, --j) {
        swapChars(string + i, string + j);
    }
}

static void integerToString(int number, int radix, char* string)
{
    int i = 0;
    unsigned num = radix == 10 && number < 0 ?
                    ~number + 1 : number;
    do {
        string[i++] = num % radix + '0';
        num /= radix;
    } while (num != 0);

    if (radix == 10 && number < 0) {
        string[i++] = '-';
    }

    string[i] = '\0';
    reverseString(string);
}

void convertInput(AppData* appData)
{
    int convertedInput;
    appData->error = stringToInteger(appData->input, appData->inputRadix, &convertedInput);
    if (appData->error == NoError) {
        integerToString(convertedInput, appData->outputRadix, appData->output);
    }
}

void setInputRadix(AppData* appData, int radix)
{
    appData->inputRadix = radix;
    appData->error = validateInput(appData->input, appData->inputRadix);
}

void setOutputRadix(AppData* appData, int radix)
{
    appData->outputRadix = radix;
    appData->error = validateInput(appData->input, appData->inputRadix);
}

void setInput(AppData* appData, const char* input)
{
    appData->error = validateInput(input, appData->inputRadix);
    if (appData->error == NoError) {
        strncpy(appData->input, input, sizeof(appData->input) - 1);
        appData->input[sizeof(appData->input) - 1] = '\0';
    }
}

void setOutput(AppData* appData, char* output)
{
    strncpy(appData->output, output, sizeof(appData->output) - 1);
    appData->output[sizeof(appData->output) - 1] = '\0';
}
