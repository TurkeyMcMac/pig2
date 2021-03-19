#ifndef SPACER_WIDGET_H_
#define SPACER_WIDGET_H_

#include "Widget.h"

struct SpacerWidget;
typedef struct SpacerWidget SpacerWidget;

SpacerWidget *SpacerWidget_alloc(
	struct widget_pair dims, struct widget_pair min_dims);

#endif /* SPACER_WIDGET_H_ */
