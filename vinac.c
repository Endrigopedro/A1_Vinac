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
            }
            calc_offset(dir);
            buffer = buffer_size(dir);
            insert_arch(fp, dir, buffer);
            write_directory(fp, dir);
            break;
         case 'i':
            break;
         case 'm':
            printf("Movimentando membro %s para [nova posição]\n", optarg);
            if(argv[4] == "NULL"){               
               move_member(dir, fp, argv[3], (sizeof(struct directory)*dir->size));
            }else{
            }

            break;
         case 'x':
            printf("Extração dos seguintes membros\n");
            if(argc = 3)
               extract_directory(dir, fp);
            else{
               for(int i = 0; i < argc + dir->size; i++)
                  extrect_member(dir, fp, argv[i+3]);
            }
            break;
         case 'r':
            printf("Remoção dos seguintes membros\n"); //colocar os outros mebmros quando eu descobrir como
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
