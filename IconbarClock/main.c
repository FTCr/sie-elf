#include <swilib.h>
#include <mxml.h>
#include <libxce.h>

typedef struct
{
	CSM_RAM csm;
}MAIN_CSM;

CSM_RAM *under_idle;

int bts;
IMGHDR *icons[11];
char *map;

#define CONF_PATH "2:\\etc\\IconbarClock.xcfg"

int rec_mes;
char font_path[128];
char icons_path[128];
int tmp_icon, empty_icon;

const char mypng[124]=
{
	0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A, 0x00, 0x00, 0x00, 0x0D, 0x49, 0x48, 0x44, 0x52, 
	0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01, 0x08, 0x03, 0x00, 0x00, 0x00, 0x28, 0xCB, 0x34, 
	0xBB, 0x00, 0x00, 0x00, 0x19, 0x74, 0x45, 0x58, 0x74, 0x53, 0x6F, 0x66, 0x74, 0x77, 0x61, 0x72, 
	0x65, 0x00, 0x41, 0x64, 0x6F, 0x62, 0x65, 0x20, 0x49, 0x6D, 0x61, 0x67, 0x65, 0x52, 0x65, 0x61, 
	0x64, 0x79, 0x71, 0xC9, 0x65, 0x3C, 0x00, 0x00, 0x00, 0x06, 0x50, 0x4C, 0x54, 0x45, 0xFF, 0xFF, 
	0xFF, 0x00, 0x00, 0x00, 0x55, 0xC2, 0xD3, 0x7E, 0x00, 0x00, 0x00, 0x0C, 0x49, 0x44, 0x41, 0x54, 
	0x78, 0xDA, 0x62, 0x60, 0x00, 0x08, 0x30, 0x00, 0x00, 0x02, 0x00, 0x01, 0x4F, 0x6D, 0x59, 0xE1, 
	0x00, 0x00, 0x00, 0x00, 0x49, 0x45, 0x4E, 0x44, 0xAE, 0x42, 0x60, 0x82
};

int InitConfig(void)
{
	mxml_node_t *tree = xce_load(CONF_PATH);
	
	if (tree == NULL) return 0;
	
	rec_mes = xce_get_combobox(tree, "Enable reconfigure message");
	xce_get_string(tree, "Png font path", font_path);
	xce_get_string(tree, "Icons path", icons_path);
	tmp_icon = xce_get_integer(tree, "Number temporary icon");
	empty_icon = xce_get_integer(tree, "Number empty icon");
	
	xce_delete(tree);
	
	return 1;
}

int maincsm_onmessage(CSM_RAM* data,GBS_MSG* msg)
{
	if(msg->msg == MSG_RECONFIGURE_REQ)
	{
		if(strcmp(CONF_PATH, (char *)msg->data0) == 0)
		{
			InitConfig();
			if(rec_mes) ShowMSG(1, (int)"IconbarClock config updated");
		}
	}
	return (1);
}

TTime ttime;

static void IconBar(short* num)
{
	int t[5] = {0, 0, 10, 0, 0}, c;
	TDate dt;
	TTime tm;
	GetDateTime(&dt, &tm);
	IMGHDR *ic = GetPITaddr(tmp_icon);
	if((tm.hour != ttime.hour) || (tm.min != ttime.min) || ((char*)ic->bitmap != map))
	{
		ttime = tm;
		t[0] = ttime.hour / 10;
		t[1] = ttime.hour;
		while(t[1] >= 10)
		{
			t[1] -= 10;
		}
		t[3] = ttime.min / 10;
		t[4] = ttime.min;
		while(t[4] >= 10)
		{
			t[4] -= 10;
		}
		ic->h = icons[0]->h;
		ic->w = icons[t[0]]->w + icons[t[1]]->w + icons[t[2]]->w + icons[t[3]]->w + icons[t[4]]->w;
		mfree(ic->bitmap);
		map = malloc(ic->h * ic->w * bts);
		zeromem(map, ic->h * ic->w * bts);
		for(int i = 0; i < ic->h; i++)
		{
			c = i * ic->w * bts;
			for(int j = 0; j < 5; j++)
			{
				memcpy(map+c, icons[t[j]]->bitmap+i * icons[t[j]]->w * bts, icons[t[j]]->w * bts);
				c += icons[t[j]]->w * bts;
			}
			ic->bitmap = map;
		}
	}
	AddIconToIconBar(tmp_icon, num);
	AddIconToIconBar(empty_icon, num);
}

static void maincsm_oncreate(CSM_RAM *data)
{

}

static void maincsm_onclose(CSM_RAM *csm)
{
  for(int i=0; i<11; i++)
  {
    if(icons[i])
    {
      mfree(icons[i]->bitmap);
      mfree(icons[i]);
    }
  }
  SUBPROC((void *)kill_elf);
}

typedef struct
{
	char check_name[8];
	int addr;
}ICONBAR_H;

const int minus11 =- 11;
static unsigned short maincsm_name_body[140];

struct
{
	CSM_DESC maincsm;
	WSHDR maincsm_name;
	ICONBAR_H iconbar_handler;
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
		sizeof(CSM_RAM),
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
	},
	{
		"IconBar",
		(int)IconBar
	}
};

static void UpdateCSMname(void)
{
	wsprintf((WSHDR *)(&MAINCSM.maincsm_name), "IconbarClock");
}

int main()
{
	if (!InitConfig())
		return EXIT_FAILURE;
	
	char fl[64];
	for(int i = 0; i < 11; i++)
	{
		zeromem(fl, 64);
		sprintf(fl, "%s%x.png", font_path, i);
		icons[i] = CreateIMGHDRFromPngFile(fl, 0);
		if(!icons[i])
		{
			ShowMSG(1, (int)fl);
			MsgBoxError(1, (int)"Png font not found!");
			for(int j = 0; j < i; j++)
			{
				if(icons[j])
				{
					mfree(icons[j]->bitmap);
					mfree(icons[j]);
				}
			}
			return 0;
		}
	}
	switch(icons[0]->bpnum)
	{
		case 0x5: case 0x85:
			bts = 1;
		break;
		case 0x8: case 0x88:
			bts = 2;
		break;
		case 0xA: case 0x8A:
			bts = 4;
		break;
		default:
			MsgBoxError(1, (int)"Unsupported png files");
			for(int i=0; i<11; i++)
			{
				if(icons[i])
				{
					mfree(icons[i]->bitmap);
					mfree(icons[i]);
				}
			}
		return 0;
	}
	zeromem(fl, 64);
	sprintf(fl, "%s%d.png", icons_path, tmp_icon);
	int f;
	unsigned int ul;
	if ((f = _open(fl, A_ReadOnly + A_BIN, P_READ, &ul)) != -1) _close(f, &ul);
	else
	{
		if ((f = _open(fl, A_ReadWrite + A_Create + A_Truncate, P_READ + P_WRITE,&ul)) != -1)
		{
			_write(f, mypng, 124, &ul);
			_close(f, &ul);
			_close(f, &ul);
		}
		else
		{
			MsgBoxError(1, (int)"Can't create temporary icon!");
			for(int j = 0; j < 11; j++)
			{
				if(icons[j])
				{
					mfree(icons[j]->bitmap);
					mfree(icons[j]);
				}
			}
		return 0;
		}
	}
	
	
	CSM_RAM *save_cmpc;
	CSM_RAM main_csm;
	UpdateCSMname();
	//SetIconBarHandler();
	LockSched();
	save_cmpc = CSM_root()->csm_q->current_msg_processing_csm;
	CSM_root()->csm_q->current_msg_processing_csm = CSM_root()->csm_q->csm.first;
	CreateCSM(&MAINCSM.maincsm, &main_csm, 0);
	CSM_root()->csm_q->current_msg_processing_csm = save_cmpc;
	UnlockSched();
	return 0;
}
