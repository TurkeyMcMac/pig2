#include "Object.h"
#include "GridWidget.h"
#include "TextWidget.h"
#include "Widget.h"

int main(void)
{
	initscr();

	GridWidget *grid = GridWidget_alloc(3, 3);
	GridWidget_place(grid, 0, 0, (Object *)TextWidget_alloc("One line"));
	GridWidget_place(grid, 1, 1,
		(Object *)TextWidget_alloc("test\nfoo\nbar\nbaz"));
	GridWidget_place(grid, 2, 2,
		(Object *)TextWidget_alloc("Blah blah blah"));
	GridWidget_place(grid, 0, 2,
		(Object *)TextWidget_alloc("The quick brown fox"));
	struct widget_pair pos = { .x = 0, .y = 0 };
	struct widget_pair dims = { .x = COLS, .y = LINES };
	const struct Widget_impl *impl = PIG2_GET(grid, Widget_iid);
	struct widget_pair req_dims, req_min_dims;
	impl->get_requested_dims(grid, &req_dims, &req_min_dims);
	impl->draw(grid, stdscr, pos, dims);
	getch();
	Object_remove_ref((Object *)grid);

	endwin();
}
