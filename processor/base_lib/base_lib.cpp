#include "base_lib.h"

#ifdef __linux__
    bool _IsBadReadPtr__(void* pointer, size_t size) {
        int nullfd = open("/dev/random", O_WRONLY);

        if (write(nullfd, pointer, size) < 0) {
            return true;
        }
        close(nullfd);

        return false;
    }
#elif _WIN32
    bool _IsBadReadPtr__(void* pointer) {
        MEMORY_BASIC_INFORMATION mbi = { 0 };
        if (::VirtualQuery(pointer, &mbi, sizeof(mbi))) {
            DWORD mask = (PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY);
            bool b = !(mbi.Protect & mask);

            // check the page is not a guard page
            if (mbi.Protect & (PAGE_GUARD | PAGE_NOACCESS)) {
                b = true;
            }

            return b;
        }

        return true;
    }
#endif

/*--------------------------------------------------------------------------*/
 
#ifdef _WIN32
    int file_cmp(const char* first_file, const char* second_file) {
        struct _stat first_stat;
        struct _stat second_stat;
        _stat(first_file, &first_stat);
        _stat(second_file, &second_stat);
        if (first_stat.st_mtime < second_stat.st_mtime) {
            return -1;
        }
        else {
            if (first_stat.st_mtime > second_stat.st_mtime) {
                return 1;
            }
            else {
                return 0;
            }
        }
    }
#endif
      
/*--------------------------------------------------------------------------*/

long long getFileSize(FILE* file/*!< - Pointer to opened file*/) {
    assert(file != nullptr);
    if (file == nullptr) {
        return -1;
    }

    fseek(file, 0L, SEEK_END);
    long long sz = ftell(file);
    rewind(file);

    return sz;
}
