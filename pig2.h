#ifndef PIG2_H_
#define PIG2_H_

typedef const void *(*pig2_getter_fun)(const void *iid);

#define PIG2_GET(obj, iid) ((*(pig2_getter_fun *)(obj))(iid))

#define PIG2_SET_GETTER(obj, getter) \
	((void)(*(pig2_getter_fun *)(obj) = (getter)))

#endif /* PIG2_H_ */
