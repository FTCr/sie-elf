#include <swilib.h>
#include <stdlib.h>
#include "coordinates.h"

typedef struct
{
	GUI gui;
	WSHDR *ws;
	int type;
	int x, x2;
	int y, y2;
	int x_pos, y_pos;
	int rect_state;
}MAIN_GUI;

extern void UpdateEditControl(const char *data, const int max_len, void (*callback_ws)(WSHDR *ws));

RECT Canvas = {0, 0, 0, 0};

void DrawCursor(int x, int y)
{
	DrawLine(x - 2, y, x + 2, y, 0, GetPaletteAdrByColorIndex(1));
	DrawLine(x, y - 2, x, y + 2, 0, GetPaletteAdrByColorIndex(1));
}

static void OnRedraw(MAIN_GUI *main_gui)
{ 
	const char lines_col[4] = {0xC6, 0xAA, 0xAF, 0x32};
	DrawRectangle(0, 0, Canvas.x2, Canvas.y2, 0, GetPaletteAdrByColorIndex(0), GetPaletteAdrByColorIndex(0));
	for (int i = 0; i < Canvas.y2; i += 10)
	{
		DrawLine(i, 0, i, Canvas.y2, 1, lines_col);
		DrawLine(0, i, Canvas.x2, i, 1, lines_col);
	}
	if (main_gui->type == COORD_T_POSITION)
	{
		main_gui->x = main_gui->x_pos;
		main_gui->y = main_gui->y_pos;
		
		wsprintf(main_gui->ws, "%d,%d", main_gui->x, main_gui->y);
		DrawString(main_gui->ws, 0, Canvas.y2 - GetFontYSIZE(FONT_SMALL), Canvas.x2, Canvas.y2, FONT_SMALL, TEXT_ALIGNLEFT,
			GetPaletteAdrByColorIndex(1), GetPaletteAdrByColorIndex(23));
		
		DrawCursor(main_gui->x, main_gui->y);
	}
	else
	{
		
		switch (main_gui->rect_state)
		{
			case 0:
				main_gui->x = main_gui->x2 = main_gui->x_pos;
				main_gui->y = main_gui->y2 = main_gui->y_pos;
				wsprintf(main_gui->ws, "%d,%d;%d,%d", main_gui->x, main_gui->y, main_gui->x2, main_gui->y2);
			break;
			case 1:
				main_gui->x2 = main_gui->x_pos;
				main_gui->y2 = main_gui->y_pos;
				wsprintf(main_gui->ws, "%d,%d;%d,%d", main_gui->x, main_gui->y, main_gui->x2, main_gui->y2);
			break;
			case 2:
				wsprintf(main_gui->ws, "%d,%d;%d,%d", main_gui->x_pos, main_gui->y_pos, main_gui->x2, main_gui->y2);
		}
		DrawString(main_gui->ws, 0, Canvas.y2 - GetFontYSIZE(FONT_SMALL), Canvas.x2, Canvas.y2, FONT_SMALL, TEXT_ALIGNLEFT,
			GetPaletteAdrByColorIndex(1), GetPaletteAdrByColorIndex(23));
		
		if (main_gui->rect_state != 0)
		{
			DrawLine(main_gui->x, main_gui->y, main_gui->x2, main_gui->y, 1, GetPaletteAdrByColorIndex(1));
			DrawLine(main_gui->x2, main_gui->y, main_gui->x2, main_gui->y2, 1, GetPaletteAdrByColorIndex(1));
			DrawLine(main_gui->x2, main_gui->y2, main_gui->x, main_gui->y2, 1, GetPaletteAdrByColorIndex(1));
			DrawLine(main_gui->x, main_gui->y2, main_gui->x, main_gui->y, 1, GetPaletteAdrByColorIndex(1));
		}
		DrawCursor(main_gui->x_pos, main_gui->y_pos);
	}
}

static void OnCreate(MAIN_GUI *main_gui, void *(*malloc_adr)(int))
{
	main_gui->ws = AllocWS(32);
	main_gui->gui.state = 1;
	main_gui->x_pos = main_gui->x;
	main_gui->y_pos = main_gui->y;
	main_gui->rect_state  = 2;
}

static void OnClose(MAIN_GUI *main_gui, void (*mfree_adr)(void*))
{
	FreeWS(main_gui->ws);
	main_gui->gui.state = 0;
}

static void OnFocus(MAIN_GUI *main_gui, void *(*malloc_adr)(int), void (*mfree_adr)(void*))
{
#ifdef ELKA
	DisableIconBar(1);
#endif
	main_gui->gui.state = 2;
}

static void OnUnfocus(MAIN_GUI *main_gui, void (*mfree_adr)(void*))
{
	if (main_gui->gui.state != 2) return;
#ifdef ELKA
	DisableIconBar(0);
#endif
	main_gui->gui.state = 1;
}

#define L_STEP 10
#define l(a)   a -= step; if (a < 0) a = 0;
#define h(a,b) a += step; if (a > b) a = b; 

static int OnKey(MAIN_GUI *main_gui, GUI_MSG *msg)
{
	int key   = msg->gbsmsg->submess;
	int style = msg->gbsmsg->msg;
	const int step = (style == KEY_DOWN) ? 1 : L_STEP;
	
	if (style == LONG_PRESS || style == KEY_DOWN)
	{
		char data[16];
		switch (key)
		{
			case RIGHT_SOFT:
				return 1;
			break;
			case ENTER_BUTTON:
				if (main_gui->type == COORD_T_POSITION)
				{
					sprintf(data, "%d,%d", main_gui->x, main_gui->y);
					UpdateEditControl(data, 8, NULL);
					return 1;
				}
				else
				{
					switch (main_gui->rect_state)
					{
						case 0:
							main_gui->rect_state = 1;
							DirectRedrawGUI();
						break;
						case 1:
							sprintf(data, "%d,%d;%d,%d", min(main_gui->x, main_gui->x2), min(main_gui->y, main_gui->y2),
								max(main_gui->x, main_gui->x2), max(main_gui->y, main_gui->y2));
							UpdateEditControl(data, 16, NULL);
							return 1;
						break;
						case 2:
							main_gui->rect_state = 0;
							DirectRedrawGUI();
							main_gui->rect_state = 1;
						break;
					}
				}
			break;
			case LEFT_BUTTON: case '4':
				l(main_gui->x_pos);
				DirectRedrawGUI();
			break;
			case RIGHT_BUTTON: case '6':
				h(main_gui->x_pos, Canvas.x2);
				DirectRedrawGUI();
			break;
			case UP_BUTTON: case '2':
				l(main_gui->y_pos);
				DirectRedrawGUI();
			break;
			case DOWN_BUTTON: case '8':
				h(main_gui->y_pos, Canvas.y2);
				DirectRedrawGUI();
			break;
			case '1':
				l(main_gui->x_pos);
				l(main_gui->y_pos);
				DirectRedrawGUI();
			break;
			case '3':
				h(main_gui->x_pos, Canvas.x2);
				l(main_gui->y_pos);
				DirectRedrawGUI();
			break;
			case '7':
				l(main_gui->x_pos);
				h(main_gui->y_pos, Canvas.y2);
				DirectRedrawGUI();
			break;
			case '9':
				h(main_gui->x_pos, Canvas.x2);
				h(main_gui->y_pos, Canvas.y2);
				DirectRedrawGUI();
			break;
		}
	}
	return 0;
}

static int method8(void){return(0);}

static int method9(void){return(0);}

extern void kill_data(void *p, void (*func_p)(void *));

static const void * const gui_methods[11] = {
	(void*)OnRedraw,
	(void*)OnCreate,
	(void*)OnClose,
	(void*)OnFocus,
	(void*)OnUnfocus,
	(void*)OnKey,
	0,
	(void*)kill_data,
	(void*)method8,
	(void*)method9,
	0
};

void CreateCoordinatesGUI(const int x, const int y, const int x2, const int y2, const int type)
{
	MAIN_GUI *main_gui = malloc(sizeof(MAIN_GUI));
	zeromem(main_gui, sizeof(MAIN_GUI));
	Canvas.x2 = ScreenW() - 1;
	Canvas.y2 = ScreenH() - 1;
	main_gui->gui.canvas = (void*)(&Canvas);
	main_gui->gui.flag30 = 2;
	main_gui->gui.methods = (void*)gui_methods;
	main_gui->gui.item_ll.data_mfree = (void (*)(void *))mfree_adr();
	main_gui->type = type;
	main_gui->x = x;
	main_gui->y = y;
	main_gui->x2 = x2;
	main_gui->y2 = y2;
	CreateGUI(main_gui);
}
