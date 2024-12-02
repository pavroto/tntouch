// SPDX-License-Identifier: MIT

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

static size_t
ceil_size_t_division (size_t x, size_t y)
{
  return (x + y - 1) / y;
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
  int is_allowed = 1;

  while (*src != '\0')
    {
      while (restricted_chars[k] != '\0')
        {
          if (*src == restricted_chars[k])
            {
              is_allowed = 0;
              break;
            }
          k++;
        }

      if (is_allowed)
        *ptr++ = *src++;
      else
        *src++;

      is_allowed = 1;
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
  int is_string = 0;
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

      if (ptemplate[k] == '"')
        is_string = (is_string) ? 0 : 1;
      if (is_string == 0)
        {
          if (ptemplate[k] == ')')
            {
              depth--;
              if (depth == 0)
                {
                  *i += k + 1;
                  break;
                }
            }

          if (ptemplate[k] == '(')
            depth++;
        }

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

static char
is_escaped (char *c)
{
  if (*c != '\\')
    return 0;

  char next = *(c + 1);

  switch (next)
    {
    case '\0':
      return -1;
      break;

    case 'i':
      return 'i';
      break;
    case ')':
      return ')';
      break;
    case '(':
      return '(';
      break;
    case '\\':
      return '\\';
      break;
    default:
      return 0;
      break;
    }
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
                  TEMPLATE_STEP
                      * ceil_size_t_division (bash_result_len, TEMPLATE_STEP));

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
                  int is_escape;
                  while (template[i] != '\0' && depth != 0)
                    {
                      is_escape = 0;

                      if (is_escaped (&template[i]))
                        {
                          is_escape = 1;
                          i++;
                        }

                      if (template[i] == '(' && !is_escape)
                        depth++;
                      if (template[i] == ')' && !is_escape)
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
                          TEMPLATE_STEP
                              * ceil_size_t_division (ivalue_len,
                                                      TEMPLATE_STEP));

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
                          TEMPLATE_STEP
                              * ceil_size_t_division (ivalue_len,
                                                      TEMPLATE_STEP));

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
              int is_escape;

              while (template[i] != '\0' && depth != 0)
                {
                  is_escape = 0;
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

                  if (is_escape = is_escaped (&template[i]))
                    i++;

                  if (template[i] == '(' && !is_escape)
                    depth++;
                  if (template[i] == ')' && !is_escape)
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

  return parsed_template;
}
