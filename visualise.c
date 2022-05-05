#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <limits.h>
#include <ctype.h>

// Specified error message defined as a global constant.
#define ERROR_MESSAGE "Input error."

// Using constants to say which type is being used.
enum { Char, Int, Long, unsignedChar, unsignedInt, Double };
typedef int type;

// Handles errors called in functions that don't return a string.
void error() {
  fprintf(stderr, "%s\n", ERROR_MESSAGE);
  exit(1);
}

// ----------------------------- CONVERSION FUNCTIONS --------------------------

// Returns maximum possible output size of binary string for a given type
// including spaces between nibbles and null terminator, or returns 0 for
// invalid type.
int mySizeOf(type t) {
  int size = 0;

  switch (t) {
    case Char: size = sizeof(char); break;
    case Int: size = sizeof(int); break;
    default: break;
  }

  return size;
}

// Returns the formatted binary string of a valid char or int or the error
// message if the decimal value is not valid for the given type.
char * decimalToBinary(type t, const char*decimalInput) {
  int length = strlen(decimalInput);

  switch (t) {
    case Char: if (length > 4) return ERROR_MESSAGE;
    case Int: if (length > 11) return ERROR_MESSAGE;
    default: break;
  }

  int i = 0;

  if (decimalInput[0] == '-') i++;

  bool startingZero = false;

  if (decimalInput[0] == '0') startingZero = true;

  while (i < length) {
    if (isdigit(decimalInput[i]) == 0) return ERROR_MESSAGE;
    if (startingZero && decimalInput[i] != '0') return ERROR_MESSAGE;

    i++;
  }

  int x = atoi(decimalInput);

  switch (t) {
    case Char: if (x < SCHAR_MIN || x > SCHAR_MAX) return ERROR_MESSAGE;
    case Int: if (x < INT_MIN || x > INT_MAX) return ERROR_MESSAGE;
    default: break;
  }

  int outputSize = (mySizeOf(t) * 10);

  char *out = malloc(outputSize);

  for (int i = 0; i < (outputSize); i++) out[i] = 0;

  char temp[2] = {0,0};

  for (int i = (mySizeOf(t) * 8) - 1; i >= 0; i--) {
    if (x & (1u << i)) temp[0] = '1';
    else temp[0] = '0';

    strcat(out, temp);

    if(i > 0 && i % 4 == 0) strcat(out, " ");
  }

  return out;
}

// Given a string specifying a type, return the enumerated value of said type
// or -1 if the string is not a valid type.
type stringToType(const char *typeInput) {
  type t = -1;

  if (strcmp(typeInput, "char") == 0) t = Char;
  else if (strcmp(typeInput, "int") == 0) t = Int;

  return t;
}

// Returns true if the string 'typeInput' represents a valid type, otherwise
// false.
bool checkType(const char *typeInput) {
  if (stringToType(typeInput) == -1) return false;

  return true;
}

// Prints to the console the bit structure of the given data in groups of a
// nibble, or throws an error for invalid input. Function is only called if the
// user passes command line parameters.
void visualise(const char *typeInput, const char *decimalInput) {
  if (!checkType(typeInput)) error();

  type t = stringToType(typeInput);

  char *out = malloc(mySizeOf(t) * 10);

  switch (t) {
    case Char: out = decimalToBinary(Char, decimalInput); break;
    case Int: out = decimalToBinary(Int, decimalInput); break;
    default: out = ERROR_MESSAGE;
  }

  printf("%s\n", out);
}

// ----------------------------- TESTING FUNCTIONS -----------------------------

void assert(int line, bool b) {
  if (b) return;
  printf("The test on line %d fails.\n", line);
  exit(1);
}

bool check(type t, const char *in, const char *out) {
  if (strcmp(decimalToBinary(t, in), out) == 0) return true;
  return false;
}

void testCheckType() {
  assert(__LINE__, (checkType("char") == true));
  assert(__LINE__, (checkType("int") == true));
  assert(__LINE__, (checkType("") == false));
  assert(__LINE__, (checkType("gobbledygook") == false));
}

void testStringToType() {
  assert(__LINE__, (stringToType("char") == Char));
  assert(__LINE__, (stringToType("int") == Int));
  assert(__LINE__, (stringToType("") == -1));
  assert(__LINE__, (stringToType("gobbledygook") == -1));
}

void testMySizeOf() {
  assert(__LINE__, (mySizeOf(Char) == 1));
  assert(__LINE__, (mySizeOf(Int) == 4));
}

void testDecimalToBinary() {
  // Tests function valid for type char.
  assert(__LINE__, check(Char, "7", "0000 0111"));
  assert(__LINE__, check(Char, "-128", "1000 0000"));
  assert(__LINE__, check(Char, "-50", "1100 1110"));
  assert(__LINE__, check(Char, "101", "0110 0101"));
  assert(__LINE__, check(Char, "255", "Input error."));
  assert(__LINE__, check(Char, "08", "Input error."));
  assert(__LINE__, check(Char, "-x0", "Input error."));
  // Tests function valid for type int.
  assert(__LINE__, check(Int, "10000000", "0000 0000 1001 1000 1001 0110 1000 0000"));
  assert(__LINE__, check(Int, "-1000000000", "1100 0100 0110 0101 0011 0110 0000 0000"));
  assert(__LINE__, check(Int, "10055", "0000 0000 0000 0000 0010 0111 0100 0111"));
  assert(__LINE__, check(Int, "-100000000000", "Input error."));
  assert(__LINE__, check(Int, "0x64", "Input error."));
  assert(__LINE__, check(Int, "000005", "Input error."));
}

void test() {
  testCheckType();
  testStringToType();
  testMySizeOf();
  testDecimalToBinary();
  printf("All tests pass.\n");
}

int main(int n, char *args[n]) {
  setbuf(stdout, NULL);
  if (n == 1) {
    test();
  } else if (n == 3) {
    visualise(args[1], args[2]);
  } else {
    error();
  }
  return 0;
}
