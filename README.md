# pig2

This is the second version of a previous project of mine called "pig" (which is
defective.) pig2 lets you use runtime polymorphism in C (89 and onward) with a
manageable amount of boilerplate and few macros. pig2 is more of a concept than
a library, so you don't technically need to include `pig2.h` at all.

## How It Works

A pig2 object pointer is essentially a pointer to a function pointer. Some code
using an object pointer calls the function pointer, passing in another pointer
used purely to identify a certain interface uniquely. The function returns yet
another pointer, which the caller casts to the type corresponding to the
interface ID they used. This type is a user-defined struct representing an
interface implementation and containing function pointers and stuff. The
function pointers in the implementation are methods which the caller can call on
the original object for as long as they want.

The system is very simple. It doesn't require you to register anything at
runtime, since the linker ensures all interfaces have unique IDs. It's also
pretty flexible. For example, you can implement inheritance, an example of which
is in `examples/ui`.

There's some more documentation in `pig2.h`.

## Examples

Right now, the only example is in `examples/ui`. It's pretty sizeable, though.
You can read more about it in its root directory.

## Limitations

Right now, the getter functions have no context. This would make it hard to use
pig2 in scripting languages without generating code, for example. I could add a
second getter parameter pointing to the object containing the getter, thus
providing some context. I could also have objects contain pointers to some list
data structure rather than a function, but this would be less flexible in some
ways. I don't know how it would compare speed-wise.
