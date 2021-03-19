#ifndef WIDGET_H_
#define WIDGET_H_

#include <curses.h>
#include <stdbool.h>

struct widget_pair {
	int x, y;
};

struct Widget_impl {
	void (*get_requested_dims)(void *self,
		struct widget_pair *dims, struct widget_pair *min_dims);
	void (*draw)(void *self, WINDOW *win,
		struct widget_pair pos, struct widget_pair dims);
	bool (*focus)(void *self);
	bool (*recv_input)(void *self, int key);
	void (*unfocus)(void *self);
};

extern const char Widget_iid[1];

#endif /* WIDGET_H_ */
