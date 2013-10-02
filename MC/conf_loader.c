#include <cfg_items.h>
#include "mc.h"

#include "conf_loader.h"
#include "config.h"
#include "gui.h"

char successed_config_filename[MAX_PATH] = "";

#pragma segment="CONFIG_C"
int SaveConfigData(const char *fname)
{
	/*int f;
	unsigned int ul;
	int result = -1;
	unsigned int len =(int)__segment_end("CONFIG_C")-(int)__segment_begin("CONFIG_C");

	if ((f = fopen(fname, A_ReadWrite+A_Create+A_Truncate+A_BIN, P_READ+P_WRITE, &ul)) != -1)
	{
		if (fwrite(f, __segment_begin("CONFIG_C"), len, &ul) == len) result = 0;
		fclose(f, &ul);
	}

	return(result);*/
	return 1;
}

int LoadConfigData(const char *fname, CFG_HDR *cfghdr0, int * __config_begin, int * __config_end, char *scf)
{
	int f;
	unsigned int ul;
	char *buf;
	int result = 0;
	void *cfg;
	
	cfg = (void*)cfghdr0;
	volatile unsigned int _segb = (volatile unsigned int)__config_begin;
	volatile unsigned int _sege = (volatile unsigned int)__config_end;
	
	unsigned int len = (_sege - _segb) - 4;
	if (!(buf = malloc(len))) return -1;
	if ((f = _open(fname, A_ReadOnly + A_BIN, 0, &ul)) != -1)
	{
		if (_read(f, buf, len, &ul) == (int)len)
		{
			memcpy(cfg, buf, len);
			_close(f,&ul);
		}
		else
		{
			_close(f, &ul);
			goto L_SAVENEWCFG;
		}
	}
	else
	{
		L_SAVENEWCFG:
			if ((f = _open(fname, A_ReadWrite + A_Create + A_Truncate, P_READ + P_WRITE, &ul)) != -1)
			{
				if (_write(f, cfg, len, &ul) != (int)len) result =- 1;
				_close(f, &ul);
			}
			else
				result =- 1;
	}
	mfree(buf);
	if (result >= 0) strcpy(successed_config_filename, fname);
	return result;
}

void InitTempDir()
{
	if (!CONFIG_TEMP_PATH[0])
		strcpy((char*)CONFIG_TEMP_PATH, DEFAULT_DISK":\\Temp");

	int pathLen = strlen(CONFIG_TEMP_PATH);
	if (pathLen > 0 && ((char*)CONFIG_TEMP_PATH)[pathLen - 1] == '\\')
		((char*)CONFIG_TEMP_PATH)[pathLen - 1] = 0;

	mktree((char*)CONFIG_TEMP_PATH);
}

void InitConfig()
{
  /*extern char *conf_name;
  if(strlen(successed_config_filename)==0)
  {
    sprintf(successed_config_filename, _s_sbcfg, etc_path, conf_name);
    // Сначала пробуем взять информацию с того диска с которого запускаемся
    successed_config_filename[0] = mcpath[0];
  }*/
  
  __CONFIG_EXTERN(0, cfghdr0);
  
  if (LoadConfigData("0:\\Zbin\\Etc\\MC.bcfg", __CONFIG(0, cfghdr0), successed_config_filename) < 0)
  {
    /*// Пробуем с 0:
    successed_config_filename[0] = '0';
    if (mcpath[0] == '0' || LoadConfigData(successed_config_filename) < 0)
    {
      // Пробуем с 4:
      successed_config_filename[0] = '4';*/
      LoadConfigData("4:\\Zbin\\Etc\\MC.bcfg", __CONFIG(0, cfghdr0), successed_config_filename);
    //}
  }

	// Setup drives info
	Drives[DRV_IDX_Cache].enabled	= CONFIG_SHOW_SYSDRV;
	Drives[DRV_IDX_Config].enabled	= CONFIG_SHOW_SYSDRV;
	Drives[DRV_IDX_MMC].enabled	= CONFIG_SHOW_MMC;

	// Setup fonts
	NormalFont = FONT_SMALL;
	BoldFont = FONT_SMALL_BOLD;

	// Init temp dir
	InitTempDir();

	// Setup hidden/system files visibility
	show_hidden = CONFIG_SHOW_HIDDEN;
	show_system = CONFIG_SHOW_SYSTEM;
	show_hiddrv = CONFIG_SHOW_SYSDRV;
}
