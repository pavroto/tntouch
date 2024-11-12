// SPDX-License-Identifier: MIT

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define DEFAULT_CONFIG_DIR "/.config"
#define PROJECT_DIR "/tntouch"
#define CONFIG_NAME "/default"

#define MAX_TEMPLATE_LENGTH 1000

char*
normalize_path
(char* path, int length)
{ 
  char buf;
  char* output = (char*)malloc(length);
  
  int k = 0;
  for (int i = 0; path[i] != '\0' && i < length; i++)
  {
    if (buf && buf == '/' && path[i] == '/')
      continue;

    if (path[i] == '\n')
      continue;

    output[k++] = path[i];
    buf = path[i];
  }
  output[k] = '\0';
  
  free(path);
  return output;
}

char* 
get_cpath
(void)
{
	char* xdg_cpath = getenv("XDG_CONFIG_HOME");
	if (xdg_cpath != NULL)
	{
		size_t len = 
			strlen(xdg_cpath) +
			strlen(PROJECT_DIR) +
			strlen(CONFIG_NAME) + 1;

		char* cpath = (char*)calloc(len, sizeof(char));
		if (cpath == NULL)
			return NULL;
	
		snprintf(cpath, len, "%s%s%s", xdg_cpath, PROJECT_DIR, CONFIG_NAME);

    cpath = normalize_path(cpath, len);

		return cpath;
	}

	char* hpath = getenv("HOME");
	if (hpath == NULL)
		return NULL;	

	size_t len = 
		strlen(hpath) + 
		strlen(DEFAULT_CONFIG_DIR) + 
		strlen(PROJECT_DIR) +
		strlen(CONFIG_NAME) + 1;

	char* cpath = (char*)calloc(len, sizeof(char));
	if (cpath == NULL)
		return NULL;

	snprintf(cpath, len, "%s%s%s%s", hpath, DEFAULT_CONFIG_DIR, PROJECT_DIR, CONFIG_NAME);

  cpath = normalize_path(cpath, len);

	return cpath;
}

int 
set_dtemplate
(const char *path)
{
	// TODO: set_dtemplate
	//
	// 1. Check if file exists
	// 2. Check if template is valid (maybe?)
	// 3. Set new default template
}

char* 
get_dtemplate
(void)
{	
	// TODO: get_dtemplate
	//
	// 1. Check if default template specified
	// 2. Return template text

	char *cpath = get_cpath();
	
	FILE* cfile = fopen(cpath, "r");
	if (cfile == NULL)
	{
		perror(cpath);
		free(cpath);
		return NULL;
	}	

	char* tpath = (char*)malloc(MAX_TEMPLATE_LENGTH);
	
 	int tpath_size = fread(tpath, sizeof(char), MAX_TEMPLATE_LENGTH-1, cfile);
	fclose(cfile);

	if (tpath_size <= 1)
  {
    fprintf(stderr, "Default template path is invalid.\n");
    free(tpath);
    free(cpath);
		return NULL;
  }

  tpath = normalize_path(tpath, strlen(tpath));

  FILE* tfile = fopen(tpath, "r");
  if (tfile == NULL)
  {
    perror(tpath);
    free(cpath);
    free(tpath);
    return NULL;
  }

  char* template = (char*)malloc(MAX_TEMPLATE_LENGTH);
  int template_size = fread(template, sizeof(char), MAX_TEMPLATE_LENGTH-1, tfile);
  fclose(tfile);

  if (template_size < 1)
  {
    fprintf(stderr, "Default template is invalid.\n");
    free(cpath);
    free(tpath);
    return NULL;
  }


  free(cpath);
  free(tpath);
  return template;
}
