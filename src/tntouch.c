// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#include "default.h"

void show_help_text();

int main(int argc, char *argv[])
{
	int opt;
	int hflag = 0;
	char* dvalue = NULL;
	char* tvalue = NULL;
	
	while ((opt = getopt(argc, argv, "hd:t:")) != -1)
	{
		switch (opt)
		{
			// h: Show help page;
			case 'h':
				hflag = 1;
				break;

			// d: Specify default template;
			case 'd':
				dvalue = strdup(optarg);
				break;

			// t: Specify template path;
			case 't':
				tvalue = strdup(optarg);
				break;

			case '?':
				return 1;

			default:
				abort();
		}
	}

	if (hflag)
	{
		show_help_text();
		return 0;
	}

	char* result = get_dtemplate();
	if (result == NULL)
		return 1;

	printf("hflag: %d\ndvalue: %s\ntvalue: %s\n", hflag, dvalue, tvalue); 	

	// all good;
	return 0;
}

void show_help_text()
{
	static const char *help_text = \
		"Usage: tntouch [OPTION]... PATH [VARIABLE]...\n"
		"Use a template name for new files.\n"
		"Example: tntouch -t /path/to/template Foo\n"
		"\n"
		"Options:\n"
		"-t   Select a template.\n"
		"-d   Select a default template.\n"
		"-h   Show this help page.\n"
		"\n"
		"Report bugs to: <https://github.com/pavroto/tntouch/issues>\n"
		"Tntouch home page: <https://github.com/pavroto/tntouch>\n";

	printf(help_text);
}
