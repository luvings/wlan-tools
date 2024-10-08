#ifndef _DIRENT_H
# include <dirstream.h>
# include <dirent/dirent.h>

/* Now define the internal interfaces.  */
extern DIR *__opendir(__const char *__name);
extern int __closedir(DIR *__dirp);
extern struct dirent *__readdir(DIR *__dirp);
extern struct dirent64 *__readdir64(DIR *__dirp);
extern int __readdir_r(DIR *__dirp, struct dirent *__entry,
                       struct dirent **__result);
extern int __readdir64_r(DIR *__dirp, struct dirent64 *__entry,
                         struct dirent64 **__result);
extern int __scandir64(__const char * __dir,
                       struct dirent64  ***__namelist,
                       int (*__selector)(__const struct dirent64 *),
                       int (*__cmp)(__const void *, __const void *));
extern __ssize_t __getdents(int __fd, char *__buf, size_t __nbytes)
internal_function;
extern __ssize_t __getdents64(int __fd, char *__buf, size_t __nbytes)
internal_function;
extern int __alphasort64(const void *a, const void *b)
__attribute_pure__;
extern int __versionsort64(const void *a, const void *b)
__attribute_pure__;
#endif
