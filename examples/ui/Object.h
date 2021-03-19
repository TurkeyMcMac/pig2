#ifndef OBJECT_H_
#define OBJECT_H_

#include <stddef.h>
#include <pig2.h>

typedef struct {
	pig2_getter_fun getter;
	long refcount;
} Object;

Object *Object_alloc(size_t struct_size);

Object *Object_add_ref(Object *obj);

void Object_remove_ref(Object *obj);

struct Object_impl {
	void (*release)(Object *self);
};

extern const char Object_iid[1];

const void *Object_getter(const void *iid);

void Object_release(Object *self);

#endif /* OBJECT_H_ */
