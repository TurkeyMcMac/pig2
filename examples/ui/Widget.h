#ifndef WIDGET_H_
#define WIDGET_H_

#include <curses.h>
#include <stdbool.h>

struct widget_pair {
	int x, y;
};

struct Widget_impl {
	void (*get_requested_dims)(void *self, struct widget_pair *dims,
		struct widget_pair *min_dims, struct widget_pair *max_dims);
	void (*draw)(void *self, WINDOW *win,
		const struct widget_pair *pos,
		const struct *widget_pair dims,
		const struct widget_pair *last_pos,
		const struct widget_pair *last_dims);
	bool (*focus)(void *self);
	bool (*recv_input)(void *self, int key);
	void (*unfocus)(void *self);
};

extern const char Widget_iid[1];

#endif /* WIDGET_H_ */
