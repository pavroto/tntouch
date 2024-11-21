// SPDX-License-Identifier: MIT

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TEMPLATE_INIT_SIZE 50
#define TEMPLATE_STEP 50

#define SHELL_SCRIPT_INIT_SIZE 50
#define SHELL_SCRIPT_STEP 50

#define SHELL_OUTPUT_INIT_SIZE 50
#define SHELL_OUTPUT_STEP 50

#define DEF_IVALUE_INIT_SIZE 50
#define DEF_IVALUE_STEP 50

static void
for_printf (char *s, size_t size)
{
  for (size_t i = 0; i < size; i++)
    {
      if (s[i] == '\0')
        printf ("\\0");
      else if (s[i] == '\n')
        printf ("\\n");
      else if (s[i] >= 32 && s[i] <= 126)
        printf ("%c", s[i]);
      else
        printf ("<%d>", s[i]);
    }
  printf ("\n\n");
}

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

static size_t
restrict_strcat (char *dst, char *src, char *restricted_chars)
{
  char *ptr = dst + strlen (dst);

  int k = 0;
  int if_allowed = 1;

  while (*src != '\0')
    {
      while (restricted_chars[k] != '\0')
        {
          if (*src == restricted_chars[k])
            {
              if_allowed = 0;
              break;
            }
          k++;
        }

      if (if_allowed)
        *ptr++ = *src++;
      else
        *src++;

      if_allowed = 1;
      k = 0;
    }

  return strlen (dst);
}

char *
parse_shell (char *ptemplate, size_t *i)
{

  size_t shell_script_size = SHELL_SCRIPT_INIT_SIZE;
  char *shell_script = (char *)calloc (SHELL_SCRIPT_INIT_SIZE, sizeof (char));

  int depth = 1;
  size_t k = 0;

  while (ptemplate[k] != '\0' && depth != 0)
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

      switch (ptemplate[k])
        {
        case ')':
          depth--;
          if (depth == 0)
            {
              *i += k + 1;
            }
          break;
        case '(':
          depth++;
          break;

        default:
          shell_script[k] = ptemplate[k];
          break;
        }

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
  char *shell_output = (char *)calloc (SHELL_OUTPUT_INIT_SIZE, sizeof (char));

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
parse (char *template, char *ivalue)
{
  size_t ivalue_len = 0;
  if (ivalue)
    ivalue_len = strlen (ivalue);

  size_t parsed_template_size = TEMPLATE_INIT_SIZE;
  char *parsed_template = (char *)calloc (TEMPLATE_INIT_SIZE, sizeof (char));

  size_t i = 0;
  size_t k = 0;
  while (template[i] != '\0')
    {
      if (k >= parsed_template_size - 2)
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

      // $()
      if (template[i] == '$' && template[i + 1] == '('
          && template[i - 1] != '\\')
        {
          i += 2;
          char *bash_result = parse_shell (template + i, &i);

          if (bash_result == NULL)
            {
              free (parsed_template);
              return NULL;
            }

          size_t bash_result_len = strlen (bash_result);
          if (bash_result_len + k > parsed_template_size)
            {
              char *buf = (char *)stepwise_realloc (
                  parsed_template, sizeof (char), &parsed_template_size,
                  TEMPLATE_STEP * ceil (bash_result_len / TEMPLATE_STEP));

              if (buf == NULL)
                {
                  free (parsed_template);
                  free (bash_result);
                  return NULL;
                }

              parsed_template = buf;
            }

          k = restrict_strcat (parsed_template, bash_result, "\n/ ");
          continue;
        }
      // \i[()]
      else if (template[i] == '\\' && template[i + 1] == 'i'
               && template[i - 1] != '\\')
        {
          if (ivalue)
            {
              if (template[i + 2] == '(')
                {
                  i += 3;
                  int depth = 1;
                  while (template[i] != '\0' && depth != 0)
                    {
                      if (template[i] == '(')
                        depth++;
                      if (template[i] == ')')
                        {
                          depth--;
                          if (depth == 0)
                            {
                              i++;
                              break;
                            }
                        }
                      i++;
                    }
                  if (ivalue_len + k > parsed_template_size)
                    {
                      char *buf = (char *)stepwise_realloc (
                          parsed_template, sizeof (char),
                          &parsed_template_size,
                          TEMPLATE_STEP * ceil (ivalue_len / TEMPLATE_STEP));

                      if (buf == NULL)
                        {
                          free (parsed_template);
                          return NULL;
                        }

                      parsed_template = buf;
                    }
                  k = restrict_strcat (parsed_template, ivalue, "\n/ ");
                }
              else
                {
                  i += 2;

                  if (ivalue_len + k > parsed_template_size)
                    {
                      char *buf = (char *)stepwise_realloc (
                          parsed_template, sizeof (char),
                          &parsed_template_size,
                          TEMPLATE_STEP * ceil (ivalue_len / TEMPLATE_STEP));

                      if (buf == NULL)
                        {
                          free (parsed_template);
                          return NULL;
                        }

                      parsed_template = buf;
                    }
                  k = restrict_strcat (parsed_template, ivalue, "\n/ ");
                }
            }
          else
            {
              if (template[i + 2] != '(')
                {
                  fprintf (stderr, "Input value required\n");
                  free (parsed_template);
                  return NULL;
                }

              i += 3;
              int depth = 1;

              while (template[i] != '\0' && depth != 0)
                {
                  if (k >= parsed_template_size - 1)
                    {
                      char *buf = (char *)stepwise_realloc (
                          parsed_template, sizeof (char),
                          &parsed_template_size, TEMPLATE_STEP);

                      if (buf == NULL)
                        {
                          free (parsed_template);
                          return NULL;
                        }

                      parsed_template = buf;
                    }
                  if (template[i] == '(')
                    depth++;
                  if (template[i] == ')')
                    {
                      depth--;
                      if (depth == 0)
                        {
                          i++;
                          break;
                        }
                    }
                  parsed_template[k++] = template[i++];
                }
            }
        }

      if (template[i] != '\n')
        parsed_template[k++] = template[i++];
      else
        i++;
    }

  for_printf (parsed_template, parsed_template_size);
  return parsed_template;
}
