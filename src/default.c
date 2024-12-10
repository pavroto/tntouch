// SPDX-License-Identifier: MIT

#include "file.h"
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
  char *output = (char *)calloc (length, sizeof (char));

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

// get_cdir_path:
// Returns path to the project's
// configuration directory
char *
get_cdir_path (void)
{
  char *xdg_config_home = getenv ("XDG_CONFIG_HOME");
  if (xdg_config_home != NULL)
    {
      size_t len = strlen (xdg_config_home) + strlen (PROJECT_DIR) + 1;

      char *config_dir_path = (char *)calloc (len, sizeof (char));
      if (config_dir_path == NULL)
        return NULL;

      snprintf (config_dir_path, len, "%s%s", xdg_config_home, PROJECT_DIR);

      config_dir_path = normalize_path (config_dir_path, len);

      return config_dir_path;
    }

  char *home_path = getenv ("HOME");
  if (home_path == NULL)
    return NULL;

  size_t len = strlen (home_path) + strlen (DEFAULT_CONFIG_DIR)
      + strlen (PROJECT_DIR) + 1;

  char *config_dir_path = (char *)calloc (len, sizeof (char));
  if (config_dir_path == NULL)
    return NULL;

  snprintf (config_dir_path, len, "%s%s%s", home_path, DEFAULT_CONFIG_DIR,
            PROJECT_DIR);

  config_dir_path = normalize_path (config_dir_path, len);

  return config_dir_path;
}

// get_cfile_path:
// Returns path to the `default` file containing
// path to the default template.
char *
get_cfile_path (void)
{
  char *config_file_path = get_cdir_path ();
  size_t config_file_path_len
      = strlen (config_file_path) + strlen (CONFIG_NAME) + 1;

  if (realloc (config_file_path, config_file_path_len) == NULL)
    return NULL;

  strcat (config_file_path, CONFIG_NAME);
  if (config_file_path == NULL)
    return NULL;

  return config_file_path;
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

  char *template = (char *)calloc (tfile_size + 1, sizeof (char));
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
int
set_dtemplate (const char *dtpath)
{
  int file_status = if_file_exists (dtpath);
  if (file_status == -1)
    {
      perror (dtpath);
      return 1;
    }
  else if (file_status == 1)
    return 1;

  char *dtpath_resolved;
  if ((dtpath_resolved = realpath (dtpath, NULL)) == NULL)
    {
      free (dtpath_resolved);
      return 1;
    }

  char *cfile_path = get_cfile_path ();
  char *cdir_path = get_cdir_path ();

  int cdir_status = if_dir_exists (cdir_path);
  if (cdir_status == -1)
    {
      if (create_dir (cdir_path) != 0)
        {
          perror (cdir_path);
          free (cfile_path);
          free (cdir_path);
          free (dtpath_resolved);
          return 1;
        }
    }
  else if (cdir_status != 0)
    {
      free (cfile_path);
      free (cdir_path);
      free (dtpath_resolved);
      return 1;
    }

  FILE *cfile = fopen (cfile_path, "w");
  if (cfile == NULL)
    {
      perror (cfile_path);
      free (cfile_path);
      free (cdir_path);
      free (dtpath_resolved);
      return 1;
    }

  size_t count = strlen (dtpath_resolved);
  size_t written = fwrite (dtpath_resolved, sizeof (char), count, cfile);
  if (written != count)
    {
      perror ("Error writing to file");
      free (cfile_path);
      free (cdir_path);
      free (dtpath_resolved);
      fclose (cfile);
      return 1;
    }

  free (cfile_path);
  free (cdir_path);
  free (dtpath_resolved);
  fclose (cfile);
  return 0;
}

// get_dtemplate:
// Find default template and return its content.
char *
get_dtemplate (void)
{
  char *cpath = get_cfile_path ();

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

  char *tpath = (char *)calloc (cfile_size + 1, sizeof (char));

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
