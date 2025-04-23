#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "tadArch.h"
#include "ultils.h"

int main(int argc, char **argv){

   char opt;
   while ((opt = getopt(argc, argv, "p:c:m:x::r:C")) != -1) {
      switch (opt) {
         case 'p':
            printf("Inserção sem compressãso foi selecionado\n");
            break;
         case 'c':
            printf("Inserção com compressãso foi selecionado\n");
            break;
         case 'm':
            printf("Movimentando membro %s para [nova posição]\n", optarg);
            break;
         case 'x':
            printf("Extração dos seguintes membros\n");
            break;
         case 'r':
            printf("Remoção dos seguintes membros\n"); //colocar os outros mebmros quando eu descobrir como
            break;
         case 'C':
            printf("Archive possui os seguintes conteúdos\n"); // same de cima
            break;
         default:
            perror("%Argumentos: -a [Valor Op.] -b [Valor Ob.]\n"); //pensar numa mensagem legal de erro
            return 1;
      }
   }
   return 0;
}
