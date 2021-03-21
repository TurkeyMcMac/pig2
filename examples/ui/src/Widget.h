#ifndef WIDGET_H_
#define WIDGET_H_

#include <curses.h>

// Widgets are UI elements that are drawn and may receive input. In this program
// they form a directed acyclic graph, since the children are reference-counted
// and can be used in multiple places.

// This represents an x-y pair of screen coordinates/distances.
struct widget_pair {
	int x, y;
};

struct Widget_impl {
	// Gets the dimensions the widget wants. The preferred dimensions are
	// put into dims. The dimensions below which the widget would not have
	// enough room do display all its information are put into min_dims.
	// This method should return consistent results for consecutive calls.
	void (*get_requested_dims)(void *self,
		struct widget_pair *dims, struct widget_pair *min_dims);
	// Draws the widget on the given window at the given position with the
	// given widget dimensions. This method is only ever called zero or more
	// times directly after get_requested_dims. dims may be less than the
	// minimum are above the preferred determined by get_requested_dims.
	// This method should have consistent results for consecutive calls.
	void (*draw)(void *self, WINDOW *win,
		struct widget_pair pos, struct widget_pair dims);
	// Moves the focus to the widget. The return value is whether the widget
	// accepts the focus. If the widget is already the focus, it should
	// return true. This may change the widget state if true is returned and
	// the widget was not already in focus. The widget graph should be
	// redrawn when a widget changes state.
	bool (*focus)(void *self);
	// Gives a key to the widget. This will only be called on focused
	// widgets, so it can be NULL if focus will always return false. The
	// return value is whether the widget accepts the key. If it does not,
	// the parent widget may use the key instead. If so, the widget's state
	// may change
	bool (*recv_input)(void *self, int key);
	// Removes focus from an already focused widget, possibly changing its
	// state. Like recv_input, this may be NULL for unfocusable widgets.
	void (*unfocus)(void *self);
};

// This pointer identifies the interface.
extern const char Widget_iid[1];

#endif /* WIDGET_H_ */
