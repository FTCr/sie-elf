#include <swilib.h>
#include "color.h"
#include "rect_patcher.h"

WSHDR *ws;
int COLOR[4];
int cursor;

extern void UpdateEditControl(const char *data, const int max_len, void (*callback_ws)(WSHDR *ws));

static HEADER_DESC header = {0, 0, 0, 0, 0, 0, LGP_NULL};

static SOFTKEY_DESC sk[] =
{
	{0x0018, 0x0000, (int)"OK"},
	{0x0001, 0x0000, (int)"Back"},
	{0x003D, 0x0000, (int)LGP_DOIT_PIC}
};

static SOFTKEYSTAB skt =
{
	sk, 0
};

inline unsigned int us_reverse(unsigned int v)
{
	return((v>>8)|((v&0xFF)<<8));
}

void ws_set_color(WSHDR *ws_to, const char *col)
{
	wsInsertChar(ws_to, us_reverse(((unsigned short *)col)[1]), 1);
	wsInsertChar(ws_to, us_reverse(((unsigned short *)col)[0]), 1);
	wsInsertChar(ws_to, 0xE006, 1);
}

void get_color_from_str(char *color, const char *str_col) //:-) happy ARM user
{
	int c[4];
	sscanf(str_col, "%02x%02x%02x02x", &c[0], &c[1], &c[2], &c[3]);
	color[0] = c[0]; color[1] = c[1]; color[2] = c[2]; color[3] = c[3];
}

static void OnRedraw(void)
{
	const char c[4] = {COLOR[0], COLOR[1], COLOR[2], COLOR[3]};
	const int s = GetFontYSIZE(FONT_SMALL) * 2;
	const char RGBA[4] = {'R', 'G', 'B', 'A'};
	
	int y = YDISP + HeaderH() + 4;
	
	DrawRectangle(4, y, ScreenW() - 1 - 4, y + s, 0, c, c);
	wsprintf(ws, "%c: %02X%02X%02X%02X", RGBA[cursor], COLOR[0], COLOR[1], COLOR[2], COLOR[3]);
	DrawString(ws, 4, y + GetFontYSIZE(FONT_SMALL) / 2, ScreenW() - 1 - 4, y + GetFontYSIZE(FONT_SMALL) / 2 + GetFontYSIZE(FONT_SMALL),
		FONT_SMALL, TEXT_ALIGNMIDDLE + TEXT_OUTLINE, GetPaletteAdrByColorIndex(0), GetPaletteAdrByColorIndex(1));
	y += s + 8;
	
	int x;
	int y2 = ScreenH() - SoftkeyH() - 1;
	const int offset_w = (ScreenW() - 1 - 8 - s * 4) / 3;
	const int p_i[4] = {2, 4, 3, 23};
	int attr;
	float step;
	for (int i = 0; i < 4; i++)
	{
		if (cursor == i)
		{
			attr = 0;
		}
		else
		{
			attr = 1;
		}
		x = 4 + (s + offset_w) * i;
		DrawRectangle(x, y, x + s, y2, attr,
			GetPaletteAdrByColorIndex(0), GetPaletteAdrByColorIndex(p_i[i]));
		
		if (i < 3)
			step = (y2 - y) / (float)0xFF;
		else
			step = (y2 - y) / (float)0x64;
		DrawLine(x, y2 - step * COLOR[i], x + s, y2 - step * COLOR[i], attr, GetPaletteAdrByColorIndex(0));
	}
}

void update_pWS(WSHDR *pWS)
{
	unsigned char c[4] = {COLOR[0], COLOR[1], COLOR[2], COLOR[3]};
	wsprintf(pWS, "%02X%02X%02X%02X", COLOR[0], COLOR[1], COLOR[2], COLOR[3]);
	ws_set_color(pWS, c);
}

static int OnKey(GUI *gui, GUI_MSG *msg)
{
	if (msg->keys == sk[2].key1 || msg->keys == sk[0].key1)
	{
		char data[8];
		sprintf(data, "%02X%02X%02X%02X", COLOR[0], COLOR[1], COLOR[2], COLOR[3]);
		str_2ws(ws, data, 8);
		UpdateEditControl(data, 8, update_pWS);
		return 1;
	}
	
	int style = msg->gbsmsg->msg;
	int key   = msg->gbsmsg->submess;
	
	if (style == KEY_DOWN)
	{
		if (key == LEFT_BUTTON) goto LB;
		else if (key == RIGHT_BUTTON) goto RB;
		else if (key == UP_BUTTON)
		{
			COLOR[cursor]++;
			goto UB;
		}
		else if (key == DOWN_BUTTON)
		{
			COLOR[cursor]--;
			goto DB;
		}
	}
	else if (style == LONG_PRESS)
	{
		if (key == LEFT_BUTTON)
		{
			LB:
				cursor = (cursor == 0) ? 4 - 1 : --cursor;
				DirectRedrawGUI();
		}
		else if (key == RIGHT_BUTTON)
		{
			RB:
				cursor = (cursor == 4 - 1) ? 0 : ++cursor;
				DirectRedrawGUI();
		}
		else if (key == UP_BUTTON)
		{
			COLOR[cursor] += 10;
			UB:
				if (cursor == 4 - 1)
				{
					if (COLOR[cursor] > 0x64)
						COLOR[cursor] = 0x00;
				}
				else
				{
					if (COLOR[cursor] > 0xFF)
						COLOR[cursor] = 0x00;
				}
				DirectRedrawGUI();
		}
		else if (key == DOWN_BUTTON)
		{
			COLOR[cursor] -= 10;
			DB:
				if (COLOR[cursor] < 0x00)
					COLOR[cursor] = (cursor == 4 - 1) ? 0x64 : 0xFF;
				DirectRedrawGUI();
		}
	}
	
	return 0;
}

static void GHook(GUI *gui, int cmd)
{
	if (cmd == TI_CMD_CREATE)
	{
		//Hook OnRedraw method
		static void *methods[16];
		void **m = GetDataOfItemByID(gui, 4);
		memcpy(methods, m[1], sizeof(methods));
		methods[0] = (void *)OnRedraw;
		m[1] = methods;
	}
	if (cmd == TI_CMD_REDRAW)
	{
		SetSoftKey(gui, &sk[0], SET_SOFT_KEY_N);
		SetSoftKey(gui, &sk[1], (SET_SOFT_KEY_N == 1) ? 0 : 1);
		SetSoftKey(gui, &sk[2], SET_SOFT_KEY_M);
	}
	if (cmd == TI_CMD_DESTROY)
	{
		FreeWS(ws);
		cursor = 0;
	}
}


static INPUTDIA_DESC desc=
{
	1,
	OnKey,
	GHook,
	NULL,
	0,
	&skt,
	{0, 0, 0, 0},
	FONT_SMALL,
	100,
	101,
	0,
	0,
	0x40000000
};


void CreateColorGUI(const char *hdr, const int *color)
{
	void *ma = malloc_adr();
	void *eq = AllocEQueue(ma, mfree_adr());
	
	EDITCONTROL ec;
	PrepareEditControl(&ec);
	
	ws = AllocWS(256);
	header.lgp_id = (int)hdr;
	for (int i = 0; i < 4; COLOR[i] = color[i], i++);
	
	ConstructEditControl(&ec, ECT_HEADER, ECF_APPEND_EOL, ws, 1);
	AddEditControlToEditQend(eq, &ec, ma);
	
	patch_header(&header);
	patch_input(&desc);
	CreateInputTextDialog(&desc, &header, eq, 1, NULL);
}
