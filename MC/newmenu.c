#include <swilib.h>
#include "rect_patcher.h"
#include "mc.h"
#include "mui.h"
#include "krnl.h"
#include "gui.h"
#include "newmenu.h"
#include "color.h"
#include "colormap.h"
#include "zslib.h"
#include "screenmake.h"

typedef struct
{
  int name;
  KEY_PROC act;
}MENU_ITEMS;

int cur=0, sab=0, cur_sab=0, ssab=0, cur_ssab=0, showmenu=0;
int cur_save, cur_sab_save, sab_save;
int menu_txt, mnf, mnc;
IMGHDR *mn_fon, *mn_cur;

int filetype=0;
int Isdirect=0;
#define MMenuH   8
#define MMenuH1  10
#define MMenuH2  9
#ifdef NEWSGOLD
  #define CreateMH 3
#else
  #define CreateMH 2
#endif
#define PictMH   3
#define SoundMH  4
#define FileMH   4
#define OperMH   8
#define ViewMH   3
#define SortMH   5
#define MiskMH   4
#define BM_MH    2

#ifdef NEWSGOLD
void GetType()
{
  if(filetype && cur>2 && cursave)
    cur--;

  filetype=0;
  
  if(!IsInArchive())
  {
    if(scfile==NULL) return;
  
    if(IsMedia(scfile->sname))
    {
      if(cur>2 && cursave)cur++;
      filetype=2;
    }

    if(IsPict(scfile->sname))
    {
      if(cur>2 && cursave)cur++;
      filetype=1;
    }
  }
}
#endif

void freemenu()
{
  if (mnf==1)
  {
    mfree(mn_fon->bitmap);
    mfree(mn_fon);
  }
  if (mnc==1)
  {
    mfree(mn_cur->bitmap);
    mfree(mn_cur);
  }
}

void DoOpenSab1()
{
  sab=1;
  if (sab_save==sab && cursave)
    cur_sab=cur_sab_save;
  else
    cur_sab=0;
}

void DoOpenSab2()
{
  sab=2;
  if (sab_save==sab && cursave)
    cur_sab=cur_sab_save;
  else
    cur_sab=0;
}

void DoOpenSab3()
{
  sab=3;
  if (sab_save==sab && cursave)
    cur_sab=cur_sab_save;
  else
    cur_sab=0;
}

void DoOpenSab4()
{
  sab=4;
  if (sab_save==sab && cursave)
    cur_sab=cur_sab_save;
  else
    cur_sab=0;
}

void DoOpenSab5()
{
  sab=5;
  if (sab_save==sab && cursave)
    cur_sab=cur_sab_save;
  else
    cur_sab=0;
}
#ifdef NEWSGOLD
void DoOpenSab6()
{
  if(filetype==1)
  {
    sab=6;
    if (sab_save==sab && cursave)
      cur_sab=cur_sab_save;
    else
      cur_sab=0;
  }
  
  if(filetype==2)
  {
    sab=7;
    if (sab_save==sab && cursave)
      cur_sab=cur_sab_save;
    else
      cur_sab=0;
  }
}
#endif
void DoOpenSSab()
{
  ssab=1;
  cur_ssab=0;
}

void Menu_DoAltOpen()
{
  //menuclose();
  DoAltOpen();
}

void Menu_DoSent()
{
  //menuclose();
  DoSent();
}

void Menu_DoSeach()
{
  DoSearch();
}

void Menu_DoFilter()
{
  //menuclose();
  DoFilter();
}

void Menu_DoRefresh()
{
  //menuclose();
  DoRefresh();
}

void Menu_DoFilePropNew()
{
  //menuclose();
  DoFilePropNew();
}

void Menu_DoDrvInf()
{
  //menuclose();
  DoDrvInf();
}

void Menu_DoOptions()
{
  //menuclose();
  DoOptions();
}

void Menu_DoAboutDlg()
{
  //menuclose();
  DoAboutDlg();
}

void Menu_DoExit()
{
  //menuclose();
  CB_Exit(IDYES);
}

#ifdef NEWSGOLD
void Menu_SetWall()
{
  CurFullPathByName(scfile->sname);
  SetWallpaper(pathbuf);
  //menuclose();
}

void Menu_SetLogo()
{
  CurFullPathByName(scfile->sname);
  SetLogo(pathbuf);
  //menuclose();
}

void Menu_SetScreenSaver()
{
  CurFullPathByName(scfile->sname);
  SetScreenSaver(pathbuf);
  //menuclose();
}

void Menu_SetCall()
{
  CurFullPathByName(scfile->sname);
  SetCall(pathbuf);
  //menuclose();
}

void Menu_SetMessages()
{
  CurFullPathByName(scfile->sname);
  SetMessages(pathbuf);
  //menuclose();
}

void Menu_SetAlarmClock()
{
  CurFullPathByName(scfile->sname);
  SetAlarmClock(pathbuf);
  //menuclose();
}

void Menu_SetAppointments()
{
  CurFullPathByName(scfile->sname);
  SetAppointments(pathbuf);
  //menuclose();
}

MENU_ITEMS MMenu1[]=
{
  {ind_altopen, Menu_DoAltOpen},
  {ind_bt, Menu_DoSent},
  {ind_set_as, DoOpenSab6},
  {ind_create, DoOpenSab1},
  {ind_file_m, DoOpenSab2},
  {ind_oper_m, DoOpenSab3},
  {ind_prop, Menu_DoFilePropNew},
  {ind_view_m, DoOpenSab4},
  {ind_misc_m, DoOpenSab5},
  {ind_exit, Menu_DoExit}
};

MENU_ITEMS MSetPict[]=
{
  {ind_wall, Menu_SetWall},
  {ind_logo, Menu_SetLogo},
  {ind_ss, Menu_SetScreenSaver}
};

MENU_ITEMS MSetSound[]=
{
  {ind_call, Menu_SetCall},
  {ind_mess, Menu_SetMessages},
  {ind_alarm, Menu_SetAlarmClock},
  {ind_org, Menu_SetAppointments}
};

#endif


MENU_ITEMS MMenu[]=
{
  {ind_altopen, Menu_DoAltOpen},
  {ind_create, DoOpenSab1},
  {ind_file_m, DoOpenSab2},
  {ind_oper_m, DoOpenSab3},
  {ind_prop, Menu_DoFilePropNew},
  {ind_view_m, DoOpenSab4},
  {ind_misc_m, DoOpenSab5},
  {ind_exit, Menu_DoExit}
};

MENU_ITEMS MMenu2[]=
{
  {ind_altopen, Menu_DoAltOpen},
  {ind_bt, Menu_DoSent},
  {ind_create, DoOpenSab1},
  {ind_file_m, DoOpenSab2},
  {ind_oper_m, DoOpenSab3},
  {ind_prop, Menu_DoFilePropNew},
  {ind_view_m, DoOpenSab4},
  {ind_misc_m, DoOpenSab5},
  {ind_exit, Menu_DoExit}
};
#ifdef NEWSGOLD
MENU_ITEMS CreateM[]=
{
  {ind_newfile, DoNewFile},
  {ind_newdir, DoNewDir},
  {ind_arch, DoArchive}
};
#else
MENU_ITEMS CreateM[]=
{
  {ind_newfile, DoNewFile},
  {ind_newdir, DoNewDir}
};
#endif
MENU_ITEMS FileM[]=
{
  {ind_chk, DoChk},
  {ind_chkall, DoChkAll},
  {ind_unchall, DoUnCAll},
  {ind_invchk, DoInvChk}
};

MENU_ITEMS OperM[]=
{
  {ind_past, DoPaste},
  {ind_cancel, DoCancel},
  {ind_copy, DoCopy},
  {ind_move, DoMove},
  {ind_del, DoDel},
  {ind_rename, DoRen},
  {ind_natexpl, DoNatExpl},
  {ind_find, Menu_DoSeach}
};

MENU_ITEMS ViewM[]=
{
  {ind_sort_m, DoOpenSSab},
  {ind_filter, Menu_DoFilter},
  {ind_refresh, Menu_DoRefresh}
};

MENU_ITEMS SortM[]=
{
  {ind_sortn, DoSortN},
  {ind_sorte, DoSortE},
  {ind_sorts, DoSortS},
  {ind_sortd, DoSortD},
  {ind_sortr, DoSortR}
};

MENU_ITEMS MiskM[]=
{
  {ind_bm_m, DoOpenSSab},
  {ind_drvinf, Menu_DoDrvInf},
  {ind_settings, Menu_DoOptions},
  {ind_about_m, Menu_DoAboutDlg}
};

MENU_ITEMS BM_M[]=
{
  {ind_bml, DoBMList},
  {ind_add, DoBMAdd}
};

int y_pos[5];

/*void DoMenu()
{
 allclose(); 
 ClearScreen();
 DoScreen();
 if(cursave)
 {
   cur_sab_save=cur_sab;
   sab_save=sab;
 }else
    cur=0; 
 
 showmenu=1;
 sab=0;
 cur_sab=0;
 ssab=0;
 cur_ssab=0;
 Busy1=1; 
 
 if (cursave)
 {
  if(Isdirect && cur>1)
    cur++;

  if(scfile->attr & FA_DIRECTORY || IsInArchive())
  {
    Isdirect=1;
    if(cur>0)cur--;
  }
  else
    Isdirect=0;
 }
 
#ifdef NEWSGOLD
 GetType();
#endif
 
 y_pos[0]=scr_h-MMenuH1*(menu_txt+1);
 y_pos[1]=y_pos[0]+(menu_txt+1)*3-(menu_txt+1)/2;
 y_pos[2]=y_pos[0]+(menu_txt+1)*7-(menu_txt+1)/2;
 y_pos[3]=y_pos[2]-(menu_txt+1)/2;
 y_pos[4]=y_pos[2]-(menu_txt+1)-(menu_txt+1)/2;   
}*/

void menuclose()
{
  ClearScreen();
  Busy1=0;
  showmenu=0;
  PeReDraw();
}

void DoMenuENTER()
{
  switch (sab)
  {
  case 0:
    if (!Isdirect)
    {
#ifdef NEWSGOLD
    if(filetype)
    {
      if (MMenu1[cur].act) MMenu1[cur].act();
    }else
      if (MMenu2[cur].act) MMenu2[cur].act();
#else
    if (MMenu2[cur].act) MMenu2[cur].act();
#endif
    }else
      if (MMenu[cur].act) MMenu[cur].act();
    break;
    
  case 1:
    menuclose();
    CreateM[cur_sab].act();
    break; 
    
  case 2:
    menuclose();
    FileM[cur_sab].act();
    break;
    
  case 3:
    if(cur_sab!=OperMH-1)menuclose();
    OperM[cur_sab].act();
    break;
    
  case 4:
    if (ssab==1)
    {
      menuclose();
      SortM[cur_ssab].act();
    }else
    {
      ViewM[cur_sab].act();
    }
    break;
    
  case 5:
    if (ssab==1)
    {
      menuclose();
      BM_M[cur_ssab].act();
    }else
      MiskM[cur_sab].act();
    break;
#ifdef NEWSGOLD    
  case 6:
    if (MSetPict[cur_sab].act) MSetPict[cur_sab].act();
    break;
    
  case 7:
    if (MSetSound[cur_sab].act) MSetSound[cur_sab].act();
    break;
#endif
  }
}

void DoNumKey(int key)
{
  int open=0;
  switch(sab)
  {
  case 0:
    if (key>='1' && ((key<='9' && !Isdirect) || (key<'9' && Isdirect)))
    {
      cur=key-1-'0';
      open=1;
    }else
      if (key=='0' && filetype)
      {
        cur=9;
        open=1;
      }else
        open=0;  
    break;
    
  case 1:
    if (key>='1' && key<='3')
    {
      cur_sab=key-1-'0';
      open=1;
    }else
      open=0;  
    break;

  case 2:
    if (key>='1' && key<='4')
    {
      cur_sab=key-1-'0';
      open=1;
    }else
      open=0;  
    break;

  case 3:
    if (key>='1' && key<='7')
    {
      cur_sab=key-1-'0';
      open=1;
    }else
      open=0;  
    break;
 
  case 4:
    if (ssab)
    {
      if (key>='1' && key<='5')
      {
        cur_ssab=key-1-'0';
        open=1;
      }else
        open=0; 
    }else
    {    
      if (key>='1' && key<='3')
      {
        cur_sab=key-1-'0';
        open=1;
      }else
        open=0; 
    }
    break;
    
  case 5:
    if (ssab)
    {
      if (key>='1' && key<='2')
      {
        cur_ssab=key-1-'0';
        open=1;
      }else
        open=0; 
    }else
    {    
      if (key>='1' && key<='4')
      {
        cur_sab=key-1-'0';
        open=1;
      }else
        open=0; 
    }
    break;
    
  case 6:
    if (key>='1' && key<='3')
    {
      cur_sab=key-1-'0';
      open=1;
    }else
      open=0;  
    break;

  case 7:
    if (key>='1' && key<='4')
    {
      cur_sab=key-1-'0';
      open=1;
    }else
      open=0;  
    break;

  }
  
  if (open)
    DoMenuENTER();
}

void DoMenuUP()
{
  switch(sab)
  {
    case 0:
      if (!Isdirect)
      {
        if(filetype)
          cur=(cur?cur--:MMenuH1-1);
        else
          cur=(cur?cur--:MMenuH2-1);
      }else
        cur=(cur?cur--:MMenuH-1);
    break;
    case 1:
      cur_sab=(cur_sab?cur_sab--:CreateMH-1);
    break;
    case 2:
      cur_sab=(cur_sab?cur_sab--:FileMH-1);
    break;
    case 3:
      cur_sab=(cur_sab?cur_sab--:OperMH-1);
    break;
    case 4:
      if (ssab)
        cur_ssab=(cur_ssab?cur_ssab--:SortMH-1);
      else
        cur_sab=(cur_sab?cur_sab--:ViewMH-1);
    break;
    case 5:
      if (ssab)
        cur_ssab=(cur_ssab?cur_ssab--:BM_MH-1);
      else
        cur_sab=(cur_sab?cur_sab--:MiskMH-1);
    break;
    
    case 6:
      cur_sab=(cur_sab?cur_sab--:PictMH-1);
    break;
    case 7:
      cur_sab=(cur_sab?cur_sab--:SoundMH-1);
    break;
  }
}
 
void DoMenuDOWN()
{
  switch(sab)
  {
    case 0:
      if (!Isdirect)
      {
        if(filetype)
          cur=(cur!=MMenuH1-1?cur++:0); 
        else
          cur=(cur!=MMenuH2-1?cur++:0); 
      }else
        cur=(cur!=MMenuH-1?cur++:0); 
      break;
      
    case 1:
      cur_sab=(cur_sab!=CreateMH-1?cur_sab++:0); 
      break;
      
    case 2:
      cur_sab=(cur_sab!=FileMH-1?cur_sab++:0); 
      break;
      
    case 3:
      cur_sab=(cur_sab!=OperMH-1?cur_sab++:0); 
    break;
    
    case 4:
      if (ssab)
        cur_ssab=(cur_ssab!=SortMH-1?cur_ssab++:0); 
      else
        cur_sab=(cur_sab!=ViewMH-1?cur_sab++:0); 
      break;
      
    case 5:
      if (ssab)
        cur_ssab=(cur_ssab!=BM_MH-1?cur_ssab++:0);
      else
        cur_sab=(cur_sab!=MiskMH-1?cur_sab++:0); 
      break;
      
    case 6:
      cur_sab=(cur_sab!=PictMH-1?cur_sab++:0); 
    break;
    
    case 7:
      cur_sab=(cur_sab!=SoundMH-1?cur_sab++:0); 
    break;
  }
}

void menu_key(int key)
{
  switch (key)
  {
    case RIGHT_BUTTON:
    case ENTER_BUTTON: 
      DoMenuENTER();
      PeReDraw();
      return;
      
    case UP_BUTTON: 
      DoMenuUP();
      break;
      
    case DOWN_BUTTON:
      DoMenuDOWN();
      break;
      
    case LEFT_BUTTON:
    case LEFT_SOFT:
      if (sab==0)
      {
        menuclose();
        return;
      }
      else 
      {
        if (ssab==0)
        {
          sab=0;
          cur_sab=0;
         }else
         {
          ssab=0;
          cur_ssab=0;
         }
      }
     break;
   case RIGHT_SOFT:
     menuclose(); 
     return;

   }
  if((key>='1' && key<='9') || (key=='0') || (key=='*') || (key=='#'))
    DoNumKey(key);
}

int MenuH[12];

int MenuWid(int count, MENU_ITEMS *ItemsName)
{
  WSHDR *ws = AllocWS(64);
  int wi=0;
  for(int i=0; i<count; i++)  
  {
    wsprintf(ws, _t, muitxt(ItemsName[i].name));
    if (Get_WS_width(ws,FONT_MENUV)>wi)
      wi=Get_WS_width(ws,FONT_MENUV);
  }
  FreeWS(ws);
  
  return wi;
}

void crtmenu()
{
  FSTATS fs;
  char z[128];
  
  sprintf(z, "%smenu_bg.png",graf_folder);
  if (GetFileStats(z,&fs,&err)==0)
  {
    mn_fon=CreateIMGHDRFromPngFile(z, 0); 
    mnf=1;
  }
  
  sprintf(z, "%smenu_cursor.png",graf_folder);
  if (GetFileStats(z,&fs,&err)==0)
  {
    mn_cur=CreateIMGHDRFromPngFile(z, 0); 
    mnc=1;
  }
  menu_txt =  GetFontYSIZE(FONT_MENUNV);
  
////////////////////////////////////////////////////////////////////////////////
  if (mnc==0 || mnf==0)
  {
#ifdef NEWSGOLD
    MenuH[0]=MenuWid(MMenuH1, MMenu1)+8;
#endif
    MenuH[10]=MenuWid(MMenuH2, MMenu2)+8;
    
    MenuH[11]=MenuWid(MMenuH, MMenu)+8;
    
    MenuH[1]=MenuWid(CreateMH, CreateM)+8;
    MenuH[2]=MenuWid(FileMH, FileM)+8;
    MenuH[3]=MenuWid(OperMH, OperM)+8;
    MenuH[4]=MenuWid(ViewMH, ViewM)+8;
    MenuH[5]=MenuWid(SortMH, SortM)+menu_txt+8;
    MenuH[6]=MenuWid(MiskMH, MiskM)+8;
    MenuH[7]=MenuWid(BM_MH, BM_M)+8;
#ifdef NEWSGOLD
    MenuH[8]=MenuWid(PictMH, MSetPict)+10;
    MenuH[9]=MenuWid(SoundMH, MSetSound)+8;
#endif
  }
////////////////////////////////////////////////////////////////////////////////  
}

#ifdef ELKA
  #define sdvig 35
#else
  #define sdvig 23
#endif

static void DrawCheckBox(int x,int y,int flag)
{
  RECT rc;
  StoreXYXYtoRECT(&rc,x,y,x+menu_txt-3,y+menu_txt-3);
  
  if (flag!=4)
    DrawRoundedFrame(rc.x,rc.y,rc.x2,rc.y2,6,6,0,(char*)&Colors[clCheckBD],NULL);
  else
    DrawRectangle   (rc.x,rc.y,rc.x2,rc.y2,0,(char*)&Colors[clCheckBD],NULL);
  
  if ((flag==4 && _CurTab->sort>=ST_REVERS) || (_CurTab->sort>=ST_REVERS && _CurTab->sort==flag+ST_REVERS) || (_CurTab->sort==flag))
    DrawRoundedFrame(rc.x+2,rc.y+2,rc.x2-2,rc.y2-2,6,6,0,(char*)&Colors[clCheckEnter],(char*)&Colors[clCheckEnter]);
  if (flag==4 && _CurTab->sort>=ST_REVERS)
    DrawRectangle(rc.x+2,rc.y+2,rc.x2-2,rc.y2-2,0,(char*)&Colors[clCheckEnter],(char*)&Colors[clCheckEnter]);
}      

void DrawMenu(int count, MENU_ITEMS *ItemsName, int ots, int idx, int cur_pos, int drwcb, int wi)
{  
  int i=0, ot=drwcb?ots+menu_txt:ots; 
  int font;
  int tc;
  if (mnf==1)
    DrwImage(mn_fon, ots, idx-2, 0, 0, mn_fon->w, count*(menu_txt+1)+4, 0);
  else
  {
    DrwImage(background, ots, idx-1, 
             ots, idx-1-YDISP1, wi, count*(menu_txt+1)+2, 0);    
    DrawRoundedFrame(ots, idx-1, ots+wi, idx-2+count*(menu_txt+1)+3,
                     0,0,0,(char*)&Colors[clMenuBD],(char*)&Colors[clMenuBG]);
  }
  
  if (mnc==1)
    DrwImage(mn_cur, ots+2, idx+cur_pos*(menu_txt+1), 0, 0, mn_cur->w, mn_cur->h, 0);
  else
    DrawRoundedFrame(ots+1, idx+cur_pos*(menu_txt+1), ots+wi-1, idx+cur_pos*(menu_txt+1)+(menu_txt+1),
                     0,0,0,(char*)&Colors[clMenuCurBD],(char*)&Colors[clMenuCurBG]);  
  
  
  do{
    font=(cur_pos==i? FONT_MENUV: FONT_MENUNV);
    tc=(cur_pos==i? clMenuTXTSelect: clMenuTXTNormal);
    
    wsprintf(guibuf, _t, muitxt(ItemsName[i].name));
    DrawString(guibuf, ot+4, idx, scr_w, idx+menu_txt, font, TEXT_ALIGNLEFT,
               (char*)&Colors[tc],0);
    
    if (drwcb)  DrawCheckBox(ots+4,idx+2,i);  

    i++;
    idx+=(menu_txt+1);
  }while(i<count);
  
}

void MenuVide()
{
  DrawScreen();
  
  if (!Isdirect)
  {
#ifdef NEWSGOLD
    if(filetype)
      DrawMenu(MMenuH1, MMenu1, 1, y_pos[0], cur, 0, MenuH[0]);
    else
#endif
      DrawMenu(MMenuH2, MMenu2, 1, y_pos[0]+(menu_txt+1), cur, 0, MenuH[10]);
  }else
    DrawMenu(MMenuH, MMenu, 1, y_pos[0]+(menu_txt+1)*2, cur, 0, MenuH[11]);
  
  switch (sab)
  {
  case 1:
    DrawMenu(CreateMH, CreateM, sdvig, y_pos[1], cur_sab, 0,MenuH[1]);
    break;
    
  case 2:
    DrawMenu(FileMH, FileM, sdvig, y_pos[1], cur_sab, 0,MenuH[2]);
    break;
    
  case 3:
    DrawMenu(OperMH, OperM, sdvig, y_pos[1], cur_sab, 0,MenuH[3]);
    break;
    
  case 4:
    DrawMenu(ViewMH, ViewM, sdvig, y_pos[2], cur_sab, 0,MenuH[4]);
    if (ssab)
      DrawMenu(SortMH, SortM, sdvig*2-5, y_pos[4], cur_ssab, 1,MenuH[5]);
    break; 
    
  case 5:
    DrawMenu(MiskMH, MiskM, sdvig, y_pos[2], cur_sab, 0,MenuH[6]);  
    if (ssab)
      DrawMenu(BM_MH, BM_M, sdvig*2, y_pos[3], cur_ssab, 0,MenuH[7]);
    break;
#ifdef NEWSGOLD    
  case 6:
    DrawMenu(PictMH, MSetPict, sdvig, y_pos[1]-menu_txt, cur_sab, 0,MenuH[8]);
    break;
    
  case 7:
    DrawMenu(SoundMH, MSetSound, sdvig, y_pos[1]-menu_txt, cur_sab, 0,MenuH[9]);
    break;
#endif
  }
}


//---------------------------------------------------------------------MAIN_MENU-----------------------------------------------------------------------------//

unsigned int mm_gui_id;

#define MAX_MM_ITEMS 10

static HEADER_DESC mm_header = {0, 0, 0, 0, NULL, (int)"Menu MC", LGP_NULL};

static int softkeys[] = {0, 1, 2};

static SOFTKEY_DESC sk[] =
{
	{0x0018, 0x0000, (int)"Select"},
	{0x0001, 0x0000, (int)"Back"},
	{0x003D, 0x0000, (int)LGP_DOIT_PIC}
};

static SOFTKEYSTAB skt =
{
	sk, 0
};

void MM_AltOpen(GUI *gui)
{
	DoAltOpen();
	GeneralFuncF1(2);
}

void MM_Send(GUI *gui)
{
	DoSent();
	GeneralFuncF1(2);
}

void MM_SetAs(GUI *gui)
{
	CreateSetAsMenu();
}

void MM_CreateMenu(GUI *gui)
{
	CreateCreateMenu();
}

void MM_FileMenu(GUI *gui)
{
	CreateFileMenu();
}

void MM_OperationMenu(GUI *gui)
{
	CreateOperationMenu();
}

void MM_Properties(GUI *gui)
{
	ShowMSG(1, (int)"Properties");
	GeneralFuncF1(2);
}

void MM_ViewMenu(GUI *gui)
{
	CreateViewMenu();
}

void MM_MiscMenu(GUI *gui)
{
	CreateMiscMenu();
}

void MM_Exit(GUI *gui)
{
	GeneralFuncF1(2);
	DoExit();
}

MENUITEM_DESC mm_items[MAX_MM_ITEMS]=
{
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
};

MENUPROCS_DESC mm_procs[MAX_MM_ITEMS];

static MENU_DESC mm_desc =
{
	8, NULL, NULL, NULL,
	softkeys,
	&skt,
	0x10,
	NULL,
	mm_items,
	mm_procs,
	0
};

void CreateMainMenu(void)
{
	unsigned int n_items = 0;
	
	if (IsInArchive())
	{
		n_items = 5;
		mm_procs[0] = MM_FileMenu;
		mm_items[0].lgp_id_small = (int)muitxt(ind_file_m);
		mm_procs[1] = MM_OperationMenu;
		mm_items[1].lgp_id_small = (int)muitxt(ind_oper_m);
		mm_procs[2] = MM_Properties;
		mm_items[2].lgp_id_small = (int)muitxt(ind_prop);
		mm_procs[3] = MM_MiscMenu;
		mm_items[3].lgp_id_small = (int)muitxt(ind_misc_m);
		mm_procs[4] = MM_Exit;
		mm_items[4].lgp_id_small = (int)muitxt(ind_exit);
	}
	else
	{
		int t;
		if ((t = IsCurFile()))
		{
			const int lng_ind[MAX_MM_ITEMS] = {ind_altopen, ind_bt, ind_set_as, ind_create, ind_file_m, ind_oper_m, ind_prop, ind_view_m, ind_misc_m, ind_exit};
			void *prcs[MAX_MM_ITEMS] = {MM_AltOpen, MM_Send, MM_SetAs, MM_CreateMenu, MM_FileMenu, MM_OperationMenu, MM_Properties, MM_ViewMenu, MM_MiscMenu,
			                           MM_Exit};
			if (t == 2) // if directory
			{
				
				n_items = MAX_MM_ITEMS - 3;
				for (unsigned int i = 0; i < n_items; i++)
				{
					mm_procs[i] = prcs[i + 3];
					mm_items[i].lgp_id_small = (int)muitxt(lng_ind[i + 3]);
				}
			}
			else
			{
				unsigned int off;
				mm_procs[0] = MM_AltOpen;
				mm_items[0].lgp_id_small = (int)muitxt(ind_altopen);
				mm_procs[1] = MM_Send;
				mm_items[1].lgp_id_small = (int)muitxt(ind_bt);
				if (IsMedia(scfile->sname) || IsPict(scfile->sname))
				{
					n_items = MAX_MM_ITEMS;
					off = 0;
				}
				else
				{
					n_items = MAX_MM_ITEMS - 1;
					off = 1;
				}
				for (unsigned int i = 2; i < n_items; i++)
				{
					mm_procs[i] = prcs[i + off];
					mm_items[i].lgp_id_small = (int)muitxt(lng_ind[i + off]);
				}
			}
		}
		else
		{
			mm_procs[0] = MM_CreateMenu;
			mm_items[0].lgp_id_small = (int)muitxt(ind_create);
			
			unsigned int tmp_ind;
			if (buffer.count)
			{
				n_items = 4;
				tmp_ind = 2;
				mm_procs[1] = MM_OperationMenu;
				mm_items[1].lgp_id_small = (int)muitxt(ind_oper_m);
			}
			else
			{
				n_items = 3;
				tmp_ind = 1;
			}
			mm_procs[tmp_ind] = MM_MiscMenu;
			mm_items[tmp_ind].lgp_id_small = (int)muitxt(ind_misc_m);
			mm_procs[tmp_ind + 1] = MM_Exit;
			mm_items[tmp_ind + 1] .lgp_id_small = (int)muitxt(ind_exit);
		}
	}
	
	mm_desc.n_items = n_items;
	patch_header(&mm_header);
	mm_gui_id = CreateMenu(0, 0, &mm_desc, &mm_header, 0, n_items, NULL, NULL);
}
//-------------------------------------------------------------------------Set as-----------------------------------------------------------------------------//
#define MAX_SAM_ITEMS 0x04

static HEADER_DESC sam_header = {0, 0, 0, 0, NULL, (int)"Set as...", LGP_NULL};

void SAM_SetWall(GUI *gui)
{
	CurFullPathByName(scfile->sname);
	SetWallpaper(pathbuf);
	GeneralFunc_flag1(mm_gui_id, 0);
	GeneralFuncF1(2);
}

void SAM_SetLogo(GUI *gui)
{
	CurFullPathByName(scfile->sname);
	SetLogo(pathbuf);
	GeneralFunc_flag1(mm_gui_id, 0);
	GeneralFuncF1(2);
}

void SAM_SetScreenSaver(GUI *gui)
{
	CurFullPathByName(scfile->sname);
	SetScreenSaver(pathbuf);
	GeneralFunc_flag1(mm_gui_id, 0);
	GeneralFuncF1(2);
}

MENUITEM_DESC sam_items[MAX_SAM_ITEMS]=
{
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2}
};

MENUPROCS_DESC sam_procs[MAX_SAM_ITEMS];

static MENU_DESC sam_desc =
{
	8, NULL, NULL, NULL,
	softkeys,
	&skt,
	0x10,
	NULL,
	sam_items,
	sam_procs,
	0
};

void CreateSetAsMenu(void)
{
	int n_items = 3;
	sam_desc.n_items = n_items;
	
	
	sam_items[0].lgp_id_small = (int)muitxt(ind_wall);
	sam_items[1].lgp_id_small = (int)muitxt(ind_logo);
	sam_items[2].lgp_id_small = (int)muitxt(ind_ss);
	sam_procs[0] = SAM_SetWall;
	sam_procs[1] = SAM_SetLogo;
	sam_procs[2] = SAM_SetScreenSaver;
	
	patch_header(&sam_header);
	sam_header.lgp_id = (int)muitxt(ind_set_as);
	CreateMenu(0, 0, &sam_desc, &sam_header, 0, n_items, NULL, NULL);
}

//------------------------------------------------------------------------Create Menu-------------------------------------------------------------------------//

#define MAX_CM_ITEMS 0x03

static HEADER_DESC cm_header = {0, 0, 0, 0, NULL, (int)"Create...", LGP_NULL};

void CM_NewFile(GUI *gui)
{
	GeneralFunc_flag1(mm_gui_id, 0);
	GeneralFuncF1(2);
	DoNewFile();
}

void CM_NewDir(GUI *gui)
{
	GeneralFunc_flag1(mm_gui_id, 0);
	GeneralFuncF1(2);
	DoNewDir();
}

void CM_Archive(GUI *gui)
{
	GeneralFunc_flag1(mm_gui_id, 0);
	GeneralFuncF1(2);
	DoArchive();
}

MENUITEM_DESC cm_items[MAX_CM_ITEMS]=
{
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
};

MENUPROCS_DESC cm_procs[MAX_CM_ITEMS]=
{
	CM_NewFile,
	CM_NewDir,
	CM_Archive
};

static MENU_DESC cm_desc =
{
	8, NULL, NULL, NULL,
	softkeys,
	&skt,
	0x10,
	NULL,
	cm_items,
	cm_procs,
	0
};

void CreateCreateMenu(void)
{
	unsigned int n_items = (IsCurFile()) ? MAX_CM_ITEMS : MAX_CM_ITEMS - 1;
	cm_desc.n_items = n_items;
	
	
	cm_items[0].lgp_id_small = (int)muitxt(ind_newfile);
	cm_items[1].lgp_id_small = (int)muitxt(ind_newdir);
	cm_items[2].lgp_id_small = (int)muitxt(ind_arch);
	
	patch_header(&cm_header);
	cm_header.lgp_id = (int)muitxt(ind_create);
	CreateMenu(0, 0, &cm_desc, &cm_header, 0, n_items, NULL, NULL);
}

//------------------------------------------------------------------File Menu---------------------------------------------------------------------------------//

#define MAX_FM_ITEMS 0x04

static HEADER_DESC fm_header = {0, 0, 0, 0, NULL, (int)"File...", LGP_NULL};

void FM_Check(GUI *gui)
{
	GeneralFunc_flag1(mm_gui_id, 0);
	GeneralFuncF1(2);
	DoChk();
	ShowFiles();
}

void FM_CheckAll(GUI *gui)
{
	GeneralFunc_flag1(mm_gui_id, 0);
	GeneralFuncF1(2);
	DoChkAll();
	ShowFiles();
}

void FM_UnCheckAll(GUI *gui)
{
	GeneralFunc_flag1(mm_gui_id, 0);
	GeneralFuncF1(2);
	DoUnCAll();
	ShowFiles();
}

void FM_Invert(GUI *gui)
{
	GeneralFunc_flag1(mm_gui_id, 0);
	GeneralFuncF1(2);
	DoInvChk();
	ShowFiles();
}

MENUITEM_DESC fm_items[MAX_FM_ITEMS]=
{
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2}
};

MENUPROCS_DESC fm_procs[MAX_FM_ITEMS]=
{
	FM_Check,
	FM_CheckAll,
	FM_UnCheckAll,
	FM_Invert
};

static MENU_DESC fm_desc =
{
	8, NULL, NULL, NULL,
	softkeys,
	&skt,
	0x10,
	NULL,
	fm_items,
	fm_procs,
	0
};

void CreateFileMenu(void)
{
	unsigned int n_items = MAX_FM_ITEMS;
	fm_desc.n_items = n_items;
	
	fm_items[0].lgp_id_small = (int)muitxt(ind_chk);
	fm_items[1].lgp_id_small = (int)muitxt(ind_chkall);
	fm_items[2].lgp_id_small = (int)muitxt(ind_unchall);
	fm_items[3].lgp_id_small = (int)muitxt(ind_invchk);
	
	patch_header(&fm_header);
	fm_header.lgp_id = (int)muitxt(ind_file_m);
	CreateMenu(0, 0, &fm_desc, &fm_header, 0, n_items, NULL, NULL);
}

//-----------------------------------------------------------------------Operation Menu-----------------------------------------------------------------------//
#define MAX_OM_ITEMS 0x07

static HEADER_DESC om_header = {0, 0, 0, 0, NULL, (int)"Operation...", LGP_NULL};

void OM_Paste(GUI *gui)
{
	GeneralFunc_flag1(mm_gui_id, 0);
	GeneralFuncF1(2);
	DoPaste();
}

void OM_Copy(GUI *gui)
{
	GeneralFunc_flag1(mm_gui_id, 0);
	GeneralFuncF1(2);
	DoCopy();
}

void OM_Move(GUI *gui)
{
	GeneralFunc_flag1(mm_gui_id, 0);
	GeneralFuncF1(2);
	DoMove();
}

void OM_Del(GUI *gui)
{
	GeneralFunc_flag1(mm_gui_id, 0);
	GeneralFuncF1(2);
	DoDel;
}

void OM_Rename(GUI *gui)
{
	GeneralFunc_flag1(mm_gui_id, 0);
	GeneralFuncF1(2);
	DoRen();
}

void OM_NaxExplorer(GUI *gui)
{
	GeneralFunc_flag1(mm_gui_id, 0);
	GeneralFuncF1(2);
	DoNatExpl();
}

void OM_Search(GUI *gui)
{
	GeneralFunc_flag1(mm_gui_id, 0);
	GeneralFuncF1(2);
	DoSearch();
}

MENUITEM_DESC om_items[MAX_OM_ITEMS]=
{
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2}
};

MENUPROCS_DESC om_procs[MAX_OM_ITEMS];

static MENU_DESC om_desc =
{
	8, NULL, NULL, NULL,
	softkeys,
	&skt,
	0x10,
	NULL,
	om_items,
	om_procs,
	0
};

void CreateOperationMenu(void)
{
	const int lng_ind[MAX_OM_ITEMS] = {ind_past, ind_copy, ind_move, ind_del, ind_rename, ind_natexpl, ind_find};
	const void *prcs[MAX_OM_ITEMS]  = {OM_Paste, OM_Copy, OM_Move, OM_Del, OM_Rename, OM_NaxExplorer, OM_Search};
	
	unsigned int n_items = 0;
	unsigned int arch_t  = 0;
	
	if ((arch_t = IsInArchive()))
	{
		/*if (arch_t = _7Z_ARCH)
		{
			n_items = 1;
			om_items[0].lgp_id_small = (int)muitxt(ind_copy);
			om_procs[0] = OM_Copy;
		}
		else
		{*/
			if (buffer.count)
			{
				n_items = 3;
				om_items[0].lgp_id_small = (int)muitxt(ind_past);
				om_procs[0] = OM_Paste;
				om_items[1].lgp_id_small = (int)muitxt(ind_copy);
				om_procs[1] = OM_Copy;
				om_items[2].lgp_id_small = (int)muitxt(ind_del);
				om_procs[2] = OM_Del;
			}
			else
			{
				n_items = 2;
				om_items[0].lgp_id_small = (int)muitxt(ind_copy);
				om_procs[0] = OM_Copy;
				om_items[1].lgp_id_small = (int)muitxt(ind_del);
				om_procs[1] = OM_Del;
			}
		//}
	}
	else
	{
		if (IsCurFile() != 0)
		{
			unsigned int off;
			if (buffer.count)
			{
				n_items = MAX_OM_ITEMS;
				off = 0;
			}
			else
			{
				n_items = MAX_OM_ITEMS - 1;
				off = 1;
			}
			for (unsigned int i = 0; i < n_items; i++)
			{
				om_items[i].lgp_id_small = (int)muitxt(lng_ind[i + off]);
				om_procs[i] = prcs[i + off];
			}
		}
		else
		{
			if (buffer.count)
			{
				n_items = 1;
				om_items[0].lgp_id_small = (int)muitxt(lng_ind[0]);
				om_procs[0] = prcs[0];
			}
		}
	}
	
	patch_header(&om_header);
	om_header.lgp_id = (int)muitxt(ind_oper_m);
	om_desc.n_items = n_items;
	CreateMenu(0, 0, &om_desc, &om_header, 0, n_items, NULL, NULL);
}

//-----------------------------------------------------------------------View menu------------------------------------------------------------------------------
unsigned int vm_gui_id;

#define MAX_VM_ITEMS 0x03

static HEADER_DESC vm_header = {0, 0, 0, 0, NULL, (int)"View...", LGP_NULL};

MENUITEM_DESC vm_items[MAX_VM_ITEMS] =
{
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2}
};

void VM_CreateSortingMenu(GUI *gui)
{
	CreateSortingMenu();
}

void VM_Filter(GUI *gui)
{
	GeneralFunc_flag1(mm_gui_id, 0);
	GeneralFuncF1(2);
	DoFilter();
}

void VM_Refresh(GUI *gui)
{
	GeneralFunc_flag1(mm_gui_id, 0);
	GeneralFuncF1(2);
	DoRefresh();
}

MENUPROCS_DESC vm_procs[MAX_VM_ITEMS] =
{
	VM_CreateSortingMenu,
	VM_Filter,
	VM_Refresh
};

static MENU_DESC vm_desc =
{
	8, NULL, NULL, NULL,
	softkeys,
	&skt,
	0x10,
	NULL,
	vm_items,
	vm_procs,
	MAX_VM_ITEMS
};

void CreateViewMenu(void)
{
	patch_header(&vm_header);
	vm_header.lgp_id = (int)muitxt(ind_view_m);
	vm_items[0].lgp_id_small = (int)muitxt(ind_sort_m);
	vm_items[1].lgp_id_small = (int)muitxt(ind_filter);
	vm_items[2].lgp_id_small = (int)muitxt(ind_refresh);
	vm_gui_id = CreateMenu(0, 0, &vm_desc, &vm_header, 0, MAX_VM_ITEMS, NULL, NULL);
}

//---------------------------------------------------------------Misc menu-------------------------------------------------------------------------------------
unsigned int mim_gui_id;

#define MAX_MIM_ITEMS 0x04

static HEADER_DESC mim_header = {0, 0, 0, 0, NULL, (int)"Misc...", LGP_NULL};

void MIM_Bookmarks(GUI *gui)
{
	CreateBookmarksMenu();
}

void MIM_DriveInfo(GUI *gui)
{
	GeneralFunc_flag1(mm_gui_id, 0);
	GeneralFuncF1(2);
	DoDrvInf();
}

void MIM_Settings(GUI *gui)
{
	DoOptions();
	GeneralFunc_flag1(mm_gui_id, 0);
	GeneralFuncF1(2);
}

void MIM_About(GUI *gui)
{
	GeneralFunc_flag1(mm_gui_id, 0);
	GeneralFuncF1(2);
	DoAboutDlg();
}


MENUITEM_DESC mim_items[MAX_MIM_ITEMS] =
{
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2}
};

MENUPROCS_DESC mim_procs[MAX_MIM_ITEMS] =
{
	MIM_Bookmarks,
	MIM_DriveInfo,
	MIM_Settings,
	MIM_About
};

static MENU_DESC mim_desc =
{
	8, NULL, NULL, NULL,
	softkeys,
	&skt,
	0x10,
	NULL,
	mim_items,
	mim_procs,
	MAX_MIM_ITEMS
};

void CreateMiscMenu(void)
{
	patch_header(&mim_header);
	mim_header.lgp_id = (int)muitxt(ind_misc_m);
	mim_items[0].lgp_id_small = (int)muitxt(ind_bm_m);
	mim_items[1].lgp_id_small = (int)muitxt(ind_drvinf);
	mim_items[2].lgp_id_small = (int)muitxt(ind_settings);
	mim_items[3].lgp_id_small = (int)muitxt(ind_about_m);
	mim_gui_id = CreateMenu(0, 0, &mim_desc, &mim_header, 0, MAX_MIM_ITEMS, NULL, NULL);
}

//-------------------------------------------------------------Bookmark menu----------------------------------------------------------------------------------//

#define MAX_BM_ITEMS 0x02

static HEADER_DESC bm_header = {0, 0, 0, 0, NULL, (int)"Bookmarks", LGP_NULL};

void BM_List(GUI *gui)
{
	GeneralFunc_flag1(mm_gui_id, 0);
	GeneralFunc_flag1(mim_gui_id, 0);
	GeneralFuncF1(2);
	DoBMList();
}

void BM_Add(GUI *gui)
{
	GeneralFunc_flag1(mm_gui_id, 0);
	GeneralFunc_flag1(mim_gui_id, 0);
	GeneralFuncF1(2);
	DoBMAdd();
}

MENUITEM_DESC bm_items[MAX_BM_ITEMS] =
{
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
	{NULL, 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
};

MENUPROCS_DESC bm_procs[MAX_BM_ITEMS] =
{
	BM_List,
	BM_Add
};

static MENU_DESC bm_desc =
{
	8, NULL, NULL, NULL,
	softkeys,
	&skt,
	0x10,
	NULL,
	bm_items,
	bm_procs,
	MAX_BM_ITEMS
};

void CreateBookmarksMenu(void)
{
	patch_header(&bm_header);
	mim_header.lgp_id = (int)muitxt(ind_bm_m);
	bm_items[0].lgp_id_small = (int)muitxt(ind_bml);
	bm_items[1].lgp_id_small = (int)muitxt(ind_add);
	CreateMenu(0, 0, &bm_desc, &bm_header, 0, MAX_BM_ITEMS, NULL, NULL);
}

//----------------------------------------------------------------------Sorting menu----------------------------------------------------------------------------

#define MAX_SM_ITEMS 0x05

static HEADER_DESC sm_header = {0, 0, 0, 0, NULL, (int)"Sorting", LGP_NULL};

#ifdef ELKA
	int icons1[] = {1342, 1341};
	int icons2[] = {1340, 1338};
#endif

MENUITEM_DESC sm_items[MAX_SM_ITEMS] =
{
	{&icons1[0], 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
	{&icons1[0], 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
	{&icons1[0], 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
	{&icons1[0], 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2},
	{&icons2[0], 0, LGP_NULL, 0, NULL, MENU_FLAG3, MENU_FLAG2}
};

void SM_SortN(GUI *gui)
{
	GeneralFunc_flag1(mm_gui_id, 0);
	GeneralFunc_flag1(vm_gui_id, 0);
	GeneralFuncF1(2);
	DoSortN();
}

void SM_SortE(GUI *gui)
{
	GeneralFunc_flag1(mm_gui_id, 0);
	GeneralFunc_flag1(vm_gui_id, 0);
	GeneralFuncF1(2);
	DoSortE();
}

void SM_SortS(GUI *gui)
{
	GeneralFunc_flag1(mm_gui_id, 0);
	GeneralFunc_flag1(vm_gui_id, 0);
	GeneralFuncF1(2);
	DoSortS();
}

void SM_SortD(GUI *gui)
{
	GeneralFunc_flag1(mm_gui_id, 0);
	GeneralFunc_flag1(vm_gui_id, 0);
	GeneralFuncF1(2);
	DoSortD();
}

inline void SetSortRIcon(void)
{
	if (_CurTab->sort >= ST_REVERS)
		sm_items[4].icon = &icons2[1];
	else
		sm_items[4].icon = &icons2[0];
}

void SM_SortR(GUI *gui)
{
	DoSortR();
	SetSortRIcon();
	RefreshGUI();
}


MENUPROCS_DESC sm_procs[MAX_SM_ITEMS] =
{
	SM_SortN,
	SM_SortE,
	SM_SortS,
	SM_SortD,
	SM_SortR
};

static MENU_DESC sm_desc =
{
	8, NULL, NULL, NULL,
	softkeys,
	&skt,
	0x11,
	NULL,
	sm_items,
	sm_procs,
	MAX_SM_ITEMS
};

void CreateSortingMenu(void)
{
	patch_header(&sm_header);
	sm_header.lgp_id = (int)muitxt(ind_sort_m);
	sm_items[0].lgp_id_small = (int)muitxt(ind_sortn);
	sm_items[1].lgp_id_small = (int)muitxt(ind_sorte);
	sm_items[2].lgp_id_small = (int)muitxt(ind_sorts);
	sm_items[3].lgp_id_small = (int)muitxt(ind_sortd);
	sm_items[4].lgp_id_small = (int)muitxt(ind_sortr);
	
	SetSortRIcon();
	for (unsigned int i = 0; i < 4; i++)
	{
		if (i == (_CurTab->sort & STV_MASK))
			sm_items[i].icon = &icons1[1];
		else
			sm_items[i].icon = &icons1[0];
	}
	
	CreateMenu(0, 0, &sm_desc, &sm_header, 0, MAX_SM_ITEMS, NULL, NULL);
}
