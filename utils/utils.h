#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <pwd.h>
#include <ftw.h>

extern void find_home_path(char *home_path);
extern void write_file(const unsigned char *contents, const unsigned int contents_len, const char *out_path, mode_t permissions);
extern void cp_file(const char *source, const char *destination);
extern void system_no_output(const char *command);
