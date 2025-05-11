#ifndef __TADARCH__
#define __TADARCH__

#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "lz/lz.h"
#include <sys/types.h>


// Representa um arquivo no archive
struct archive {
   unsigned char name[1024];
   int udi;
   off_t oldSize;              // tamanho original
   off_t discSize;            // tamanho no disco
   struct timespec lastMod;   // última modificação
   unsigned long offset; 
   int isCompress;
};

// Diretório que agrupa os arquivos no archive
struct directory {
   struct archive *arch;  // vetor de arquivos
   unsigned long size;  // quantos arquivos inseridos
};

struct directory *create_directory();

struct directory *read_directory(FILE *fp);

struct archive *create_arch(char *name, int udi);

int add_arch(struct directory *dir, struct archive *arch);

void destroy_directory(struct directory *dir);

void calc_offset(struct directory *dir);

unsigned long buffer_size(struct directory *dir);

void insert_arch(FILE *fp, struct directory *dir, unsigned long buffer);

void write_directory(FILE *fp, struct directory *dir);

void print_directory(struct directory *dir);

void destroy_directory(struct directory *dir);

unsigned long buffer_size(struct directory *dir);

void move_data(FILE *fp, size_t from_offset, size_t to_offset, size_t size, unsigned char *buffer);

void update_index(struct directory*dir, int from, int to);

#endif // __TADARCH__

