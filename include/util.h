#ifndef UTIL_H
#define UTIL_H

#define DEFAULT_TEXT_LENGTH 16

int getAnswer();
char* getText(FILE *file);
char askConfirmation(const char *text);
void backupFile(const char *filename);
void* wrp_malloc(size_t size);
void* wrp_realloc(void *ptr, size_t size);
FILE* wrp_fopen(const char *fn, const char *option);

#endif
