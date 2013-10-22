#include <swilib.h>
#include <mxml.h>
#include "libxce.h"

int xce_load(mxml_node_t **node, const char *path, mxml_type_t (*callback)(mxml_node_t *))
{
	if (path)
	{
		mxml_node_t *tree;
		FILE *f = fopen(path, "r");
		char msg[128];
		if (f == NULL)
		{
			strcpy(msg, "Can't open file!");
			MsgBoxError(1, (int)msg);
			return 0;
		}
		tree = mxmlLoadFile(NULL, f, callback);
		fclose(f);
		if (tree == NULL)
		{
			strcpy(msg, "Can't load xml tree!");
			MsgBoxError(1, (int)msg);
			return 0;
		}
		const char *ver = mxmlElementGetAttr(tree, "version");
		if (ver)
		{
			if (strcmp(ver, CONFIG_VERSION) == 0)
			{
				*node = tree;
				return 1;
			}
			else
			{
				strcpy(msg, "Version is not supported!");
				MsgBoxError(1, (int)msg);
				return 0;
			}
		}
		else
		{
			strcpy(msg, "Version is not found!");
			MsgBoxError(1, (int)msg);
			return 0;
		}
	}
}

static int get_int(mxml_node_t *tree, const char *name, const char *value)
{
	mxml_node_t *node = mxmlFindElement(tree, tree, name, "name", value, MXML_DESCEND);
	const char *opaque = mxmlGetOpaque(node);
	int i = 0;
	if (opaque)
	{
		i = atoi(opaque);
	}
	return i;
}

int xce_get_integer(mxml_node_t *tree, const char *name)
{
	return get_int(tree, "integer", name);
}

int xce_get_combobox(mxml_node_t *tree, const char *name)
{
	return get_int(tree, "combobox", name);
}


