// SPDX-License-Identifier: MIT

#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int
if_file_exists (const char *fpath)
{
  if (strlen (fpath) >= 1)
    return access (fpath, F_OK);
  fprintf (stderr, "File path can not be empty\n");
  return 1;
}

int
if_dir_exists (const char *dpath)
{
  if (strlen (dpath) >= 1)
    {
      DIR *dir = opendir (dpath);

      if (dir)
        {
          closedir (dir);
          return 0;
        }
      else if (ENOENT == errno)
        return -1;
      else
        {
          perror (dpath);
          return 1;
        }
    }
  fprintf (stderr, "Directory path can not be empty\n");
  return 1;
}

int
create_file (const char *fpath)
{
  if (if_file_exists (fpath) == 0)
    {
      fprintf (stderr, "%s: File already exists.\n", fpath);
      return 1;
    }

  FILE *file = fopen (fpath, "w");

  if (file == NULL)
    {
      perror (fpath);
      return 1;
    }

  // all good;
  fclose (file);
  return 0;
}

int
create_dir (const char *dpath)
{
  if (strlen (dpath) < 1)
    return 1;

  int dir_status = 0;

  struct stat st = { 0 };

  if (stat (dpath, &st) == -1)
    {
      dir_status = mkdir (dpath, 0700);
    }

  return dir_status;
}
