#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "tadArch.h"
#include "archive.h" 

int main(int argc, char **argv){

   char opt;
   struct directory *dir = NULL;
   FILE *fp = fopen(argv[2], "r+b");
   unsigned long buffer;
   unsigned char *buf;
   int has_members;
   while ((opt = getopt(argc, argv, "p:i:m:x::r:c")) != -1) {
      switch (opt) {
         case 'p':
            if(!fp) {
               fp = fopen(argv[optind - 1], "wb+");
               if (!fp) {
                  perror("Erro ao abrir o arquivo");
                  exit(1);
               }
               dir = create_directory();
            }
            else {
               dir = read_directory(fp);
            }
            for(int i = optind; i < argc; i++){

               struct archive *arch = create_arch(argv[i]); 
               int j = add_arch(dir, arch);
               if(j == -1){
                  buffer = buffer_size(dir);
                  unsigned char *bufferAux = malloc(buffer);
                  if (!bufferAux) {
                     perror("Erro ao alocar buffer");
                     exit(0);
                  }
                  insert_member(fp, dir, bufferAux);
               }else {
                  buffer = buffer_size(dir);
                  if ((off_t)buffer < arch->discSize)
                     buffer = arch->discSize;
                  unsigned char *bufferAux = malloc(buffer);
                  if (!bufferAux) {
                     perror("Erro ao alocar buffer");
                     exit(0);
                  }

                  same_member(fp, dir, arch, j, bufferAux);
               }
               free(arch);
            }
            calc_offset(dir);
            write_directory(fp, dir);
            break;

         case 'i':
            printf("Inserção com compressãso foi selecionado\n");
            for (int i = optind; i < argc; i++) {
               if (access(argv[i], F_OK) == -1) {
                  fprintf(stderr, "Erro: o arquivo '%s' não existe.\n", argv[i]);
                  exit(1);
               }
            }
            if(!fp) {
               fp = fopen(argv[optind - 1], "wb+");
               if (!fp) {
                  perror("Erro ao abrir o arquivo");
                  exit(1);
               }
               dir = create_directory();
            }
            else {
               dir = read_directory(fp);
            }
            for(int i = optind; i < argc; i++){

               struct archive *arch = create_arch(argv[i]); 
               int j = add_arch(dir, arch);
               if(j == -1){
                  buffer = buffer_size(dir);
                  unsigned char *bufferAux = malloc(buffer);
                  if (!bufferAux) {
                     perror("Erro ao alocar buffer");
                     exit(0);
                  }
                  compress_member(fp, dir, buffer, bufferAux, dir->size - 1);
               }else {
                  buffer = buffer_size(dir);
                  if ((off_t)buffer < arch->discSize)
                     buffer = arch->discSize;
                  unsigned char *bufferAux = malloc(buffer);
                  if (!bufferAux) {
                     perror("Erro ao alocar buffer");
                     exit(0);
                  }
                  same_compress(fp, dir, arch, j, bufferAux, buffer);
               }
               free(arch);
            }
            calc_offset(dir);
            write_directory(fp, dir);
            break;

         case 'm':
            if (!fp) {
               fprintf(stderr, "Arquivo não encontrado.\n");
               exit(1);
            }

            dir = read_directory(fp);

            if (optind >= argc) {
               fprintf(stderr, "Uso: -m <membro> <destino>\n");
               break;
            }


            int from_index = -1, to_index = 0;


            for (size_t i = 0; i < dir->size; ++i) {
               if (strncmp((char *)dir->arch[i].name, argv[optind], 1024) == 0)
                  from_index = i;

               if (argc > 4 && strcmp(argv[optind], "NULL") != 0 && strncmp((char *)dir->arch[i].name, argv[optind + 1], 1024) == 0)
                  to_index = i;
            }

            if (from_index == -1) {
               fprintf(stderr, "Erro: membro %s não encontrado\n", argv[3]);
               break;
            }

            buffer = buffer_size(dir);
            buf = malloc(buffer);
            move_member(dir, fp, from_index, to_index, buf);
            free(buf);

            break; 
         case 'x':

            if(optind < argc - 1)
               has_members = 1;
            else
               has_members = 0;

            dir = read_directory(fp);
            calc_offset(dir);
            if (!has_members) {
               printf("Extração\n");
               for (unsigned long i = 0; i < dir->size; i++) {
                  extract_directory(dir, fp, i);
               }
            } else {
               printf("Extração dos seguintes membros\n");
               for (int i = optind; i < argc; i++) {
                  for (unsigned long j = 0; j < dir->size; j++) {
                     if (strncmp(argv[i], (char *)dir->arch[j].name, 1024) == 0) {
                        printf("- %s\n", dir->arch[j].name);
                        extract_directory(dir, fp, j);
                     }
                  }
               }
            }
            break;
         case 'r':

            printf("Remoção dos seguintes membros\n");
            dir = read_directory(fp);
            buffer = buffer_size(dir);
            buf = malloc(buffer);
            for (int i = optind; i < argc; i++) {
               for (long unsigned int j = 0; j < dir->size; j++) {
                  if (strncmp(argv[i],(char *)dir->arch[j].name, 1024) == 0) {
                     printf("%s\n", dir->arch[j].name);
                     remove_member(j, dir, fp, buf);
                     break;
                  }
               }
            }
            free(buf);
            break; 

         case 'c':

            printf("Archive possui os seguintes conteúdos\n");
            if(fp){   
               dir = read_directory(fp);
               print_directory(dir);
            }else {
               printf("Diretorio vazio\n");
            }

            break;

         default:
            perror("Escolha uma opção valida\n");
            return 1;
      }
   }
   if (fp) 
      fclose(fp);
   if (dir)
      destroy_directory(dir);

   return 0;
}
