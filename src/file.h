// SPDX-License-Identifier: MIT

#ifndef _FILE_H_ //Include guard
#define _FILE_H_

int if_file_exists (const char *fpath);
int if_dir_exists (const char *dpath);
int create_file (const char *fpath);
int create_dir (const char *dpath);

#endif
