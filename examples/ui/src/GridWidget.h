#ifndef GRID_WIDGET_H_
#define GRID_WIDGET_H_

#include "Object.h"
#include <stdbool.h>

// A GridWidget is an Object subclass implementing Widget and Node. It holds
// other widgets and spaces them out properly. The user can navigate in a grid
// using the arrow keys.

struct GridWidget;
typedef struct GridWidget GridWidget;

// Allocates a grid with the non-negative width and height measured in tiles.
GridWidget *GridWidget_alloc(int width, int height);

// Gets the width in tiles.
int GridWidget_width(const GridWidget *grid);

// Gets the height in tiles.
int GridWidget_height(const GridWidget *grid);

// Places a widget on the grid. The grid takes ownership of the given child
// reference if it is not NULL. The return value is whether the child was
// successfully placed. If so, the widget it took the place of is removed.
// Passing a NULL child deletes the widget at the position. The grid should not
// be modified while it is in focus so as not to mess up the currently focused
// child element.
bool GridWidget_place(GridWidget *grid, int x, int y, Object *child);

#endif /* GRID_WIDGET_H_ */
