#include "Object.h"
#include <assert.h>
#include <pig2.h>
#include <stdlib.h>

Object *Object_alloc(size_t struct_size)
{
	assert(struct_size >= sizeof(Object));
	Object *obj = malloc(struct_size);
	assert(obj);
	PIG2_SET_GETTER(obj, Object_getter);
	obj->refcount = 1;
	return obj;
}

Object *Object_add_ref(Object *obj)
{
	++obj->refcount;
	return obj;
}

void Object_remove_ref(Object *obj)
{
	if (obj) {
		--obj->refcount;
		if (obj->refcount <= 0) {
			const struct Object_impl *impl =
				PIG2_GET(obj, Object_iid);
			assert(impl);
			impl->release(obj);
		}
	}
}

const char Object_iid[1];

const void *Object_getter(const void *iid)
{
	static const struct Object_impl Object_impl = {
		.release = Object_release,
	};
	if (iid == Object_iid) return &Object_impl;

	return NULL;
}

void Object_release(Object *self)
{
	free(self);
}
