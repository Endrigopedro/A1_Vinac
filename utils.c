#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>

void rearrange(off_t st_size){

   FILE *arch = fopen("name.txt", "a+");

}

int main(){

   FILE *arch = fopen("teste.txt", "r+b");

   int pos;
   scanf("digite a posição que vc quer inserir: %d", &pos);

   struct stat st;
   stat("teste.txt", &st);

   fseek(arch, 0, SEEK_END);

   long tam = ftell(arch);

   long tam_fim = tam - pos;
   char buffer[tam_fim];

   fseek(arch, pos, SEEK_SET);

   fread(buffer, sizeof(char), tam_fim, arch);

   fseek(arch, 0, SEEK_END);

   fwrite(buffer, sizeof(char), tam_fim, arch);

   fseek(arch, pos, SEEK_SET);

   //buffer recebe o conteudo que eu quero escrever
   //escrevo o conteudo
   //a partir da posição leio o q esta no final e escrevo dps do que eu ja tinha escrito



}
