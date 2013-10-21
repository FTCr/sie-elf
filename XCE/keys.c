#include <swilib.h>
#include <stdlib.h>
#include "rect_patcher.h"

//RED BUTTON NOT SUPPORTED!

typedef struct
{
	WSHDR *ws;
	unsigned int gui_id;
	int wait;
	char *key; //ptr on data
} keys_s;

extern void UpdateEditControl(const char *data, const int max_len, void (*callback_ws)(WSHDR *ws));

#define WAIT_STR   "Press any key"
#define NOWAIT_STR "Press ENTER_BUTTON or LEFT_SOFT to accept"

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

void GetKeyName(char *keyname, const int keycode)
{
	if (keycode >= '0' && keycode <= '9')
		sprintf(keyname, "%c", keycode);
	else if (keycode == LEFT_SOFT)
		strcpy(keyname, "LEFT_SOFT");
	else if (keycode == GREEN_BUTTON)
		strcpy(keyname, "GREEN_BUTTON");
	else if (keycode == RIGHT_SOFT)
		strcpy(keyname, "RIGHT_SOFT");
	else if (keycode == ENTER_BUTTON)
		strcpy(keyname, "ENTER_BUTTON");
	else if (keycode == UP_BUTTON)
		strcpy(keyname, "UP_BUTTON");
	else if (keycode == DOWN_BUTTON)
		strcpy(keyname, "DOWN_BUTTON");
	else if (keycode == LEFT_BUTTON)
		strcpy(keyname, "LEFT_BUTTON");
	else if (keycode == RIGHT_BUTTON)
		strcpy(keyname, "RIGHT_BUTTON");
	else if (keycode == '*')
		strcpy(keyname, "*");
	else if (keycode == '#')
		strcpy(keyname, "#");
	else if (keycode == VOL_UP_BUTTON)
		strcpy(keyname, "VOL_UP_BUTTON");
	else if (keycode == VOL_DOWN_BUTTON)
		strcpy(keyname, "VOL_DOWN_BUTTON");
}

static int OnKey(GUI *gui, GUI_MSG *msg)
{
	int style = msg->gbsmsg->msg;
	int key   = msg->gbsmsg->submess;
	if (style == LONG_PRESS)
	{
		if (key == RIGHT_SOFT) return 1;
	}
	else if (style == KEY_UP)
	{
		keys_s *keys = EDIT_GetUserPointer(gui);
		static char keyname[32];
		if (keys->wait)
		{
			keys->wait = 0;
			GetKeyName(keyname, key);
			str_2ws(keys->ws, keyname, 31);
			EDIT_SetTextToEditControl(gui, 3, keys->ws);
			str_2ws(keys->ws, NOWAIT_STR, 127);
			EDIT_SetTextToEditControl(gui, 1, keys->ws);
			RefreshGUI();
		}
		else
		{
			if (key == RIGHT_SOFT)
			{
				keys->wait = 1;
				str_2ws(keys->ws, keys->key, 31);
				EDIT_SetTextToEditControl(gui, 3, keys->ws);
				str_2ws(keys->ws, WAIT_STR, 127);
				EDIT_SetTextToEditControl(gui, 1, keys->ws);
				RefreshGUI();
			}
			else if (key == LEFT_SOFT || key == ENTER_BUTTON)
			{
				strcpy(keys->key, keyname);
				UpdateEditControl(keys->key, 31, NULL);
				return 1;
			}
		}
	}
	return 0;
}

static void GHook(GUI *gui, int cmd)
{
	keys_s *keys = EDIT_GetUserPointer(gui);
	if (cmd == TI_CMD_REDRAW)
	{
		SOFTKEY_DESC sk_lb = {0x0FFF, 0x0000, 0};
		SOFTKEY_DESC sk_rb = {0x0FF0, 0x0000, 0};
		if (keys->wait == 0)
		{
			sk_lb.lgp_id = (int)"OK";
			sk[2].lgp_id = (int)LGP_DOIT_PIC;
			sk_rb.lgp_id = (int)"Cancel";
			SetSoftKey(gui, &sk[2], SET_SOFT_KEY_M);
		}
		else
		{
			sk_lb.lgp_id = (int)"";
			sk_rb.lgp_id = (int)"";
		}
		SetSoftKey(gui, &sk_lb, SET_SOFT_KEY_N);
		SetSoftKey(gui, &sk_rb, (SET_SOFT_KEY_N == 0) ? 1 : 0);
	}
	if (cmd == TI_CMD_DESTROY)
	{
		FreeWS(keys->ws);
		mfree(keys);
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
	0x00000000,
	0x00000002,
	0x40000000
};

void CreateKeyGUI(const char *hdr, const char *key)
{
	void *ma = malloc_adr();
	void *eq = AllocEQueue(ma, mfree_adr());
	
	EDITCONTROL ec;
	PrepareEditControl(&ec);
	
	header.lgp_id = (int)hdr;
	keys_s *keys = malloc(sizeof(keys_s));
	keys->ws = AllocWS(128);
	keys->wait = 1;
	keys->key = (char*)key;
	
	str_2ws(keys->ws, WAIT_STR, 127);
	ConstructEditControl(&ec, ECT_HEADER, ECF_APPEND_EOL, keys->ws, 64);
	AddEditControlToEditQend(eq, &ec, ma);
	
	str_2ws(keys->ws, " ", 1);
	ConstructEditControl(&ec, ECT_HEADER, ECF_APPEND_EOL, keys->ws, keys->ws->wsbody[0]);
	AddEditControlToEditQend(eq, &ec, ma);
	
	str_2ws(keys->ws, key, 31);
	ConstructEditControl(&ec, ECT_HEADER, ECF_APPEND_EOL, keys->ws, 32);
	AddEditControlToEditQend(eq, &ec, ma);
	
	patch_header(&header);
	patch_input(&desc);
	keys->gui_id = CreateInputTextDialog(&desc, &header, eq, 1, keys);
}
