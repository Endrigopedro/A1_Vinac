#include "archive.h"

void insert_arch(FILE *fp, struct directory *dir, size_t buffer) {

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

   fwrite(&dir->capacity, sizeof(size_t), 1, fp);

}

struct directory *read_directory(FILE *fp){

   size_t capacity;
   fseek(fp, -sizeof(size_t), SEEK_END);
   fread(&capacity, sizeof(size_t), 1, fp);

   struct directory *dir = malloc(sizeof(struct directory));
   struct archive *new_arch = malloc(sizeof(struct archive) * capacity);
   if (!new_arch) 
      return NULL;
   dir->arch = new_arch;
   dir->capacity = capacity;
   dir->size = 0;

   for (size_t i = 0; i < dir->capacity; ++i) {
      fread(dir->arch[i].name, sizeof(char), 1024, fp);
      fread(&dir->arch[i].udi, sizeof(int), 1, fp);
      fread(&dir->arch[i].ogSize, sizeof(off_t), 1, fp);
      fread(&dir->arch[i].discSize, sizeof(off_t), 1, fp);
      fread(&dir->arch[i].lastMod, sizeof(struct timespec), 1, fp);
      fread(&dir->arch[i].offset, sizeof(size_t), 1, fp);
      dir->size++;
   }

   return dir;

}


