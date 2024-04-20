#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define MAX_APPS 10
#define MAX_NAME_LENGTH 20

// Variabel global untuk menyimpan PID dari setiap proses yang dijalankan
pid_t app_pids[MAX_APPS];
char app_names[MAX_APPS][MAX_NAME_LENGTH]; // Menyimpan nama aplikasi yang dijalankan
int num_apps_running = 0;

void open_apps_from_args(int argc, char *argv[]) {
    // Mulai dari argumen kedua, karena argumen pertama adalah nama program itu sendiri
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-o") == 0) {
            // Jika argumen adalah "-o", kita membaca nama aplikasi dan jumlah jendela yang diinginkan
            char *app_name = argv[++i];
            int num_windows = atoi(argv[++i]);

            printf("Opening %d windows of %s\n", num_windows, app_name);

            // Implementasi membuka aplikasi sesuai dengan argumen di sini
            // Misalnya, Anda dapat menggunakan fork() dan exec() untuk menjalankan aplikasi
            // di bawah ini hanyalah contoh sederhana
            for (int j = 0; j < num_windows; j++) {
                pid_t pid = fork();
                if (pid == 0) {
                    execlp(app_name, app_name, NULL);
                    // Jika execlp() gagal, cetak pesan kesalahan
                    perror("exec");
                    _exit(1);
                } else if (pid < 0) {
                    // Jika fork() gagal, cetak pesan kesalahan
                    perror("fork");
                } else {
                    // Simpan PID dan nama aplikasi dari proses yang dijalankan
                    app_pids[num_apps_running] = pid;
                    strcpy(app_names[num_apps_running], app_name);
                    num_apps_running++;
                }
            }
        }
    }
}

void open_apps_from_config(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char app_name[MAX_NAME_LENGTH];
    int num_windows;

    while (fscanf(file, "%s %d", app_name, &num_windows) == 2) {
        printf("Opening %d windows of %s\n", num_windows, app_name);

        // Implementasi membuka aplikasi sesuai dengan konfigurasi di sini
        // Misalnya, Anda dapat menggunakan fork() dan exec() untuk menjalankan aplikasi
        // di bawah ini hanyalah contoh sederhana
        for (int j = 0; j < num_windows; j++) {
            pid_t pid = fork();
            if (pid == 0) {
                execlp(app_name, app_name, NULL);
                // Jika execlp() gagal, cetak pesan kesalahan
                perror("exec");
                _exit(1);
            } else if (pid < 0) {
                // Jika fork() gagal, cetak pesan kesalahan
                perror("fork");
            } else {
                // Simpan PID dan nama aplikasi dari proses yang dijalankan
                app_pids[num_apps_running] = pid;
                strcpy(app_names[num_apps_running], app_name);
                num_apps_running++;
            }
        }
    }

    fclose(file);
}

void kill_all_apps() {
    for (int i = 0; i < num_apps_running; i++) {
        kill(app_pids[i], SIGKILL);
    }
    num_apps_running = 0;
}

void kill_apps_from_config(const char *filename) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        return;
    }

    char app_name[MAX_NAME_LENGTH];
    int num_windows;

    while (fscanf(file, "%s %d", app_name, &num_windows) == 2) {
        // Cari PID dari proses yang sesuai dengan konfigurasi dan kirim sinyal SIGKILL
        for (int i = 0; i < num_apps_running; i++) {
            // Bandingkan nama aplikasi dengan nama aplikasi yang dijalankan
            // dan kirim sinyal SIGKILL jika cocok
            if (strcmp(app_name, app_names[i]) == 0) {
                kill(app_pids[i], SIGKILL);
                break;
            }
        }
    }

    fclose(file);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage: %s -o <app1> <num1> -o <app2> <num2> ... \n", argv[0]);
        printf("       %s -f <filename>\n", argv[0]);
        printf("       %s -k\n", argv[0]);
        printf("       %s -k <filename>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "-o") == 0) {
        open_apps_from_args(argc, argv);
    } else if (strcmp(argv[1], "-f") == 0) {
        const char *filename = argv[2];
        open_apps_from_config(filename);
    } else if (strcmp(argv[1], "-k") == 0) {
        if (argc == 2) {
            kill_all_apps();
        } else {
            const char *filename = argv[2];
            kill_apps_from_config(filename);
        }
    } else {
        printf("Invalid command.\n");
        return 1;
    }

    return 0;
}
