#ifndef _LIBXCE_H_
	#define _LIBXCE_H_

#define CONFIG_VERSION "1.0"

mxml_node_t* xce_load(const char *path);
void xce_delete(mxml_node_t *tree);

int xce_get_integer(mxml_node_t *tree, const char *name);
int xce_get_combobox(mxml_node_t *tree, const char *name);
int xce_get_key(mxml_node_t *tree, const char *name);
int xce_get_checkbox(mxml_node_t *tree, const char *name);
void xce_get_color(mxml_node_t *tree, const char *name, char *color);
void xce_get_position(mxml_node_t *tree, const char *name, int *x, int *y);
void xce_get_rect(mxml_node_t *tree, const char *name, RECT *rc);
void xce_get_string(mxml_node_t *tree, const char *name, char *str);


#endif
