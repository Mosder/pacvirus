#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <pwd.h>
#include <ftw.h>
#include <openssl/evp.h>
#include <openssl/sha.h>
#include <openssl/md5.h>
#include <arpa/inet.h>
#include <sys/prctl.h>

extern void find_home_path(char *home_path);
extern void find_virus_path(char *home_path);
extern void write_file(const unsigned char *contents, const unsigned int contents_len, const char *out_path, mode_t permissions);
extern void cp_file(const char *source, const char *destination);
extern void system_no_output(const char *command);
extern void calculate_hash_of_file(const char *hash_type, const char *file_path, char *output);
