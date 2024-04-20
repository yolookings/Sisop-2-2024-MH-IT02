#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <syslog.h>


void makeLog(const char *diredit, char *filename) {
    FILE *log = fopen("/home/vboxuser/Documents/kuman/virus.log", "a");
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    char time[100];
    strftime(time, sizeof(time), "[%d-%m-%Y][%H:%M:%S]", timeinfo);
    fprintf(log, "%s String aneh di %s berhasil diganti!\n", time, filename);
    fclose(log);
}

char *editContent(char *str, char *before, char *after) {
    char *pos, *temp;
    int index, before_len;
    int i, count = 0;

    before_len = strlen(before);

    for (i = 0; str[i] != '\0'; i++) {
        if (strstr(&str[i], before) == &str[i]) {
            count++;
            i += before_len - 1;
        }
    }

    temp = (char *)malloc(i + count * (strlen(after) - before_len) + 1);

    i = 0;
    while (*str) {
        if ((pos = strstr(str, before)) == str) {
            strcpy(&temp[i], after);
            i += strlen(after);
            str += before_len;
        } else {
            temp[i++] = *str++;
        }
    }

    temp[i] = '\0';
    return temp;
}

void editFile(const char *diredit) {
    struct dirent *entry;
    struct stat stat;
    char file_path[1024];
    char buffer[1024];
    char *kontenTelahDiUbah;

    DIR *dir = opendir(diredit);
    if (!dir) return;

    chdir(diredit);
    while((entry = readdir(dir))){
        if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) continue;

        if(lstat(entry->d_name, &stat) == -1) continue;

        if(S_ISDIR(stat.st_mode)){
            editFile(entry->d_name);
        }else if(S_ISREG(stat.st_mode)){
            sprintf(file_path, "%s/%s", diredit, entry->d_name);

            FILE *awal = fopen(file_path, "r");
            if (awal != NULL){
                fread(buffer, 1, sizeof(buffer), awal);
                fclose(awal);

                kontenTelahDiUbah = buffer;
                kontenTelahDiUbah = editContent(kontenTelahDiUbah, "m4LwAr3", "[MALWARE]");
                kontenTelahDiUbah = editContent(kontenTelahDiUbah, "5pYw4R3", "[SPYWARE]");
                kontenTelahDiUbah = editContent(kontenTelahDiUbah, "R4nS0mWaR3", "[RANSOMWARE]");
            
              if (strcmp(buffer, kontenTelahDiUbah) != 0) {
                    awal = fopen(file_path, "w");
                    fputs(kontenTelahDiUbah, awal);
                    fclose(awal);
                    makeLog(diredit, entry->d_name);
                    sleep(15);
              }
            }
        }
    }
  closedir(dir);
}

int main(int argc, char *argv[]){

  pid_t pid, sid;

  pid = fork();

  if (pid < 0)
  {
  exit(EXIT_FAILURE);
  }

  if (pid > 0)
  {
  exit(EXIT_SUCCESS);
  }

  umask(0);

  sid = setsid();
  if (sid < 0)
  {
  exit(EXIT_FAILURE);
  }

  if (chdir("/") < 0)
  {
  exit(EXIT_FAILURE);
  }

  close(STDIN_FILENO); 
  close(STDOUT_FILENO);
  close(STDERR_FILENO);

  while (1){
  sleep(15);
  editFile(argv[1]);
  }

return 0;
}
