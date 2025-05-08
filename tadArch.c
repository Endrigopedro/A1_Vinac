#include "tadArch.h"

struct directory *create_directory(){
   struct directory *dir = malloc(sizeof(struct directory));

   dir->arch = malloc(sizeof(struct archive)*2);

   dir->size = 0;
   dir->capacity = 2;

   return dir;
}


int add_arch(struct directory *dir, struct archive *arch){

   for (size_t i = 0; i < dir->size; ++i) {
      if (strcmp(dir->arch[i].name, arch->name) == 0) {
         dir->arch[i] = *arch;
      }
   }

   if (dir->size >= dir->capacity) {
      int new_capacity = dir->capacity * 2;
      struct archive *new_arch = realloc(dir->arch, sizeof(struct archive) * new_capacity);
      if (!new_arch) 
         return -1;
      dir->arch = new_arch;
      dir->capacity = new_capacity;
   }
   dir->arch[dir->size++] = *arch;
   return 0;

}


struct archive *create_arch(char *name, int i){

   struct archive *arch = malloc(sizeof(struct archive));

   struct stat st;

   stat(name, &st);

   arch->discSize = st.st_size;
   arch->ogSize = st.st_size;
   arch->udi = i;
   arch->lastMod = st.st_mtim;
   strncpy(arch->name, name, sizeof(arch->name)); 

   return arch;
}

void calc_offset(struct directory *dir){

   size_t dir_size = sizeof(struct archive)*dir->size + sizeof(size_t);

   size_t current_offset = dir_size;
   for(int i = 0; i < dir->size; i++) {
      dir->arch[i].offset = current_offset;
      current_offset += dir->arch[i].discSize;
   }

}

