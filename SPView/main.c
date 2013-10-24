#include <swilib.h>
#include <stdlib.h>
#include "gui.h"

typedef struct
{
	CSM_RAM csm;
	int gui_id;
}MAIN_CSM;

void maincsm_oncreate(CSM_RAM *csm_ram)
{
	MAIN_CSM *csm = (MAIN_CSM*)csm_ram;
	csm->csm.state = 0;
	csm->csm.unk1 = 0;
	csm->gui_id = CreateMyGUI();
}

void maincsm_onclose(CSM_RAM *csm_ram)
{
	SUBPROC((void *)kill_elf);
}

int maincsm_onmessage(CSM_RAM *csm_ram, GBS_MSG *msg)
{
	MAIN_CSM *csm = (MAIN_CSM*)csm_ram;
	if (msg->msg == MSG_GUI_DESTROYED)
	{
		if ((int)msg->data0 == csm->gui_id)
		{
			csm->csm.state =- 3;
		}
	}
	return 1;
}

const int minus11 =- 11;
unsigned short maincsm_name_body[140];

const struct
{
	CSM_DESC maincsm;
	WSHDR maincsm_name;
} MAINCSM =
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
	wsprintf((WSHDR *)(&MAINCSM.maincsm_name), "SPView");
}

int main(void)
{
	char dummy[sizeof(MAIN_CSM)];
	UpdateCSMname();
	LockSched();
	CreateCSM(&MAINCSM.maincsm, dummy, 0);
	UnlockSched();
	return EXIT_SUCCESS;
}
