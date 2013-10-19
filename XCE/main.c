#include <swilib.h>
#include <stdlib.h>
#include <mxml.h>
#include "rect_patcher.h"
#include "color.h"
#include "coordinates.h"
#include "keys.h"

#define CONFIG_VERSION "1.0"

#define TAG_INTEGER  0x01
#define TAG_STRING   0x02
#define TAG_CHECKBOX 0x03
#define TAG_COLOR    0x04
#define TAG_COMBOBOX 0x05
#define TAG_POSITION 0x06
#define TAG_RECT     0x07
#define TAG_KEY      0x08
#define TAG_SUBMENU  0x09

typedef struct
{
	char name[32];
	char var[256];
	int type;
	mxml_node_t *node;
}ITEM;

typedef struct
{
	INPUTDIA_DESC input_desc;
	HEADER_DESC hdr_desc;
	ITEM *items;
	int n_items;
	void *next;
	void *prev;
}INPUT_GUI;

typedef struct
{
	CSM_RAM csm;
	INPUT_GUI *gui;
	int gui_id;
}MAIN_CSM;

unsigned int CreateDialog(const char *node_name, INPUT_GUI **gui);
void SaveNodes(GUI *gui, INPUT_GUI **inp_gui);

mxml_node_t *xml_tree;
char FILE_PATH[256];

//For UpdateEditControl
GUI *p_cur_ed_gui;
char *p_cur_var;

void UpdateEditControl(const char *data, const int max_len, void (*callback_ws)(WSHDR *ws))
{
	EDITCONTROL ec;
	EDIT_ExtractFocusedControl(p_cur_ed_gui, &ec);
	if (callback_ws == NULL)
		str_2ws(ec.pWS, data, max_len);
	else
		callback_ws(ec.pWS);
	strcpy(p_cur_var, data);
	EDIT_StoreControlToFocused(p_cur_ed_gui, &ec);
}

SOFTKEY_DESC sk[]=
{
	{0x0018, 0x0000, (int)"OK"},
	{0x0001, 0x0000, (int)"Back"},
	{0x003D, 0x0000, (int)LGP_DOIT_PIC}
};

#define L_SOFT   sk[0].key1
#define E_BUTTON sk[2].key1

SOFTKEYSTAB skt =
{
	sk, 0
};

static int GetType(const char *tag)
{
	if (strcmp(tag, "integer") == 0)       return TAG_INTEGER;
	else if (strcmp(tag, "string") == 0)   return TAG_STRING;
	else if (strcmp(tag, "checkbox") == 0) return TAG_CHECKBOX;
	else if (strcmp(tag, "color") == 0)    return TAG_COLOR;
	else if (strcmp(tag, "combobox") == 0) return TAG_COMBOBOX;
	else if (strcmp(tag, "position") == 0) return TAG_POSITION;
	else if (strcmp(tag, "rect") == 0)     return TAG_RECT;
	else if (strcmp(tag, "submenu") == 0)  return TAG_SUBMENU;
	else if (strcmp(tag, "key") == 0)      return TAG_KEY;
	else return -1;
}

static void GHook(GUI *gui, int cmd)
{
	INPUT_GUI **inp_gui = EDIT_GetUserPointer(gui);
	INPUT_GUI *inp_g    = (*inp_gui);
	int i = EDIT_GetFocus(gui) / 2 - 1;
	ITEM *item = &inp_g->items[i];
	
	if (cmd == TI_CMD_REDRAW)
	{
		if (item->type != TAG_INTEGER && item->type != TAG_STRING)
		{
			if (inp_g->prev == NULL)
				sk[1].lgp_id = (int)"Exit";
			else if (item->type != TAG_INTEGER)
				sk[1].lgp_id = (int)"Back";
			SetSoftKey(gui, &sk[1], (SET_SOFT_KEY_N == 0) ? 1 : 0);
		}
		
		SetSoftKey(gui, &sk[0], SET_SOFT_KEY_N);
	}
	if (cmd == TI_CMD_DESTROY)
	{
		//Delete GUI
		if ((*inp_gui)->items)
			mfree((*inp_gui)->items);
		mfree((*inp_gui));
		INPUT_GUI *p = inp_g->prev;
		if (p)
		{
			p->next = NULL;
			(*inp_gui) = p;
		}
	}
	if (cmd == TI_CMD_COMBOBOX_FOCUS)
	{
		int n = EDIT_GetItemNumInFocusedComboBox(gui);
		WSHDR *ws = AllocWS(32);
		if (n)
		{
			char attr_name[16];
			sprintf(attr_name, "item%d", n);
			const char *attr = mxmlElementGetAttr(item->node, attr_name);
			utf8_2ws(ws, attr, 32);
			EDIT_SetTextToFocused(gui, ws);
		}
		else
		{
			EDITCONTROL ec;
			ExtractEditControl(gui, EDIT_GetFocus(gui) - 1, &ec);
			EDIT_SetTextToFocused(gui, ec.pWS);
		}
		
		FreeWS(ws);
	}
}


static int OnKey(GUI *gui, GUI_MSG *msg)
{
	int key   = msg->gbsmsg->submess;
	int style = msg->gbsmsg->msg;
	
	EDITCONTROL ec;
	INPUT_GUI **g = EDIT_GetUserPointer(gui);
	int i = EDIT_GetFocus(gui) / 2 -1;
	ITEM *item= &((*g)->items[i]);
	if (msg->keys == E_BUTTON)
	{
		if (item->type == TAG_SUBMENU)
		{
			INPUT_GUI *ptr;
			//Add new element in GUI list
			(*g)->next = malloc(sizeof(INPUT_GUI));
			ptr = (*g);
			(*g) = (*g)->next;
			(*g)->next = NULL;
			(*g)->items = NULL;
			(*g)->prev = ptr;
			
			EDIT_ExtractFocusedControl(gui, &ec);
			if (!CreateDialog(item->var, g))
			{
				(*g) = (*g)->prev;
				mfree(ptr->next);
				ptr->next = NULL;
			}
		}
		else if (item->type == TAG_CHECKBOX)
		{
			char str[3];
			EDIT_ExtractFocusedControl(gui, &ec);
			ws_2str(ec.pWS, str, 3);
			if (strcmp(str, "[x]") == 0)
			{
				str_2ws(ec.pWS, "[  ]", 4);
				//ec.pWS->wsbody[2] = 32; //whitespace
				strcpy(item->var, "false");
			}
			else
			{
				str_2ws(ec.pWS, "[x]", 3);
				strcpy(item->var, "true");
			}
			RefreshGUI();
		}
		else if (item->type == TAG_COLOR)
		{
			p_cur_ed_gui = gui;
			p_cur_var = item->var;
			
			int color[4];
			sscanf(item->var, "%02X%02X%02X%02X", &color[0], &color[1], &color[2], &color[3]);
			CreateColorGUI(item->name, color);
		}
		else if (item->type == TAG_POSITION)
		{
			p_cur_ed_gui = gui;
			p_cur_var = item->var;
			
			int x, y;
			sscanf(item->var, "%d,%d", &x, &y);
			CreateCoordinatesGUI(x, y, 0, 0, COORD_T_POSITION);
		}
		else if (item->type == TAG_RECT)
		{
			p_cur_ed_gui = gui;
			p_cur_var = item->var;
			
			int x, y, x2, y2;
			sscanf(item->var, "%d,%d;%d,%d", &x, &y, &x2, &y2);
			CreateCoordinatesGUI(x, y, x2, y2, COORD_T_RECT);
		}
		else if (item->type == TAG_KEY)
		{
			p_cur_ed_gui = gui;
			p_cur_var = item->var;
			CreateKeyGUI(item->name, item->var);
		}
	}
	else if (msg->keys == L_SOFT)
	{
		SaveNodes(gui, g);
		if ((*g)->prev == NULL)
		{
			FILE *f = fopen(FILE_PATH, "w");
			if (f)
			{
				mxmlSaveFile(xml_tree, f, NULL);
				fclose(f);
			}
			else
			{
				MsgBoxError(1, (int)"Error save file");
			}
		}
		return 1;
	}
	
	if (style == KEY_DOWN)
	{
		if (key == DOWN_BUTTON || key == UP_BUTTON)
		{
			if (item->type == TAG_INTEGER)
			{
				EDIT_ExtractFocusedControl(gui, &ec);
				if (ec.pWS->wsbody[0] == 0)
				{
					str_2ws(ec.pWS, "0", 1);
				}
			}
			else if (item->type == TAG_STRING)
			{
				EDIT_ExtractFocusedControl(gui, &ec);
				if (ec.pWS->wsbody[0] == 0)
				{
					str_2ws(ec.pWS, "NULL", 4);
				}
			}
		}
	}
	return 0;
}

void SaveNodes(GUI *gui, INPUT_GUI **inp_gui)
{
	INPUT_GUI *g = *inp_gui;
	for (int i = 0; i < g->n_items; i++)
	{
		ITEM *item = &((*inp_gui)->items[i]);
		if (item->type != TAG_SUBMENU)
		{
			EDITCONTROL ec;
			int utf8_l;
			if (item->type == TAG_INTEGER || item->type == TAG_STRING)
			{
				ExtractEditControl(gui, (i + 1) * 2, &ec);
				ws_2utf8(ec.pWS, item->var, &utf8_l, ec.pWS->wsbody[0] * 2);
			}
			else if (item->type == TAG_COMBOBOX)
			{
				char name[8];
				char *attr;
				int n = atoi(mxmlElementGetAttr(item->node, "n"));
				
				char combo_str[256];
				ExtractEditControl(gui, (i + 1) * 2, &ec);
				ws_2utf8(ec.pWS, combo_str, &utf8_l, 255);
				for (int j = 0; j < n; j++)
				{
					sprintf(name, "item%d", j + 1);
					attr = (char*)mxmlElementGetAttr(item->node, name);
					if (attr)
					{
						if (strcmp(combo_str, attr) == 0)
						{
							sprintf(item->var, "%d", j + 1);
							break;
						}
					}
				}
			}
			mxmlSetOpaque(item->node, item->var);
		}
	}
}

unsigned int CreateDialog(const char *node_name, INPUT_GUI **gui)
{
	mxml_node_t *node = mxmlFindElement(xml_tree, xml_tree, node_name, NULL, NULL, MXML_DESCEND);
	if (node == NULL) return 0;
	
	INPUT_GUI *g = *gui;
	
	//Prepare EditControl
	void *ma = malloc_adr();
	void *eq = AllocEQueue(ma, mfree_adr());
	EDITCONTROL ec;
	EDITC_OPTIONS ec_options;
	PrepareEditControl(&ec);
	
	//Prepare INPUTDIA
	zeromem(&(g->input_desc), sizeof(INPUTDIA_DESC));
	g->input_desc.one = 1;
	g->input_desc.onKey = OnKey;
	g->input_desc.global_hook_proc = GHook;
	g->input_desc.softkeystab = &skt;
	//g->input_desc.font = FONT_SMALL;
	g->input_desc._100 = 100;
	g->input_desc._101 = 101;
	g->input_desc._0x40000000 = 0x40000000;
	//Prepare HEADER
	zeromem(&(g->hdr_desc), sizeof(HEADER_DESC));
	g->hdr_desc.lgp_id = (int)"XCE";
	
	//Parse xml file
	WSHDR *ws = AllocWS(256);
	mxml_node_t *top = node;
	int node_type = 0;
	int tag_type = 0;
	int save = false;
	unsigned int i = 0;
	while (node != NULL)
	{
		node_type = mxmlGetType(node);
		if (node_type == MXML_ELEMENT)
		{
			const char *attr;
			tag_type = GetType(mxmlGetElement(node));
			if (tag_type != -1)
			{
				save = true;
				attr = mxmlElementGetAttr(node, "name");
				g->items = realloc(g->items, sizeof(ITEM) * (i + 1));
				strcpy(g->items[i].name, attr);
				g->items[i].type = tag_type;
				g->items[i].node = node;
				if (tag_type == TAG_INTEGER || tag_type == TAG_STRING || tag_type == TAG_COMBOBOX || tag_type == TAG_COLOR ||
					tag_type == TAG_POSITION || tag_type == TAG_RECT || tag_type == TAG_KEY)
				{
					utf8_2ws(ws, attr, 30);
					wsAppendChar(ws, ':');
					wsAppendChar(ws, ' ');
					ConstructEditControl(&ec, ECT_HEADER, ECF_APPEND_EOL, ws, ws->wsbody[0]);
					SetFontToEditCOptions(&ec_options, 1);
					CopyOptionsToEditControl(&ec, &ec_options);
					AddEditControlToEditQend(eq, &ec, ma);
				}
				else if (tag_type == TAG_SUBMENU || tag_type == TAG_CHECKBOX)
				{
					utf8_2ws(ws, attr, 30);
					wsAppendChar(ws, ':');
					wsAppendChar(ws, ' ');
					ConstructEditControl(&ec, ECT_HEADER, ECF_DISABLE_EOLCHAR, ws, ws->wsbody[0]);
					SetFontToEditCOptions(&ec_options, 1);
					CopyOptionsToEditControl(&ec, &ec_options);
					AddEditControlToEditQend(eq, &ec, ma);
				}
			}
		}
		else if (node_type == MXML_OPAQUE)
		{
			if (save)
			{
				const char *var = mxmlGetOpaque(node);
				strcpy(g->items[i].var, var);
				if (tag_type == TAG_INTEGER)
				{
					str_2ws(ws, var, 255);
					ConstructEditControl(&ec, ECT_NORMAL_NUM, ECF_APPEND_EOL, ws, 256);
					SetFontToEditCOptions(&ec_options, 1);
					CopyOptionsToEditControl(&ec, &ec_options);
					AddEditControlToEditQend(eq, &ec, ma);
				}
				else if (tag_type == TAG_STRING)
				{
					utf8_2ws(ws, var, 127);
					ConstructEditControl(&ec, ECT_NORMAL_TEXT, ECF_APPEND_EOL, ws, 128);
					SetFontToEditCOptions(&ec_options, 1);
					CopyOptionsToEditControl(&ec, &ec_options);
					AddEditControlToEditQend(eq, &ec, ma);
				}
				else if (tag_type == TAG_CHECKBOX)
				{
					if (strcmp(var, "true") == 0)
						str_2ws(ws, "[x]", 3);
					else
						str_2ws(ws, "[  ]", 4);
					ConstructEditControl(&ec, ECT_READ_ONLY_SELECTED, ECF_APPEND_EOL, ws, 256);
					SetFontToEditCOptions(&ec_options, 1);
					CopyOptionsToEditControl(&ec, &ec_options);
					AddEditControlToEditQend(eq, &ec, ma);
				}
				else if (tag_type == TAG_COLOR)
				{
					str_2ws(ws, var, 255);
					ConstructEditControl(&ec, ECT_READ_ONLY_SELECTED, ECF_APPEND_EOL, ws, 256);
					SetFontToEditCOptions(&ec_options, 1);
					CopyOptionsToEditControl(&ec, &ec_options);
					
					char c[4];
					get_color_from_str(c, var);
					ws_set_color(ec.pWS, c);
					
					AddEditControlToEditQend(eq, &ec, ma);
				}
				else if (tag_type == TAG_COMBOBOX)
				{
					char name[16];
					char *attr;
					
					unsigned int n_item = atoi(var);
					sprintf(name, "item%d", n_item);
					attr = (char*)mxmlElementGetAttr(g->items[i].node, name);
					utf8_2ws(ws, var, 32);
					
					attr = (char*)mxmlElementGetAttr(g->items[i].node, "n");
					
					ConstructComboBox(&ec, ECT_COMBO_BOX, ECF_APPEND_EOL, ws, 32, 0, atoi(attr), n_item);
					SetFontToEditCOptions(&ec_options, 1);
					CopyOptionsToEditControl(&ec, &ec_options);
					AddEditControlToEditQend(eq, &ec, ma);
				}
				else if (tag_type == TAG_POSITION || tag_type == TAG_RECT || tag_type == TAG_KEY)
				{
					str_2ws(ws, var, 255);
					ConstructEditControl(&ec, ECT_READ_ONLY_SELECTED, ECF_APPEND_EOL, ws, 256);
					SetFontToEditCOptions(&ec_options, 1);
					CopyOptionsToEditControl(&ec, &ec_options);
					AddEditControlToEditQend(eq, &ec, ma);
				}
				else if (tag_type == TAG_SUBMENU)
				{
					str_2ws(ws, "Enter", 5);
					ConstructEditControl(&ec, ECT_READ_ONLY_SELECTED, ECF_APPEND_EOL + ECF_GRAY, ws, 5);
					SetFontToEditCOptions(&ec_options, 1);
					CopyOptionsToEditControl(&ec, &ec_options);
					AddEditControlToEditQend(eq, &ec, ma);
				}
				save = false;
				i++;
			}
		}
		node = mxmlWalkNext(node, top, MXML_DESCEND);
	}
	g->n_items = i;
	FreeWS(ws);
	//Create EDITCONTROL
	patch_header(&(g->hdr_desc));
	patch_input(&(g->input_desc));
	
	return CreateInputTextDialog(&(g->input_desc), &(g->hdr_desc), eq, 1, gui);
}

void maincsm_oncreate(CSM_RAM *data)
{
	MAIN_CSM *csm = (MAIN_CSM*)data;
	csm->csm.state = 0;
	csm->csm.unk1 = 0;
	csm->gui = malloc(sizeof(INPUT_GUI));
	csm->gui->prev = NULL;
	csm->gui->items = NULL;
	csm->gui_id = CreateDialog("menu", &(csm->gui));
	if (!csm->gui_id)
	{
		mfree(csm->gui);
		CloseCSM(csm->csm.id);
	}
}

void maincsm_onclose(CSM_RAM *data)
{
	MAIN_CSM *csm = (MAIN_CSM*)data;
	if (xml_tree)
		mxmlDelete(xml_tree);
	SUBPROC((void *)kill_elf);
}


int maincsm_onmessage(CSM_RAM *data, GBS_MSG *msg)
{
	MAIN_CSM *csm = (MAIN_CSM*)data;
	if (msg->msg == MSG_GUI_DESTROYED)
	{
		if ((int)msg->data0 == csm->gui_id)
		{
			csm->csm.state = -3;
		}
	}
	return 1;
}

const int minus11 = -11;
unsigned short maincsm_name_body[140];

const struct
{
	CSM_DESC maincsm;
	WSHDR maincsm_name;
}MAINCSM =
{
	{
		maincsm_onmessage,
		maincsm_oncreate,
		#ifdef NEWSGOLD
			0,
			0,
			0,
			0,
		#endif
		maincsm_onclose,
		sizeof(MAIN_CSM),
		1,
		&minus11
	},
	{
		maincsm_name_body,
		NAMECSM_MAGIC1,
		NAMECSM_MAGIC2,
		0x0,
		139,
		0
	}
};

void UpdateCSMname(void)
{
	wsprintf((WSHDR *)(&MAINCSM.maincsm_name), "CfgEdit2");
}

int main(const char *b_path, const char *f_path)
{
	if (f_path == NULL)
	{
		SUBPROC((void*)kill_elf);
		return EXIT_FAILURE;
	}
	
	//load xml file
	FILE *fp;
	char str[128];
	fp = fopen(f_path, "r");
	if (fp == NULL)
	{
		sprintf(str, "Can't open file: %s", f_path);
		MsgBoxError(1, (int)str);
		SUBPROC((void*)kill_elf);
		return EXIT_FAILURE;
	}
	xml_tree = mxmlLoadFile(NULL, fp, MXML_OPAQUE_CALLBACK);
	fclose(fp);
	if (xml_tree == NULL)
	{
		MsgBoxError(1, (int)"Can't load xml file");
		SUBPROC((void*)kill_elf);
		return EXIT_FAILURE;
	}
	
	//check config version
	char *ver = (char*)mxmlElementGetAttr(xml_tree, "version");
	if (ver)
	{
		if (strcmp(ver, CONFIG_VERSION)) goto DESTROY;
	}
	else
	{
		DESTROY:
			ShowMSG(1, (int)ver);
			mxmlDelete(xml_tree);
			SUBPROC((void*)kill_elf);
			return EXIT_FAILURE;
	}
	
	strcpy(FILE_PATH, f_path);
	
	//Create CSM
	char dummy[sizeof(MAIN_CSM)];
	wsprintf((WSHDR *)(&MAINCSM.maincsm_name), "XCE - %s", FILE_PATH);
	LockSched();
	CreateCSM(&MAINCSM.maincsm, dummy, 0);
	UnlockSched();
	
	return EXIT_SUCCESS;
}
