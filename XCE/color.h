#ifndef _COLOR_H_
	#define _COLOR_H_


void CreateColorGUI(const char *hdr, const int *color);

void ws_set_color(WSHDR *ws, const char *color);
void get_color_from_str(char *color, const char *color_str);

#endif
