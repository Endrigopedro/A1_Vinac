#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "tadArch.h"
#include "utils.h"
#include "archive.h"

int main(int argc, char **argv){

   char opt;
   struct archive *arch = malloc(sizeof(struct archive));
   struct directory *dir;
   FILE *fp = fopen(argv[2], "r+b");
   unsigned long buffer;
   while ((opt = getopt(argc, argv, "p:i:m:x::r:c")) != -1) {
      switch (opt) {
         case 'p':
            printf("Inserção sem compressãso foi selecionado\n");
            if(!fp) {
               fp = fopen(argv[2], "wb+");
               *dir = (struct directory)*create_directory();
            }
            else {
               *dir = (struct directory)*read_directory(fp);
            }
            for(int i = dir->size; i < argc + dir->size; i++){
               arch = create_arch(argv[i+3], i);
               add_arch(dir, arch);
               buffer = buffer_size(dir);
               insert_arch(fp, dir, buffer);
            }
            break;

         case 'i':
            break;

         case 'm':
            printf("Movimentando membro %s para [nova posição]\n", optarg);

            int from_index = -1, to_index = -1;

            for (size_t i = 0; i < dir->size; ++i) {
               if (strcmp(dir->arch[i].name, argv[3]) == 0)
                  from_index = i;

               if (argc > 4 && strcmp(argv[4], "NULL") != 0 && strcmp(dir->arch[i].name, argv[4]) == 0)
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

            size_t size = buffer_size(dir);
            unsigned char *buffer = malloc(size);
            move_member(dir, fp, from_index, to_index, buffer);
            free(buffer);

            break;

         case 'x':
            printf("Extração dos seguintes membros\n");
            if(argc = 3)
               for(int i = 0; i < dir->size; i++)
                  extract_directory(dir, fp, i);
            else{
               for(int i = 0; i < argc + dir->size; i++)
                  if(strcmp(argv[i+3], dir->arch[i].name) == 0)
                     extract_directory(dir, fp, i);
            }
            break;

         case 'r':
            printf("Remoção dos seguintes membros\n");
            read_directory(fp);
            size = buffer_size(dir);
            buffer = malloc(size);
            for (size_t i = 0; i < dir->size; ++i) 
               if (strcmp(dir->arch[i].name, argv[i + 3]) == 0)
                 remove_member(i, dir, p, unsigned char *buffer);

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
   return 0;
}
