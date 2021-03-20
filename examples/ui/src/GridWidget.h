#ifndef GRID_WIDGET_H_
#define GRID_WIDGET_H_

#include "Object.h"
#include <stdbool.h>

struct GridWidget;
typedef struct GridWidget GridWidget;

GridWidget *GridWidget_alloc(int width, int height);

int GridWidget_width(const GridWidget *grid);

int GridWidget_height(const GridWidget *grid);

bool GridWidget_place(GridWidget *grid, int x, int y, Object *child);

#endif /* GRID_WIDGET_H_ */
