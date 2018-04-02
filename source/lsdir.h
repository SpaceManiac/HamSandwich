#ifndef LSDIR_H
#define LSDIR_H

#ifdef __GNUC__
#include <dirent.h>

class lsdir {
    DIR* dir;
public:
    explicit lsdir(const char* directory) {
        dir = opendir(directory);
    }
    ~lsdir() {
        if (dir) closedir(dir);
    }
    const char* next() {
        struct dirent *dp = dir ? readdir(dir) : NULL;
        return dp ? dp->d_name : NULL;
    }

#elif defined(_MSC_VER)
#include <io.h>
#include <string.h>

class lsdir {
    long hFile;
    struct _finddata_t filedata;
    bool first;

public:
    explicit lsdir(const char* directory) {
        char* buf = new char[strlen(directory) + 4];
        strcpy(buf, directory);
        strcat(buf, "\\*");

        hFile = _findfirst(buf, &filedata);
        first = true;
        delete[] buf;
    }
    ~lsdir() {
        if (hFile != -1) {
            _findclose(hFile);
        }
    }
    const char* next() {
        if (hFile == -1) {
            return NULL;
        }
        if (first) {
            first = false;
            return filedata.name;
        }
        if (_findnext(hFile, &filedata) != 0) {
            _findclose(hFile);
            hFile = -1;
            return NULL;
        }
        return filedata.name;
    }

#endif
private:
    lsdir(const lsdir&) = delete;
    lsdir(lsdir&&) = delete;
    lsdir& operator=(const lsdir&) = delete;
    lsdir& operator=(lsdir&&) = delete;
};

#endif // LSDIR_H
