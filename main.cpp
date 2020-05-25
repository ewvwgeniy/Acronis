#include <iostream>
#include <sys/mman.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <unordered_map>

#define N 409
 // size of substring

using namespace std;

int main(int argc, char *argv[]) // 1- name of directory with files  2 - name of file to search in
{
    clock_t all_start = clock();
    unordered_map <string, int> mp;
    int fd, n, i;
    bool flag = false;
    void * mmf;
    DIR *dir;
    struct dirent *str;
    struct stat statbuf;
    char *dir_name = argv[1], buf[4096], file_name[255];


    if (argc < 3)
        return 0;
    else {
        if ((dir = opendir(dir_name)) == NULL)
            perror("opendir");
        else {
            while ((str = readdir(dir)) != NULL) {
                if ((DT_REG & str->d_type) != 0) {
                    sprintf(file_name, "%s/%s", argv[1], str->d_name);

                    if ((fd = open(file_name, O_RDONLY)) == -1)
                        perror("open");
                    else {
                        if (fstat(fd, &statbuf) == -1)
                            perror("fstat");
                        else {
                            if ((mmf = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED)
                                perror("mmap");
                            else {

                                if (statbuf.st_size >= N) {
                                    for (i = 0; i < statbuf.st_size - N + 1; i++) {
                                        snprintf(buf, N + 1, "%s", mmf + i);
                                        string sub_str = buf;
                                        if(mp.find(sub_str) == mp.end())
                                            mp[sub_str] = 1;
                                    }

                                }

                                if (munmap(mmf, statbuf.st_size) == -1)
                                    perror("munmap");
                            }
                        }


                        if (close(fd) == -1)
                            perror("close");
                    }
                }
            }

			if (closedir(dir) == -1)
                perror("closedir");
        }
    }

    // poisk podstrok iz faila

    clock_t search_start = clock();

    if ((fd = open(argv[2], O_RDONLY)) == -1)
        perror("open");
    else {
        if (fstat(fd, &statbuf) == -1)
            perror("fstat");
        else {
            if ((mmf = mmap(0, statbuf.st_size, PROT_READ, MAP_SHARED, fd, 0)) == MAP_FAILED)
                perror("mmap");
            else {
                if (statbuf.st_size >= N) {
                    for (i = 0; i < statbuf.st_size - N + 1; i++) {
                        snprintf(buf, N + 1, "%s", mmf + i);
                        string sub_str = buf;
                        if (mp.find(sub_str) != mp.end()) {
                            cout << "Found!" << endl << sub_str << endl;
                            flag = true;
                            break;
                        }
                    }
                }
                if (flag == false)
                    cout << "Not found :(" << endl;

                if (munmap(mmf, statbuf.st_size) == -1)
                    perror("munmap");
            }
        }

        if (close(fd) == -1)
            perror("close");
    }

    cout << "All time: " << ((double) (clock() - all_start))/CLOCKS_PER_SEC << " s\n";
    cout << "Search time: " << ((double) (clock() - search_start))/CLOCKS_PER_SEC << " s\n";
    return 0;
}
