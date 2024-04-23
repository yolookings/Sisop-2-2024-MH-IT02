# Laporan Hasil Praktikum Sistem Operasi 2024 Modul 2 - IT02

## Anggota Kelompok IT 02 :

- Maulana Ahmad Zahiri (5027231010)
- Syela Zeruya Tandi Lalong (5027231076)
- Kharisma Fahrun Nisa' (5027231086)

## Daftar Isi

- [Soal 1](#soal-1)
- [Soal 2](#soal-2)
- [Soal 3](#soal-3)
- [Soal 4](#soal-4)

# Soal 1

# Soal 2

# Soal 3

## Deskripsi Soal

Pak Heze adalah seorang admin yang baik. Beliau ingin membuat sebuah program admin yang dapat memantau para pengguna sistemnya. Bantulah Pak Heze untuk membuat program tersebut!

## Pengerjaan

```c
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
```

## Soal 4

## Deskripsi Soal

Salomo memiliki passion yang sangat dalam di bidang sistem operasi. Saat ini, dia ingin mengotomasi kegiatan-kegiatan yang ia lakukan agar dapat bekerja secara efisien. Bantulah Salomo untuk membuat program yang dapat mengotomasi kegiatan dia!

## Pengerjaan

```c
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
```
