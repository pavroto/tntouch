#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define LOG(m) fprintf(stderr, "LOG@%s@%d:%s\n", __FILE__, __LINE__, m)

int main(int argc, char *argv[])
{
	int opt;
	int hflag;
	char* dvalue = NULL;
	char* tvalue = NULL;
	
	while ((opt = getopt(argc, argv, "hd:t:")) != -1)
	{
		switch (opt)
		{
			// h: Show help page;
			case 'h':
				hflag = 1;
				
				#ifdef DEBUG
				LOG("Help flag triggered");
				#endif
				break;

			// d: Specify default template;
			case 'd':
				dvalue = strdup(optarg);
				LOG("Default flag triggered");
				break;

			// t: Specify template path;
			case 't':
				tvalue = strdup(optarg);
				LOG("Template flag triggered");
				break;

			default:
				LOG("Abort");
				abort();
		}
	}

	// all good;
	return 0;
}
