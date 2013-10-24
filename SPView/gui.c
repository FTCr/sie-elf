#include <swilib.h>
#include <stdlib.h>
#include "rect_patcher.h"

WSHDR *ws;
int icon_id;

HEADER_DESC header = {0, 0, 0, 0, 0, (int)"SPView", LGP_NULL};

SOFTKEY_DESC sk[] =
{
	{0x0018, 0x0000, (int)"Go to..."},
	{0x0001, 0x0000, (int)"Quit"},
	{0x003D, 0x0000, (int)LGP_DOIT_PIC}
};

SOFTKEYSTAB skt=
{
	sk, 0
};

void OnRedraw(void)
{
	int w = GetImgWidth(icon_id);
	int h = GetImgHeight(icon_id);
	
	if (w && h)
	{
		int x = (ScreenW() - 1 - w) / 2;
		int y = (YDISP + HeaderH()) + (ScreenH() - 1 - YDISP - HeaderH() - SoftkeyH() - h) / 2;
		DrawImg(x, y, icon_id);
		DrawRectangle(x - 1, y - 1, x + w, y + h, 1, GetPaletteAdrByColorIndex(0), GetPaletteAdrByColorIndex(23));
		wsprintf(ws, "%dx%d", w, h);
		DrawString(ws, 4, YDISP + HeaderH() + 2, ScreenW() - 1, ScreenH() - 1, FONT_SMALL, TEXT_ALIGNLEFT + TEXT_OUTLINE, GetPaletteAdrByColorIndex(0),
			GetPaletteAdrByColorIndex(1));
	}
}

void UpdateGUI()
{
	static char num[8];
	sprintf(num, "%d", icon_id);
	header.lgp_null = (int)num;
	RefreshGUI();
}

#define BIG_STEP 10

int OnKey(GUI *gui, GUI_MSG *msg)
{
	int key   = msg->gbsmsg->submess;
	int style = msg->gbsmsg->msg;
	
	if (msg->keys == 0x0FFF)
	{
		void CreateGoToGUI(void);
		CreateGoToGUI();
	}
	
	if (style == KEY_DOWN)
	{
		if (key == LEFT_BUTTON)
		{
			if (icon_id) icon_id--;
			UpdateGUI();
		}
		else if (key == RIGHT_BUTTON)
		{
			if (icon_id != 9999) icon_id++;
			UpdateGUI();
		}
	}
	else if (style == LONG_PRESS)
	{
		if (key == LEFT_BUTTON)
		{
			if (icon_id - BIG_STEP >= 0) icon_id -= BIG_STEP;
			else if (icon_id) icon_id--;
			UpdateGUI();
		}
		else if (key == RIGHT_BUTTON)
		{
			if (icon_id + BIG_STEP <= 9999) icon_id += BIG_STEP;
			else if (icon_id != 9999) icon_id++;
			UpdateGUI();
		}
	}
	
	return 0;
}


void GHook(GUI *gui, int cmd)
{
	if (cmd == TI_CMD_CREATE)
	{
		static void *methods[16];
		//Called after onCreate
		void **m = GetDataOfItemByID(gui, 4);
		memcpy(methods, m[1], sizeof(methods));
		methods[0] = (void *)OnRedraw;
		m[1] = methods;
	}
	if (cmd == TI_CMD_REDRAW)
	{
		SOFTKEY_DESC sk_lb = {0x0FFF, 0x0000, 0};
		sk_lb.lgp_id = (int)"Go to...";
		SetSoftKey(gui, &sk_lb, SET_SOFT_KEY_N);
		SetSoftKey(gui, &sk[1], (SET_SOFT_KEY_N == 0) ? 1 : 0);
	}
	if (cmd == TI_CMD_FOCUS)
	{
		UpdateGUI();
	}
	if (cmd == TI_CMD_DESTROY)
	{
		FreeWS(ws);
	}
}

INPUTDIA_DESC desc=
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

int CreateMyGUI(void)
{
	void *ma = malloc_adr();
	void *eq = AllocEQueue(ma, mfree_adr());
	
	EDITCONTROL ec;
	PrepareEditControl(&ec);
	
	ws = AllocWS(64);
	ConstructEditControl(&ec, ECT_HEADER, ECF_APPEND_EOL, ws, 1);
	AddEditControlToEditQend(eq, &ec, ma);
	
	patch_header(&header);
	patch_input(&desc);
	return CreateInputTextDialog(&desc, &header, eq, 1, NULL);
}

//----------------------------------------------------------Go to..-----------------------------------------------------------

HEADER_DESC goto_header = {0, 0, 0, 0, 0, (int)"Go to...", LGP_NULL};

int GoTo_OnKey(GUI *gui, GUI_MSG *msg)
{
	if (msg->keys == 0x0018)
	{
		EDITCONTROL ec;
		EDIT_ExtractFocusedControl(gui, &ec);
		char num[4];
		ws_2str(ec.pWS, num, 4);
		icon_id = atoi(num);
		return 1;
	}
	return 0;
}

void GoTo_GHook(GUI *gui, int cmd)
{
	if (cmd == TI_CMD_REDRAW)
	{
		sk[0].lgp_id = (int)"OK";
		SetSoftKey(gui, &sk[0], SET_SOFT_KEY_N);
	}
}

INPUTDIA_DESC goto_desc=
{
	1,
	GoTo_OnKey,
	GoTo_GHook,
	NULL,
	0,
	&skt,
	{0, 0, 0, 0},
	FONT_MEDIUM,
	100,
	101,
	0,
	0,
	0x40000000
};

void CreateGoToGUI(void)
{
	void *ma = malloc_adr();
	void *eq = AllocEQueue(ma, mfree_adr());
	
	EDITCONTROL ec;
	PrepareEditControl(&ec);
	
	wsprintf(ws, "%d", icon_id);
	ConstructEditControl(&ec, ECT_NORMAL_NUM, ECF_DISABLE_EOLCHAR, ws, 4);
	AddEditControlToEditQend(eq, &ec, ma);
	
	patch_header(&goto_header);
	patch_input(&goto_desc);
	CreateInputTextDialog(&goto_desc, &goto_header, eq, 1, NULL);
}
