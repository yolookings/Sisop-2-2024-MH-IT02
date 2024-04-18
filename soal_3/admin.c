#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]) {
    // Periksa jumlah argumen
    if (argc != 2) {
        printf("Penggunaan: %s <user>\n", argv[0]);
        return 1; // Keluar dengan kode kesalahan
    }

    // Ambil nama pengguna dari argumen
    char *username = argv[1];

    // Implementasikan pemantauan kegiatan pengguna di sini

    printf("Memantau kegiatan pengguna '%s'...\n", username);

    return 0;
}
