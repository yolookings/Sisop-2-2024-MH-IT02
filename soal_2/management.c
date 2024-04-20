#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <curl/curl.h>
#include <time.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

// Fungsi untuk mendekripsi string menggunakan ROT19
void decrypt_string(char *str) 
{
    int i = 0;
    while (str[i] != '\0') 
    {
        if (str[i] >= 'a' && str[i] <= 'z') {
            str[i] = ((str[i] - 'a' + 19) % 26) + 'a';
        } else if (str[i] >= 'A' && str[i] <= 'Z') {
            str[i] = ((str[i] - 'A' + 19) % 26) + 'A';
        }
        i++;
    }
}

// Fungsi untuk menghapus sebuah file
void delete_file(char *file) 
{
    if (remove(file) == 0) 
    {
        printf("File %s berhasil dihapus.\n", file);
    } else 
    {
        printf("Gagal menghapus file %s\n", file);
    }
}

// Fungsi untuk mencatat tindakan ke dalam log file
void log_file_action(char *username, char *file, char *action) 
{
    FILE *logFile = fopen("history.log", "a");
    if (logFile == NULL) 
    {
        perror("Error membuka file log");
        exit(EXIT_FAILURE);
    }
    time_t rawtime;
    struct tm *timeinfo;
    char timestamp[20];
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    strftime(timestamp, sizeof(timestamp), "%H:%M:%S", timeinfo);
    fprintf(logFile, "[%s][%s] - %s - %s\n", username, timestamp, file, action);
    fclose(logFile);
}

// Fungsi untuk mengelola file dalam direktori 'library'
void process_directory_files() 
{
    DIR *dir;
    struct dirent *entry;

    dir = opendir("library/");
    if (dir == NULL) 
    {
        perror("Error membuka direktori");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) 
    {
        if (entry->d_type == DT_REG) 
        { 
            char *file = entry->d_name;
            char deskripsi_file[256];

            strcpy(deskripsi_file, file);
            decrypt_string(deskripsi_file);

            if (strstr(deskripsi_file, "d3Let3") != NULL) 
            {
                delete_file(file);
            } else if (strstr(deskripsi_file, "r3N4mE") != NULL) 
            {
                char *file_ext = strrchr(file, '.');
                if (file_ext != NULL) {
                    if (strcmp(file_ext, ".ts") == 0) 
                    {
                        rename(file, "helper.ts");
                    } else if (strcmp(file_ext, ".py") == 0)
                    {
                        rename(file, "calculator.py");
                    } else if (strcmp(file_ext, ".go") == 0) 
                    {
                        rename(file, "server.go");
                    } else
                    {
                        rename(file, "renamed.file");
                    }
                }
            }
        }
    }

    closedir(dir);
}

// Fungsi untuk melakukan backup file-file tertentu dari direktori 'library'
void perform_backup() 
{
    DIR *dir;
    struct dirent *entry;

    dir = opendir("library/"); 
    if (dir == NULL) 
    {
        perror("Error membuka direktori");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) 
    {
        if (entry->d_type == DT_REG) 
        { 
            char *file = entry->d_name;
            char deskripsi_file[256];
            strcpy(deskripsi_file, file);
            decrypt_string(deskripsi_file);

            if (strstr(deskripsi_file, "m0V3") != NULL) 
            {
                char backup[256];
                sprintf(backup, "backup/%s", file);
                rename(file, backup);
                log_file_action("paul", file, "Berhasil dipindahkan ke backup.");
            }
        }
    }

    closedir(dir);
}

// Fungsi untuk mengembalikan file dari direktori 'backup' ke 'library'
void restore_backup_files() 
{
    DIR *dir;
    struct dirent *entry;
    dir = opendir("library/backup/");
    if (dir == NULL) 
    {
        perror("Error membuka direktori");
        exit(EXIT_FAILURE);
    }

    while ((entry = readdir(dir)) != NULL) 
    {
        if (entry->d_type == DT_REG) 
        { 
            char *file = entry->d_name;
            char deskripsi_file[256];
            strcpy(deskripsi_file, file);
            decrypt_string(deskripsi_file);
            char restored_path[256];
            sprintf(restored_path, "%s", file);
            char backup[256];
            sprintf(backup, "backup/%s", file);
            rename(backup, restored_path);
            log_file_action("paul", file, "Berhasil dipulihkan dari backup.");
        }
    }

    closedir(dir);
}

// Fungsi untuk menangani sinyal yang diterima oleh proses
void handle_signals(int signum) 
{
    char *mode;
    if (signum == SIGRTMIN) 
    {
        mode = "default";
    } else if (signum == SIGUSR1) 
    {
        mode = "backup";
        perform_backup(); 
    } else if (signum == SIGUSR2) 
    {
        mode = "restore";
        restore_backup_files(); 
    }

    log_file_action("daemon", "daemon", mode);
}

// Fungsi utama program
int main() 
{
    // Proses pertama: Download file library.zip dari URL yang ditentukan menggunakan wget
    pid_t pid = fork();
    if (pid < 0) {
        perror("Error saat melakukan fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) 
    {
        char *download_link = "https://drive.google.com/uc?export=download&id=1rUIZmp10lXLtCIH3LAZJzRPeRks3Crup";
        char *zip_file = "library.zip";
        execlp("wget", "wget", "-O", zip_file, download_link, NULL);
        perror("Error menjalankan wget");
        exit(EXIT_FAILURE);
    } else 
    {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) {
            printf("Download berhasil.\n");
        } else 
        {
            printf("Download gagal.\n");
            exit(EXIT_FAILURE);
        }
    }

    // Proses kedua: Ekstraksi library.zip ke direktori 'library' menggunakan unzip
    pid = fork();
    if (pid < 0) 
    {
        perror("Error saat melakukan fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) 
    {
        execlp("unzip", "unzip", "library.zip", "-d", "library/", NULL);
        perror("Error menjalankan unzip");
        exit(EXIT_FAILURE);
    } else 
    {
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status) && WEXITSTATUS(status) == 0) 
        {
            printf("Ekstraksi berhasil.\n");
        } else 
        {
            printf("Ekstraksi gagal.\n");
            exit(EXIT_FAILURE);
        }
    }

    // Memproses file-file dalam direktori 'library'
    process_directory_files();

    // Menetapkan penanganan sinyal untuk tindakan backup dan restore
    signal(SIGRTMIN, handle_signals); 
    signal(SIGUSR1, handle_signals); 
    signal(SIGUSR2, handle_signals); 

    // Masuk ke dalam loop tak terbatas agar proses tetap berjalan
    while (1) 
    {
        sleep(1);
    }

    return 0;
}
