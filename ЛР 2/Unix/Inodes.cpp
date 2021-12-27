#include <sys/stat.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <map>
#include <iostream>
#include <vector>

struct entry{
    std::vector<std::string> files;
    std::vector<std::string> links;
};

std::map<ino_t, entry> filelinks;

int dirwalk(std::string path, bool checkLinks);
int dirwalk_c(std::string path, unsigned int levelup, bool checkLinks);

unsigned int max_level = 5;

int dirwalk(std::string path, bool checkLinks) {
    unsigned int level = 0;
    std::string name;
    std::string filepath;
    DIR* dfd;
    static struct stat st;
    static struct dirent* dp;

    if ((dfd = opendir(path.c_str())) == NULL) {
        perror(path.c_str());
        return 1;
    }
    while ((dp = readdir(dfd)) != NULL) {
        name = dp->d_name;
        filepath = path + "/" + name;
        if (name == "." || name == "..") {
            continue;
        }
        if (lstat(filepath.c_str(), &st) == -1) {
            perror(filepath.c_str());
            return 1;
        }
        /*
        if (S_ISLNK(st.st_mode)) {
            if (checkLinks) {
                filelinks[st.st_ino].links.push_back(name);
                continue;
            }
            printf("[%17ld|%ld] \033[91;106mL\033[0m %s %s -->(",
                st.st_ino,
                st.st_nlink,
                std::string(2 * level, '-').c_str(),
                name.c_str());
            for (auto file : filelinks[st.st_ino].files) {
                printf(", %s", file.c_str());
            }
            printf(")\n");
        }
        */
        if (S_ISREG(st.st_mode) && (st.st_nlink >= 2)) {
            if (checkLinks) {
                filelinks[st.st_ino].files.push_back(name);
                continue;
            }
            printf("[%17ld|%ld] \033[95;102mF\033[0m %s %s <--(", 
                st.st_ino, 
                st.st_nlink, 
                std::string(2*level, '-').c_str(), 
                name.c_str());
            for (auto file: filelinks[st.st_ino].files) {
                printf(", %s", file.c_str());
            }
            printf(")\n");
        }
        else if ((level < max_level) && S_ISDIR(st.st_mode)) {
            level += 1;
            if (!checkLinks) {
                printf("--------------------- \033[93;104mD\033[0m %s %s\n", std::string(2 * level, '-').c_str(), name.c_str());
            }
            dirwalk_c(filepath, level, checkLinks);
            level -= 1;
        }
    }
    closedir(dfd);
    return 0;
}

int dirwalk_c(std::string path, unsigned int levelup, bool checkLinks) {
    unsigned int level = levelup;
    std::string name;
    std::string filepath;
    DIR* dfd;
    static struct stat st;
    static struct dirent* dp;

    if ((dfd = opendir(path.c_str())) == NULL) {
        perror(path.c_str());
        return 1;
    }
    while ((dp = readdir(dfd)) != NULL) {
        name = dp->d_name;
        filepath = path + "/" + name;
        if (name == "." || name == "..") {
            continue;
        }
        if (lstat(filepath.c_str(), &st) == -1) {
            perror(filepath.c_str());
            return 1;
        }
        /*
        if (S_ISLNK(st.st_mode)) {
            if (checkLinks) {
                filelinks[st.st_ino].links.push_back(name);
                continue;
            }
            printf("[%17ld|%ld] \033[91;106mL\033[0m %s %s -->(",
                st.st_ino,
                st.st_nlink,
                std::string(2 * level, '-').c_str(),
                name.c_str());
            for (auto file : filelinks[st.st_ino].files) {
                printf(", %s", file.c_str());
            }
            printf(")\n");
        }
        */
        if (S_ISREG(st.st_mode) && (st.st_nlink >= 2)) {
            if (checkLinks) {
                filelinks[st.st_ino].files.push_back(name);
                continue;
            }
            printf("[%17ld|%ld] \033[95;102mF\033[0m %s %s <--(",
                st.st_ino,
                st.st_nlink,
                std::string(2 * level, '-').c_str(),
                name.c_str());
            for (auto file : filelinks[st.st_ino].files) {
                printf(", %s", file.c_str());
            }
            printf(")\n");
        }
        else if ((level < max_level) && S_ISDIR(st.st_mode)) {
            level += 1;
            if (!checkLinks) {
                printf("--------------------- \033[93;104mD\033[0m %s %s\n", 
                    std::string(2 * level, '-').c_str(), 
                    name.c_str());
            }
            dirwalk_c(filepath, level, checkLinks);
            level -= 1;
        }
    }
    closedir(dfd);
    return 0;
}

int main(){
    std::string path;

    std::cout << "Enter path:";
    std::cin >> path;
    std::cout << "Enter max depth:";
    std::cin >> max_level;
    dirwalk(path, true);
    dirwalk(path, false);
    return 0;
}