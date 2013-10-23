#include <swilib.h>
#include <mxml.h>
#include "libxce.h"

mxml_node_t* xce_load(const char *path)
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
			return NULL;
		}
		tree = mxmlLoadFile(NULL, f, MXML_OPAQUE_CALLBACK);
		fclose(f);
		if (tree == NULL)
		{
			strcpy(msg, "Can't load xml tree!");
			MsgBoxError(1, (int)msg);
			return NULL;
		}
		const char *ver = mxmlElementGetAttr(tree, "version");
		if (ver)
		{
			if (strcmp(ver, CONFIG_VERSION) == 0)
			{
				return tree;
			}
			else
			{
				strcpy(msg, "Version is not supported!");
				MsgBoxError(1, (int)msg);
				mxmlDelete(tree);
				return NULL;
			}
		}
		else
		{
			strcpy(msg, "Version is not found!");
			MsgBoxError(1, (int)msg);
			mxmlDelete(tree);
			return NULL;
		}
	}
}

void xce_delete(mxml_node_t *tree)
{
	mxmlDelete(tree);
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
	int i = get_int(tree, "combobox", name);
	return (!i) ? 0 : i - 1;
}

int xce_get_key(mxml_node_t *tree, const char *name)
{
	mxml_node_t *node = mxmlFindElement(tree, tree, "key", "name", name, MXML_DESCEND);
	const char *opaque = mxmlGetOpaque(node);
	if (opaque)
	{
		if (opaque[0] >= '0' && opaque[0] <= '9')
			return opaque[0];
		else if (strcmp(opaque, "Softkey left") == 0) return LEFT_SOFT;
		else if (strcmp(opaque, "Softkey right") == 0) return RIGHT_SOFT;
		else if (strcmp(opaque, "Left") == 0) return LEFT_BUTTON;
		else if (strcmp(opaque, "Right") == 0) return RIGHT_BUTTON;
		else if (strcmp(opaque, "Up") == 0) return UP_BUTTON;
		else if (strcmp(opaque, "Down") == 0) return DOWN_BUTTON;
		else if (strcmp(opaque, "Enter") == 0) return ENTER_BUTTON;
		else if (strcmp(opaque, "Green") == 0) return GREEN_BUTTON;
		else if (strcmp(opaque, "Volume+") == 0) return VOL_UP_BUTTON;
		else if (strcmp(opaque, "Volume-") == 0) return VOL_DOWN_BUTTON;
		else if (strcmp(opaque, "#") == 0) return '#';
		else if (strcmp(opaque, "*") == 0) return '*';
	}
	return 0;
}

int xce_get_checkbox(mxml_node_t *tree, const char *name)
{
	mxml_node_t *node = mxmlFindElement(tree, tree, "checkbox", "name", name, MXML_DESCEND);
	const char *opaque = mxmlGetOpaque(node);
	if (opaque)
	{
		return (strcmp(opaque, "true") == 0) ? 1 : 0;
	}
	return 0;
}

void xce_get_color(mxml_node_t *tree, const char *name, char *color)
{
	mxml_node_t *node = mxmlFindElement(tree, tree, "color", "name", name, MXML_DESCEND);
	const char *opaque = mxmlGetOpaque(node);
	if (opaque)
	{
		int col[4];
		sscanf(opaque, "%02x%02x%02x%02x", &col[0], &col[1], &col[2], &col[3]);
		for (int i = 0; i < 4; color[i] = col[i], i++);
	}
	else
	{
		for (int i = 0; i < 4; color[i] = 0x00, i++);
	}
}

void xce_get_position(mxml_node_t *tree, const char *name, int *x, int *y)
{
	mxml_node_t *node = mxmlFindElement(tree, tree, "position", "name", name, MXML_DESCEND);
	const char *opaque = mxmlGetOpaque(node);
	if (opaque)
	{
		sscanf(opaque, "%d,%d", x, y);
	}
	else
	{
		*x = 0;
		*y = 0;
	}
}

void xce_get_rect(mxml_node_t *tree, const char *name, RECT *rc)
{
	mxml_node_t *node = mxmlFindElement(tree, tree, "rect", "name", name, MXML_DESCEND);
	const char *opaque = mxmlGetOpaque(node);
	if (opaque)
	{
		sscanf(opaque, "%d,%d;%d,%d", &rc->x, &rc->y, &rc->x2, &rc->y2);
	}
	else
	{
		rc->x = 0; rc->y = 0;
		rc->x2 = 0; rc->y2 = 0;
	}
}

void xce_get_string(mxml_node_t *tree, const char *name, char *str)
{
	mxml_node_t *node = mxmlFindElement(tree, tree, "string", "name", name, MXML_DESCEND);
	const char *opaque = mxmlGetOpaque(node);
	if (opaque)
	{
		strcpy(str, opaque);
	}
}
