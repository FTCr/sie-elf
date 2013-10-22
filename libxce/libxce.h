#ifndef _LIBXCE_H_
	#define _LIBXCE_H_

#define CONFIG_VERSION "1.0"

int xce_load(mxml_node_t **node, const char *path, mxml_type_t (*callback)(mxml_node_t *));

int xce_get_integer(mxml_node_t *tree, const char *name);

#endif
