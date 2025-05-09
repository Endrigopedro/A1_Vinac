#ifndef __TADARCH__
#define __TADARCH__

#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "lz/lz.h"


// Representa um arquivo no archive
struct archive {
   unsigned char name[1024];
   int udi;
   off_t ogSize;              // tamanho original
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

// Cria e inicializa uma estrutura de diretório vazia
struct directory *create_directory();

// Lê o diretório de arquivos de um arquivo .vc
struct directory *read_directory(FILE *fp);

// Cria e preenche os metadados de um novo arquivo
struct archive *create_arch(char *name, int udi);

// Adiciona um novo arquivo ao diretório
int add_arch(struct directory *dir, struct archive *arch);

// Libera memória de um diretório
void destroy_directory(struct directory *dir);

// Calcula os offsets de cada arquivo dentro do .vc
void calc_offset(struct directory *dir);

// Retorna o maior tamanho de arquivo (útil para alocar buffer)
unsigned long buffer_size(struct directory *dir);

// Insere os dados dos arquivos no arquivo .vc
void insert_arch(FILE *fp, struct directory *dir, unsigned long buffer);

// Escreve os metadados no final do arquivo .vc
void write_directory(FILE *fp, struct directory *dir);

void print_directory(struct directory *dir);


#endif // __TADARCH__

