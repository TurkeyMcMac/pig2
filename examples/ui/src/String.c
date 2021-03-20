#include "String.h"
#include "Object.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

struct String {
	Object obj;
	char *text;
	size_t len;
};

static const void *getter(const void *iid);

String *String_alloc(void)
{
	String *str = (String *)Object_alloc(sizeof(String));
	PIG2_SET_GETTER(str, getter);
	str->text = NULL;
	str->len = 0;
	return str;
}

size_t String_len(const String *str)
{
	return str->len;
}

const char *String_cstr(const String *str)
{
	return str->text ? str->text : "";
}

void String_append(String *str, const char *append)
{
	size_t append_len = strlen(append);
	size_t append_start = str->len;
	str->len += append_len;
	str->text = realloc(str->text, str->len + 1);
	assert(str->text);
	memcpy(str->text + append_start, append, append_len + 1);
}

static void release(Object *self)
{
	free(((String *)self)->text);
	Object_release(self);
}

static const void *getter(const void *iid)
{
	static const struct Object_impl Object_impl = {
		.release = release,
	};
	if (iid == Object_iid) return &Object_impl;

	return Object_getter(iid);
}
