#include <swilib.h>
#include <stdio.h>
#include <mxml.h>

void main(void)
{
	FILE *fp;
	char str[128];
	const char *xml_path = "0:\\Zbin\\Utilities\\mxmltest\\test.xml";
	
	fp = fopen(xml_path, "r");
	
	if (fp == NULL)
	{
		sprintf(str, "Can't open file: %s", xml_path);
		ShowMSG(1, (int)str);
		return;
	}
	mxml_node_t *tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);
	fclose(fp);
	
	if (tree == NULL)
	{
		ShowMSG(1, (int)"mxmlLoadFile = NULL");
		return;
	}
	
	const char *log_path = "0:\\Zbin\\Log\\mxmltest.log";
	fp = fopen(log_path, "a");
	if (fp == NULL)
	{
		sprintf(str, "Can't open file: %s", log_path);
		ShowMSG(1, (int)str);
		mxmlDelete(tree);
		return;
	}
	
	mxml_node_t *node = tree;
	mxml_node_t *next = NULL;
	while (node != NULL)
	{
		switch (mxmlGetType(node))
		{
			case MXML_ELEMENT:
				sprintf(str, "MXML_ELEMENT = %s\n", mxmlGetElement(node));
				fwrite(str, sizeof(char), strlen(str), fp);
			break;
			case MXML_TEXT:
				sprintf(str, "MXML_TEXT = %s\n", mxmlGetText(node, 0));
				fwrite(str, sizeof(char), strlen(str), fp);
			break;
		}
		
		
		
		next = mxmlGetFirstChild(node);
		if (next != NULL)
		{
			node = next;
		}
		else
		{
			next = mxmlGetNextSibling(node);
			if (next == NULL)
			{
				next = mxmlWalkNext(node, NULL, MXML_DESCEND);
			}
			node = next;
		}
	}
	fclose(fp);
	mxmlDelete(tree);
}
