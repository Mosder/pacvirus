#include "utils.h"

// find path to home directory of user
void find_home_path(char *home_path) {
	char *sudo_user = getenv("SUDO_USER");
	struct passwd *pw = getpwnam(sudo_user);
	strcpy(home_path, pw->pw_dir);
}

// find virus path from /proc/self/exe
void find_virus_path(char *virus_path) {
	readlink("/proc/self/exe", virus_path, 256);
}

// writes file from buffer to disk
void write_file(const unsigned char *contents, const unsigned int contents_len, const char *out_path, mode_t permissions) {
	int f = open(out_path, O_WRONLY | O_CREAT | O_TRUNC, permissions);
	write(f, contents, contents_len);
	close(f);
}

// copies file
void cp_file(const char *source, const char *destination) {
	int source_file = open(source, O_RDONLY);
	int destination_file = open(destination, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	
	char buffer[1024];
	ssize_t bytes_read;
	while ((bytes_read = read(source_file, buffer, sizeof(buffer))) > 0)
		write(destination_file, buffer, bytes_read);
	
	close(source_file);
	close(destination_file);
}

// runs a command in shell and supresses its output
void system_no_output(const char *command) {
	char command_buffer[1024];
	sprintf(command_buffer, "%s >/dev/null 2>/dev/null", command);
	system(command_buffer);
}

// calculates given hash of file and saves it to output
void calculate_hash_of_file(const char *hash_type, const char *file_path, char *output) {
	int fd = open(file_path, O_RDONLY);
	
	EVP_MD_CTX *ctx = EVP_MD_CTX_new();
	const EVP_MD *md = EVP_get_digestbyname(hash_type);
	EVP_DigestInit_ex(ctx, md, NULL);
	
	char buffer[1024];
	ssize_t bytes_read;
	while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0)
		EVP_DigestUpdate(ctx, buffer, bytes_read);

	unsigned char hash[EVP_MAX_MD_SIZE];
	unsigned int hash_len;
	EVP_DigestFinal_ex(ctx, hash, &hash_len);
	EVP_MD_CTX_free(ctx);
	close(fd);

	for (int i = 0; i < hash_len; i++)
		sprintf(output + 2*i, "%02x", hash[i]);
	output[2*hash_len] = '\0';
}
