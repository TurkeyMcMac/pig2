/* MIT License
 *
 * Copyright (c) 2021 Jude Melton-Houghton
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef PIG2_H_
#define PIG2_H_

/* pig2 is a way to represent runtime-polymorphic objects in a simple way. An
 * object pointer in this header is a void pointer pointing to a function
 * pointer. It is intended that an object is a struct of your own making whose
 * first member is the function pointer. In this way, a pointer to the struct
 * can be portably cast to a pointer to a function pointer without depending on
 * the contents of the struct. Code using an object pointer can query its
 * implementation of a particular interface by calling the function pointer
 * (which is of type pig2_getter_fun.) Interfaces are identified in queries by
 * unique pointers. To create an interface ID, just define a global constant
 * variable. A pointer to this variable will be distinct from pointers to all
 * other variables, so it can be used as an interface ID. The other part of an
 * interface is its implementation. Define a struct containing function pointers
 * for methods. A pointer to one of these structs can be returned from the
 * pig2_getter_fun and the caller will cast it to the appropriate type.
 */

/* This represents a getter function that is given a non-NULL interface ID and
 * returns an implementation (which must always be the same for a given ID.) It
 * cannot dereference the interface ID. The return value can be cast to the type
 * representing an implementation of the interface corresponding to the given
 * ID. The function returns NULL if an implementation was not found for the
 * given ID.
 */
typedef const void *(*pig2_getter_fun)(const void *iid);

/* Gets an implementation pointer for the interface represented by the pointer
 * iid from the constant object pointer obj. This is a shortcut for calling the
 * object's pig2_getter_fun.
 */
#define PIG2_GET(obj, iid) ((*(const pig2_getter_fun *)(obj))(iid))

/* Sets the getter function to getter for the object pointer obj. This can be
 * used in a constructor, for example, and provides a uniform way to set the
 * getter even if the getter is embedded in a sub-struct. An object should only
 * set its getter in its constructor, then leave the getter the same for the
 * rest of its lifetime.
 */
#define PIG2_SET_GETTER(obj, getter) \
	((void)(*(pig2_getter_fun *)(obj) = (getter)))

#endif /* PIG2_H_ */
