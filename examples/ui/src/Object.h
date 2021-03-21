#ifndef OBJECT_H_
#define OBJECT_H_

#include <stddef.h>
#include <pig2.h>

// Object is an extendable base class that implements reference counting.

// The struct is exposed so that it can be embedded at the beginning of subclass
// structs. This lets pointers to subclass objects be cast to their superclass.
typedef struct {
	// Subclasses will use this as their getter, too.
	pig2_getter_fun getter;
	long refcount;
} Object;

// Allocates an object. The size is passed so that subclass constructors can
// allocate some space for their own data. The return value won't be NULL. The
// returned object has one reference. This constructor provides a default
// getter implementing Object abstract methods (see Object_impl.) Subclass
// constructors can override the getter with PIG2_SET_GETTER(...).
Object *Object_alloc(size_t struct_size);

// Increments the reference count and returns the argument.
Object *Object_add_ref(Object *obj);

// Decrements the reference count and releases the object if necessary. This
// does nothing when passed NULL so as to behave more like free(...).
void Object_remove_ref(Object *obj);

// This interface defines the abstract methods of the Object class.
struct Object_impl {
	// Release the resources held by the object, e.g. decrement referenced
	// sub-objects. Subclasses must call their superclasses' implementation
	// after doing their thing.
	void (*release)(Object *self);
};

// This is the identifying pointer of the Object interface.
extern const char Object_iid[1];

// The is the getter provided by Object_alloc. A subclass must call the getter
// of its superclass and return the result from within its own getter after it
// has checked for interface implementations which it overrides.
const void *Object_getter(const void *iid);

// This is the base implementation of the release method. As mentioned above,
// subclasses must call this as the last part of their own release methods.
void Object_release(Object *self);

#endif /* OBJECT_H_ */
