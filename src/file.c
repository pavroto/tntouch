// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <unistd.h>

int
if_file_exists (const char *fname)
{
  return access (fname, F_OK) == 0;
}

int
create_file (const char *fname)
{
  if (if_file_exists (fname))
    {
      fprintf (stderr, "%s: File already exists.\n", fname);
      return 1;
    }

  FILE *file = fopen (fname, "w");

  if (file == NULL)
    {
      perror (fname);
      return 1;
    }

  // all good;
  fclose (file);
  return 0;
}
