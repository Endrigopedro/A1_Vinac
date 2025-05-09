#include "archive.h"
#include "tadArch.h"
#include <stdio.h>
#include <stdlib.h>

void insert_arch(FILE *fp, struct directory *dir, unsigned long buffer) {

   char *bufferAux = malloc(buffer);

   fseek(fp, 0, SEEK_SET);

   for(int i = 0; i < dir->size; i++){

      FILE *current_file = fopen(dir->arch[i].name, "rb");

      fread(bufferAux, dir->arch[i].discSize, 1, current_file);
      fclose(current_file);

      fseek(fp, dir->arch[i].offset, SEEK_SET);

      fwrite(bufferAux, dir->arch[i].discSize, 1, fp);

   }

   free(bufferAux);
}

void write_directory(FILE *fp, struct directory *dir){

   fseek(fp, 0, SEEK_SET);

   for(int i = 0; i < dir->size; i++){
      fwrite(&dir->arch[i], sizeof(struct archive), 1, fp);
   }

   fwrite(&dir->size, sizeof(unsigned long), 1, fp);

}

struct directory *read_directory(FILE *fp) {
   unsigned long size;

   fseek(fp, -sizeof(unsigned long), SEEK_END);
   fread(&size, sizeof(unsigned long), 1, fp);

   struct directory *dir = malloc(sizeof(struct directory));
   if (!dir) return NULL;

   dir->arch = malloc(sizeof(struct archive) * size);
   if (!dir->arch) {
      free(dir);
      return NULL;
   }
   dir->size = size;

   rewind(fp);
   fread(dir->arch, sizeof(struct archive), size, fp);

   return dir;
}


void print_directory(struct directory *dir) {
   for (unsigned long i = 0; i < dir->size; i++) {
      printf(
         "Nome: %s, Id: %d, Tamanho original: %ld, Tamanho: %ld, Última modificação: %ld, Offset: %ld\n",
         dir->arch[i].name,
         dir->arch[i].udi,
         (long)dir->arch[i].ogSize,
         (long)dir->arch[i].discSize,
         (long)dir->arch[i].lastMod.tv_sec,
         (long)dir->arch[i].offset
      );
   }
}

void extract_directory(struct directory *dir, FILE *fp){

   for(int i = 0; i < dir->size; i++){
      if(dir->arch[i].isCompress == 0){
         FILE *current_file = fopen(dir->arch[i].name, "w+b");
         fseek(fp, dir->arch[i].offset, SEEK_SET);
         unsigned char *auxBuffer = malloc(dir->arch[i].discSize);
         fread(auxBuffer, 1, dir->arch[i].discSize, fp);
         fwrite(auxBuffer, 1, dir->arch[i].discSize, current_file);
         free(auxBuffer);
         fclose(current_file);
      }else{
         unsigned char *compress = malloc(dir->arch[i].discSize);

         FILE *current_file = fopen(dir->arch[i].name, "w+b");
         fseek(fp, dir->arch[i].offset, SEEK_SET);
         unsigned char *auxBuffer = malloc(dir->arch[i].ogSize);
         fread(compress, 1, dir->arch[i].discSize, fp);

         LZ_Uncompress(compress, auxBuffer, dir->arch[i].discSize);

         fwrite(auxBuffer, 1, dir->arch[i].ogSize, current_file);
         free(compress);

         free(auxBuffer);
         fclose(current_file);

      }
   }

}


void extrect_member(struct directory *dir, FILE *fp, unsigned char *name){


   for(int i = 0; i < dir->size; i++){
      if(strcmp(name, dir->arch[i].name) == 0){
         if(dir->arch[i].isCompress == 0){
            FILE *current_file = fopen(dir->arch[i].name, "w+b");
            fseek(fp, dir->arch[i].offset, SEEK_SET);
            unsigned char *auxBuffer = malloc(dir->arch[i].discSize);
            fread(auxBuffer, 1, dir->arch[i].discSize, fp);
            fwrite(auxBuffer, 1, dir->arch[i].discSize, current_file);
            free(auxBuffer);
            fclose(current_file);
         }
         else{
            unsigned char *compress = malloc(dir->arch[i].discSize);

            FILE *current_file = fopen(dir->arch[i].name, "w+b");
            fseek(fp, dir->arch[i].offset, SEEK_SET);
            unsigned char *auxBuffer = malloc(dir->arch[i].ogSize);
            fread(compress, 1, dir->arch[i].discSize, fp);

            LZ_Uncompress(compress, auxBuffer, dir->arch[i].discSize);

            fwrite(auxBuffer, 1, dir->arch[i].ogSize, current_file);
            free(compress);

            free(auxBuffer);
            fclose(current_file);
         }
      }
   }
}

void move_member(struct directory *dir, FILE *fp, unsigned char *name){


}


