// SPDX-License-Identifier: MIT

#include <stdbool.h>
#include <stdio.h>

static int
if_file_exists (const char *fname)
{
  FILE *file = fopen (fname, "r");

  if (file == NULL)
    return false;

  fclose (file);
  return true;
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
