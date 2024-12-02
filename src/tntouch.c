// SPDX-License-Identifier: MIT

#include <config.h>
#include <ctype.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "default.h"
#include "file.h"
#include "parser.h"

void show_help_text (void);
void show_version_text (void);

static int verbose_flag = 0; // TODO: Not used yet. Declared for future.
static int version_flag = 0;
static int help_flag = 0;

static char *default_value = NULL;
static char *input_value = NULL;
static char *template_value = NULL;

int
main (int argc, char *argv[])
{
  int c;

  while (1)
    {
      static struct option long_options[]
          = { // No arguments
              { "verbose", no_argument, &verbose_flag, 1 },
              { "help", no_argument, &help_flag, 1 },
              { "version", no_argument, &version_flag, 1 },

              // With arguments
              { "template", required_argument, 0, 't' },
              { "default", required_argument, 0, 'd' },
              { 0, 0, 0, 0 }
            };

      int option_index = 0;

      c = getopt_long (argc, argv, "t:d:", long_options, &option_index);

      if (c == -1)
        break;

      switch (c)
        {
        case 0:
          break;

        case 't':
          template_value = optarg;
          break;

        case 'd':
          default_value = optarg;
          break;

        case '?':
          break;

        default:
          abort ();
        }
    }

  if (help_flag)
    {
      show_help_text ();
      return 0;
    }

  if (version_flag)
    {
      show_version_text ();
      return 0;
    }

  // Find standalone argument if exists
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
          input_value = argv[i];
          break;
        }
    }

  if (default_value)
    return set_dtemplate (default_value);

  char *template;
  if (template_value)
    template = get_ptemplate (template_value);
  else
    template = get_dtemplate ();

  if (template == NULL)
    return 1;

  char *parsed_template = parse (template, input_value);
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
        "-t, --template   Select a template.\n"
        "-d, --default    Set a default template.\n"
        "--help           Show this help page.\n"
        "--version        Show package version.\n"
        "\n"
        "Report bugs to: <" PACKAGE_URL "/issues>\n"
        "Tntouch home page: <" PACKAGE_URL ">\n";

  printf (help_text);
}

void
show_version_text (void)
{
  static const char *version_text = PACKAGE_STRING
      "\nCopyright (c) 2024 pavroto\n"
      "MIT License"
      "\n"
      "This is free software; you are free to change and redistribute it.\n"
      "There is NO WARRANTY, to the extent permitted by law.\n"
      "More information here: <https://mit-license.org/>\n";

  printf (version_text);
}
