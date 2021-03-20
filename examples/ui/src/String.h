#ifndef STRING_H_
#define STRING_H_

#include <stddef.h>

struct String;
typedef struct String String;

String *String_alloc(void);

size_t String_len(const String *str);

const char *String_cstr(const String *str);

void String_append(String *str, const char *append);

#endif /* STRING_H_ */
