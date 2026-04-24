#include "utils.h"

// find path to home directory of user
void find_home_path(char *home_path) {
	char *sudo_user = getenv("SUDO_USER");
	struct passwd *pw = getpwnam(sudo_user);
	strcpy(home_path, pw->pw_dir);
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
	ssize_t read_characters;
	while ((read_characters = read(source_file, buffer, 1024)) > 0)
		write(destination_file, buffer, read_characters);
	
	close(source_file);
	close(destination_file);
}

// runs a command in shell and supresses its output
void system_no_output(const char *command) {
	char command_buffer[1024];
	sprintf(command_buffer, "%s >/dev/null 2>/dev/null", command);
	system(command_buffer);
}
