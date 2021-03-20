#ifndef SCROLL_WIDGET_H_
#define SCROLL_WIDGET_H_

#include "Object.h"

struct ScrollWidget;
typedef struct ScrollWidget ScrollWidget;

ScrollWidget *ScrollWidget_alloc(int height, Object *inner);

#endif /* SCROLL_WIDGET_H_ */
