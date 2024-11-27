// SPDX-License-Identifier: MIT

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "default.h"
#include "file.h"
#include "parser.h"

void show_help_text (void);

int
main (int argc, char *argv[])
{
  int opt;
  int hflag = 0;
  char *dvalue = NULL;
  char *tvalue = NULL;
  char *ivalue = NULL;

  while ((opt = getopt (argc, argv, "hd:t:")) != -1)
    {
      switch (opt)
        {
        // h: Show help page;
        case 'h':
          hflag = 1;
          break;

        // d: Specify default template;
        case 'd':
          dvalue = strdup (optarg);
          break;

        // t: Specify template path;
        case 't':
          tvalue = strdup (optarg);
          break;

        default:
          show_help_text ();
          return 1;
        }
    }

  if (hflag)
    {
      show_help_text ();
      return 0;
    }

  if (argc != 1)
    {
      char ifpdash = 0;
      for (int i = 1; i < argc; i++)
        {
          if (argv[i][0] == '-')
            {
              ifpdash = 1;
              continue;
            }
          else if (ifpdash)
            {
              ifpdash = 0;
              continue;
            }
          ivalue = argv[i];
          break;
        }
    }

  if (dvalue)
    return set_dtemplate (dvalue);

  char *template;
  if (tvalue)
    template = get_ptemplate (tvalue);
  else
    template = get_dtemplate ();

  if (template == NULL)
    return 1;

  char *parsed_template = parse (template, ivalue);
  if (parsed_template == NULL)
    {
      free (template);
      return 1;
    }

  int result = create_file (parsed_template);

  printf ("%s\n", parsed_template);

  free (template);
  free (parsed_template);

  return result;
}

void
show_help_text (void)
{
  static const char *help_text
      = "Usage: tntouch [OPTION]... PATH [VARIABLE]...\n"
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

  printf (help_text);
}
