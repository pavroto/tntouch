// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DEFAULT_CONFIG_DIR "/.config"
#define PROJECT_DIR "/tntouch"
#define CONFIG_NAME "/default"

// normalize_path:
// Removes repeating `/` characters from path.
char *
normalize_path (char *path, int length)
{
  char buf;
  char *output = (char *)malloc (length);

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

  free (path);
  return output;
}

// get_cpath:
// Returns path to the `default` file containing
// path to the default template.
char *
get_cpath (void)
{
  char *xdg_cpath = getenv ("XDG_CONFIG_HOME");
  if (xdg_cpath != NULL)
    {
      size_t len = strlen (xdg_cpath) + strlen (PROJECT_DIR)
          + strlen (CONFIG_NAME) + 1;

      char *cpath = (char *)calloc (len, sizeof (char));
      if (cpath == NULL)
        return NULL;

      snprintf (cpath, len, "%s%s%s", xdg_cpath, PROJECT_DIR, CONFIG_NAME);

      cpath = normalize_path (cpath, len);

      return cpath;
    }

  char *hpath = getenv ("HOME");
  if (hpath == NULL)
    return NULL;

  size_t len = strlen (hpath) + strlen (DEFAULT_CONFIG_DIR)
      + strlen (PROJECT_DIR) + strlen (CONFIG_NAME) + 1;

  char *cpath = (char *)calloc (len, sizeof (char));
  if (cpath == NULL)
    return NULL;

  snprintf (cpath, len, "%s%s%s%s", hpath, DEFAULT_CONFIG_DIR, PROJECT_DIR,
            CONFIG_NAME);

  cpath = normalize_path (cpath, len);

  return cpath;
}

// get_ptemplate:
// Find template using provided `path` and return
// its content.
char *
get_ptemplate (const char *path)
{
  FILE *tfile = fopen (path, "r");
  if (tfile == NULL)
    {
      perror (path);
      return NULL;
    }

  fseek (tfile, 0, SEEK_END);
  long tfile_size = ftell (tfile);
  rewind (tfile);

  char *template = (char *)malloc (tfile_size + 1);
  long template_size = fread (template, sizeof (char), tfile_size, tfile);
  fclose (tfile);

  if (template_size != tfile_size)
    {
      perror ("Error reading configuration file");
      free (template);
      return NULL;
    }

  return template;
}

// set_dtemplate:
// Set new default template.

// int
// set_dtemplate (const char *path)
// {
//   // TODO: set_dtemplate
//   //
//   // 1. Check if file exists
//   // 2. Check if template is valid (maybe?)
//   // 3. Set new default template
//
//   return 1;
// }

// get_dtemplate:
// Find default template and return its content.
char *
get_dtemplate (void)
{
  char *cpath = get_cpath ();

  FILE *cfile = fopen (cpath, "r");
  if (cfile == NULL)
    {
      perror (cpath);
      free (cpath);
      return NULL;
    }

  fseek (cfile, 0, SEEK_END);
  size_t cfile_size = ftell (cfile);
  rewind (cfile);

  char *tpath = (char *)malloc (cfile_size + 1);

  size_t tpath_size = fread (tpath, sizeof (char), cfile_size, cfile);
  fclose (cfile);

  if (tpath_size != cfile_size)
    {
      perror ("Error reading default template");
      free (tpath);
      free (cpath);
      return NULL;
    }

  tpath = normalize_path (tpath, strlen (tpath));
  char *template = get_ptemplate (tpath);
  if (template == NULL)
    {
      free (cpath);
      free (tpath);
      return NULL;
    }

  free (cpath);
  free (tpath);
  return template;
}