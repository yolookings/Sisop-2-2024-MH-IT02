#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <signal.h>
#include <sys/wait.h>
#include <pwd.h>
#include <time.h> // Untuk time_t dan localtime

FILE *log_file = NULL;

// Fungsi untuk menampilkan bantuan
void show_help() {
    printf("Usage: ./admin <option> <user>\n");
    printf("Options:\n");
    printf(" -m <user>   Monitor processes for the specified user\n");
    printf(" -s         Stop monitoring processes\n");
    printf(" -c <user>  Continuously fail processes for the specified user\n");
    printf(" -a <user>  Allow processes for the specified user\n");
}

// Fungsi untuk menjalankan program sebagai daemon
void run_as_daemon() {
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }
    if (setsid() < 0) {
        perror("setsid failed");
        exit(EXIT_FAILURE);
    }
    int fd = open("/dev/null", O_RDWR);
    dup2(fd, STDIN_FILENO);
    dup2(fd, STDOUT_FILENO);
    dup2(fd, STDERR_FILENO);
    close(fd);
}

// Fungsi untuk menulis ke file log
void write_to_log(char *message) {
    time_t rawtime;
    struct tm *timeinfo;

    time(&rawtime);
    timeinfo = localtime(&rawtime);

    fprintf(log_file, "[%s] %s\n", asctime(timeinfo), message);
    fflush(log_file);
}

// Fungsi untuk memantau proses
void monitor_processes(char *username) {
    if (log_file == NULL) {
        printf("Error: Log file not initialized.\n");
        return;
    }

    // Buka pipe untuk menjalankan perintah ps -u <username>
    FILE *ps_output = popen("ps -u", "r");
    if (ps_output == NULL) {
        perror("Failed to run ps command");
        return;
    }

    char line[256];
    int pid;
    char cmd[256];

    // Baca output dari perintah ps -u per baris
    while (fgets(line, sizeof(line), ps_output) != NULL) {
        // Parsing baris untuk mendapatkan PID dan nama proses
        if (sscanf(line, "%d %[^\n]", &pid, cmd) != 2) {
            fprintf(stderr, "Failed to parse line: %s", line);
            continue;
        }

        // Tulis ke file log
        char log_message[512];
        sprintf(log_message, "PID: %d, CMD: %s", pid, cmd);
        write_to_log(log_message);
    }

    // Tutup pipe
    pclose(ps_output);
}

// Fungsi untuk menggagalkan proses
void fail_processes(char *user) {
    DIR *dir;
    struct dirent *entry;
    char path[256];

    dir = opendir("/proc");
    if (!dir) {
        perror("opendir failed");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            sprintf(path, "/proc/%s/status", entry->d_name);
            FILE *status = fopen(path, "r");
            if (status) {
                char line[256];
                char username[256];
                int uid;
                while (fgets(line, sizeof(line), status)) {
                    if (strncmp(line, "Uid:", 4) == 0) {
                        sscanf(line, "Uid: %d", &uid);
                        struct passwd *pwd = getpwuid(uid);
                        if (pwd != NULL) {
                            strcpy(username, pwd->pw_name);
                            if (strcmp(username, user) == 0) {
                                // Mengirim sinyal SIGKILL ke proses
                                kill(atoi(entry->d_name), SIGKILL);
                            }
                        }
                    }
                }
                fclose(status);
            }
        }
    }
    closedir(dir);
}

// Fungsi untuk mengizinkan proses
void allow_processes(char *user) {
    DIR *dir;
    struct dirent *entry;
    char path[256];

    dir = opendir("/proc");
    if (!dir) {
        perror("opendir failed");
        return;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            sprintf(path, "/proc/%s/status", entry->d_name);
            FILE *status = fopen(path, "r");
            if (status) {
                char line[256];
                char username[256];
                int uid;
                while (fgets(line, sizeof(line), status)) {
                    if (strncmp(line, "Uid:", 4) == 0) {
                        sscanf(line, "Uid: %d", &uid);
                        struct passwd *pwd = getpwuid(uid);
                        if (pwd != NULL) {
                            strcpy(username, pwd->pw_name);
                            if (strcmp(username, user) == 0) {
                                // Mengirim sinyal SIGCONT untuk melanjutkan proses
                                kill(atoi(entry->d_name), SIGCONT);
                            }
                        }
                    }
                }
                fclose(status);
            }
        }
    }
    closedir(dir);
}

// Fungsi utama
int main(int argc, char *argv[]) {
    if (argc < 2) {
        show_help();
        return 1;
    }

    char *option = argv[1];

     if (argc == 2) {
        // Buat string untuk opsi `-u` dan nama pengguna
        char user_option[20];
        sprintf(user_option, "-u%s", argv[1]);
        
        // Menjalankan perintah `ps -u <option>`
        if (execlp("ps", "ps", user_option, NULL) == -1) {
            perror("execlp failed");
            return 1;
        }
    } 

    if (strcmp(option, "-m") == 0 || strcmp(option, "-s") == 0 || strcmp(option, "-c") == 0 || strcmp(option, "-a") == 0) {
        if (argc < 3 && strcmp(option, "-s") != 0) { // Tidak memerlukan argumen pengguna untuk opsi -s
            printf("Error: Missing user argument for option %s\n", option);
            show_help();
            return 1;
        }

        char *user = argc >= 3 ? argv[2] : "";

         if (strcmp(option, "-m") == 0) {
            // Memantau proses pengguna yang diberikan
            log_file = fopen("file.log", "a");
            if (!log_file) {
                perror("Error opening log file");
                return 1;
            }
            monitor_processes(user);
            fclose(log_file);
        } else if (strcmp(option, "-s") == 0) {
            // Menghentikan pemantauan dengan mengirim sinyal SIGKILL ke program admin
            if (log_file != NULL) {
                fclose(log_file);
            }
            return 0; // Keluar dari program
        } else if (strcmp(option, "-c") == 0) {
            // Menggagalkan proses untuk pengguna yang diberikan
            fail_processes(user);
        } else if (strcmp(option, "-a") == 0) {
            // Mengizinkan proses untuk pengguna yang diberikan
            allow_processes(user);
        }
    } else {
        printf("Error: Invalid option %s\n", option);
        show_help();
        return 1;
    }

    return 0;
}
