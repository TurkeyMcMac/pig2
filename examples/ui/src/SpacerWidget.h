#ifndef SPACER_WIDGET_H_
#define SPACER_WIDGET_H_

#include "Widget.h"

// A SpacerWidget is an Object implementing Widget and Node. It is invisible,
// but has certain preferred and minimum dimensions. It can be used to space out
// widgets on a grid.

struct SpacerWidget;
typedef struct SpacerWidget SpacerWidget;

// Constructs a SpacerWidget with the given dimensions.
SpacerWidget *SpacerWidget_alloc(
	struct widget_pair dims, struct widget_pair min_dims);

#endif /* SPACER_WIDGET_H_ */
