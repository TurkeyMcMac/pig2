#include "Object.h"
#include "GridWidget.h"
#include "SpacerWidget.h"
#include "TextWidget.h"
#include "Widget.h"

int main(void)
{
	initscr();
	cbreak();
	noecho();
	curs_set(0);
	keypad(stdscr, TRUE);

	GridWidget *grid = GridWidget_alloc(7, 7);
	SpacerWidget *sw = SpacerWidget_alloc((struct widget_pair) { 5, 3 },
		(struct widget_pair) { 2, 1 });
	GridWidget_place(grid, 0, 0, Object_add_ref((Object *)sw));
	GridWidget_place(grid, 2, 2, Object_add_ref((Object *)sw));
	GridWidget_place(grid, 4, 4, Object_add_ref((Object *)sw));
	GridWidget_place(grid, 6, 6, (Object *)sw);
	GridWidget_place(grid, 3, 5, (Object *)TextWidget_alloc("One line"));
	GridWidget_place(grid, 3, 3,
		(Object *)TextWidget_alloc("test\nfoo\nbar\nbaz"));
	GridWidget_place(grid, 5, 5,
		(Object *)TextWidget_alloc("Blah blah blah"));
	GridWidget_place(grid, 1, 5,
		(Object *)TextWidget_alloc("The quick brown fox"));
	GridWidget *grid2 = GridWidget_alloc(2, 2);
	GridWidget_place(grid2, 0, 0,
		(Object *)TextWidget_alloc("Subgrid foo"));
	GridWidget_place(grid2, 1, 1,
		(Object *)TextWidget_alloc("Subgrid bar"));
	GridWidget_place(grid, 1, 1, (Object *)grid2);
	for (;;) {
		erase();
		struct widget_pair pos = { .x = 0, .y = 0 };
		struct widget_pair dims = { .x = COLS, .y = LINES };
		const struct Widget_impl *impl = PIG2_GET(grid, Widget_iid);
		struct widget_pair req_dims, req_min_dims;
		impl->get_requested_dims(grid, &req_dims, &req_min_dims);
		impl->draw(grid, stdscr, pos, dims);
		switch (getch()) {
		case 'q':
			goto end;
		}
	}
end:
	Object_remove_ref((Object *)grid);

	endwin();
}
