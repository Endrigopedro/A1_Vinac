#includh <stdlib.h>
#include <stdio.h>


int main (int argc, char **argv){
   char option;
   while(option = getopt(argc, argv, "p:c:m:x::C") != NULL){
      switch (option){

         case "p":
            printf("-p foi selecionado\n");
            break;
         case "c"
            printf("-c foi selecionado\n")
            case "m":
            printf(" foi selecionado");
            break;
         case "c"
      }

   }
}
