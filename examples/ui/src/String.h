#ifndef STRING_H_
#define STRING_H_

#include <stddef.h>

// A String is an Object holding some mutable text. It doesn't have very many of
// its own methods right now.

struct String;
typedef struct String String;

// Constructs an empty string.
String *String_alloc(void);

// Returns the length of the string.
size_t String_len(const String *str);

// Returns a NUL-terminated string representation of str. The pointer is valid
// until another mutating method is called on str.
const char *String_cstr(const String *str);

// Appends the given text to the string, mutating it.
void String_append(String *str, const char *append);

#endif /* STRING_H_ */
