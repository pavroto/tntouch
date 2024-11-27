// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <string.h>
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
