// SPDX-License-Identifier: MIT

#include <stdio.h>
#include <stdlib.h>

#define TEMPLATE_INIT_SIZE 1000 // one thousand
#define TEMPLATE_STEP 1000 // one thousand

#define SHELL_SCRIPT_INIT_SIZE 250 // two hundred fifty
#define SHELL_SCRIPT_STEP 250 // two hundred fifty

#define SHELL_OUTPUT_INIT_SIZE 250 // two hundred fifty
#define SHELL_OUTPUT_STEP 250 // two hundred fifty

static void *
stepwise_realloc (void *s, const size_t size, size_t *cur_length,
                  const size_t step)
{
  s = realloc (s, (size * *cur_length) + (size * step));
  *cur_length = *cur_length + step;

  if (s == NULL)
    {
      perror ("Memory allocation fail");
      return NULL;
    }

  return s;
}

char *
parse_shell (char *ptemplate, size_t *i)
{
  size_t shell_script_size = SHELL_SCRIPT_INIT_SIZE;
  char *shell_script = (char *)malloc (SHELL_SCRIPT_INIT_SIZE);

  int depth = 1;
  size_t k = 0;

  while (ptemplate[k] != '\0')
    {
      if (k >= shell_script_size - 1)
        {
          char *buf = (char *)stepwise_realloc (shell_script, sizeof (char),
                                                &shell_script_size,
                                                SHELL_SCRIPT_STEP);
          if (buf == NULL)
            {
              free (shell_script);
              return NULL;
            }

          shell_script = buf;
        }

      if (ptemplate[k] == ')')
        {
          depth -= 1;
          if (depth == 0)
            {
              *i += k + 1;
              break;
            }
        }

      if (ptemplate[k] == '(')
        depth += 1;

      shell_script[k] = ptemplate[k];
      k++;
    }

  shell_script[k] = '\0';

  if (depth != 0)
    {
      fprintf (stderr, "Invalid template\n");
      free (shell_script);
      return NULL;
    }

  FILE *shell_stream = popen (shell_script, "r");
  if (shell_stream == NULL)
    {
      perror ("popen");
      free (shell_script);
      return NULL;
    }

  size_t shell_output_size = SHELL_OUTPUT_INIT_SIZE;
  char *shell_output = (char *)malloc (SHELL_OUTPUT_INIT_SIZE);

  size_t chunk_size = 0;
  size_t read_bytes = 0;
  while (!feof (shell_stream))
    {
      if (read_bytes + SHELL_OUTPUT_STEP > shell_output_size)
        {
          char *buf = (char *)stepwise_realloc (shell_output, sizeof (char),
                                                &shell_output_size,
                                                SHELL_OUTPUT_STEP);
          if (buf == NULL)
            {
              free (shell_script);
              free (shell_output);
              pclose (shell_stream);
              return NULL;
            }
          shell_output = buf;
        }

      chunk_size = fread (shell_output + read_bytes, sizeof (char),
                          SHELL_OUTPUT_STEP, shell_stream);
      read_bytes += chunk_size;
    }
  free (shell_script);
  pclose (shell_stream);

  return shell_output;
}

char *
parse (char *template)
{
  size_t parsed_template_size = TEMPLATE_INIT_SIZE;
  char *parsed_template = (char *)malloc (TEMPLATE_INIT_SIZE);

  size_t i = 0;
  int k = 0;
  while (template[i] != '\0')
    {
      if (i >= parsed_template_size - 2)
        {
          char *buf = stepwise_realloc (parsed_template, sizeof (char),
                                        &parsed_template_size, TEMPLATE_STEP);
          if (buf == NULL)
            {
              free (parsed_template);
              return NULL;
            }
          parsed_template = buf;
        }
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
          if (1)
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
