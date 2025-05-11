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
   while ((opt = getopt(argc, argv, "p:i:m:x::r:c")) != -1) {
      switch (opt) {
         case 'p':
            printf("Inserção sem compressãso foi selecionado\n");
            if(!fp) {
               fp = fopen(argv[optind - 1], "wb+");
               dir = create_directory();
            }
            else {
               dir = read_directory(fp);
            }
            for(int i = optind; i < argc; i++){
               struct archive *arch = create_arch(argv[i], dir->size); 
               int j = add_arch(dir, arch);
               calc_offset(dir);
               if(j == -1){
                  buffer = buffer_size(dir);
                  insert_member(fp, dir, buffer);
               }else {
                  buffer = buffer_size(dir);
                  same_member(fp, dir, arch, j, buffer);
               }
            }
            calc_offset(dir);
            write_directory(fp, dir);
            break;

         case 'i':
            printf("Inserção com compressãso foi selecionado\n");
            if(!fp) {
               fp = fopen(argv[optind - 1], "wb+");
               dir = create_directory();
            }
            else {
               dir = read_directory(fp);
            }
            for(int i = optind; i < argc; i++){
               struct archive *arch = create_arch(argv[i], dir->size); 
               int j = add_arch(dir, arch);
               calc_offset(dir);
               if(j == -1){
                  buffer = buffer_size(dir);
                  compress_member(dir, buffer);
                  insert_member(fp, dir, buffer);
               }else {
                  buffer = buffer_size(dir);
                  compress_member(dir, buffer);
                  same_member(fp, dir, arch, j, buffer);
               }
            }
            calc_offset(dir);
            write_directory(fp, dir);
            break;

         case 'm':
            printf("Movimentando membro %s para nova posição\n", optarg);


            if (!fp) {
               fprintf(stderr, "Arquivo não encontrado.\n");
               break;
            }

            dir = read_directory(fp);

            if (optind + 1 >= argc) {
               fprintf(stderr, "Uso: -m <membro> <destino>\n");
               break;
            }


            int from_index = -1, to_index = -1;


            for (size_t i = 0; i < dir->size; ++i) {
               if (strncmp((char *)dir->arch[i].name, argv[optind], 1024) == 0)
                  from_index = i;

               if (argc > 4 && strcmp(argv[optind], "NULL") != 0 && strncmp((char *)dir->arch[i].name, argv[optind], 1024) == 0)
                  to_index = i;
            }

            if (from_index == -1) {
               fprintf(stderr, "Erro: membro %s não encontrado\n", argv[3]);
               break;
            }

            if (to_index == -1) {
               fprintf(stderr, "Erro: destino %s não encontrado\n", argv[4]);
               break;
            }

            buffer = buffer_size(dir);
            buf = malloc(buffer);
            move_member(dir, fp, from_index, to_index, buf);
            free(buf);

            break;

         case 'x':
            printf("Extração\n");
            dir = read_directory(fp);
            if(argc == optind)
               for(long unsigned int i = 0; i < dir->size; i++){
                  extract_directory(dir, fp, i);
               }
            else{
               printf("Extração dos seguintes membros\n");
               for (int i = optind; i < argc; i++) {
                  for (long unsigned int j = 0; j < dir->size; j++) {

                     if (strncmp(argv[i],(char *)dir->arch[j].name, 1024) == 0){
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
            perror("%Argumentos: -a [Valor Op.] -b [Valor Ob.]\n"); //pensar numa mensagem legal de erro
            return 1;
      }
   }
   if (fp) 
      fclose(fp);
   if (dir)
      destroy_directory(dir);

   fclose(fp);

   return 0;
}
