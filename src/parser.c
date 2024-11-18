#include <stdio.h>
#include <stdlib.h>

#define MAX_TEMPLATE_LENGTH 1000
#define SHELL_BUF_LENGTH 250

char *
parse_shell (char *ptemplate, int *i)
{
  static int iterator = 1;
  fprintf (stderr, "%d:\n%s", iterator++, ptemplate);

  char *shell_script = (char *)malloc (SHELL_BUF_LENGTH);

  int pardepth = 1;
  int k = 0;

  while (pardepth != 0 && ptemplate[k] != '\0' && k < SHELL_BUF_LENGTH - 1)
    {
      if (ptemplate[k] == ')')
        {
          pardepth -= 1;
          if (pardepth == 0)
            {
              *i += k + 1;
              break;
            }
        }

      if (ptemplate[k] == '(')
        pardepth += 1;

      shell_script[k] = ptemplate[k];
      k++;
    }

  shell_script[k + 1] = '\0';

  fprintf (stderr, "%s\n", shell_script);

  if (pardepth != 0)
    {
      fprintf (stderr, "Invalid template\n");
      free (shell_script);
      return NULL;
    }

  // TODO: execute bash command and return its stdout;

  return NULL;
}

char *
parse (char *template, char *ivalue)
{
  char *parsed_template = (char *)calloc (MAX_TEMPLATE_LENGTH, sizeof (char));

  int i = 0;
  int k = 0;
  while (template[i] != '\0')
    {
      // $(
      if (template[i] == '$' && template[i + 1] == '(')
        {
          char *bash_result = parse_shell (template + i + 2, &i);

          if (bash_result == NULL)
            {
              free (parsed_template);
              return NULL;
            }
        }
      // \i
      else if (template[i] == '\\' && template[i + 1] == 'i')
        {
          if (ivalue)
            {
            }
          else
            {
            }
        }

      parsed_template[k] = template[i];

      k++;
      i++;
    }

  free (parsed_template);
  return NULL;
}
