#ifndef SCROLL_WIDGET_H_
#define SCROLL_WIDGET_H_

#include "Object.h"

// A ScrollWidget is an Object implementing Widget and Node. It holds another
// widget and provides its own screen on which to draw this widget. It only
// shows part of this screen at a time, and can be scrolled vertically using the
// arrow keys.

struct ScrollWidget;
typedef struct ScrollWidget ScrollWidget;

// Constructs a ScrollWidget. It will be the given height and will hold the
// given widget object. Its width is based on that of the inner widget.
ScrollWidget *ScrollWidget_alloc(int height, Object *inner);

#endif /* SCROLL_WIDGET_H_ */
