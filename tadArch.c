#include "tadArch.h"

struct directory *create_directory(){
   struct directory *dir = malloc(sizeof(struct directory));
   if (!dir) {
      perror("Erro ao alocar struct directory");
      return NULL;
   }

   dir->arch = NULL;
   dir->size = 0;
   return dir;
}

int add_arch(struct directory *dir, struct archive *arch){

   for (unsigned long i = 0; i < dir->size; ++i) {
      if (strncmp((char *)dir->arch[i].name, (char *)arch->name, 1024) == 0) {
         return i;
      }
   }

   int new_size = dir->size + 1;
   struct archive *new_arch = realloc(dir->arch, sizeof(struct archive) * new_size);
   if (!new_arch) {
      perror("Erro ao alocar arch");
      return -2;
   }
   dir->arch = new_arch;
   dir->arch[dir->size] = *arch; // novo elemento na posição atual
   dir->size = new_size; 
   return -1;

}


struct archive *create_arch(char *name){

   struct archive *arch = malloc(sizeof(struct archive));
   if (!arch) {
      perror("Erro ao alocar arch");
      return NULL;
   }


   struct stat st;
   if (stat(name, &st) != 0) {
      perror("Erro ao obter informações do arquivo");
      free(arch);
      return NULL;
   }

   arch->discSize = st.st_size;
   arch->oldSize = st.st_size;
   arch->udi = st.st_uid;
   arch->lastMod = st.st_mtime;
   arch->isCompress = 0;
   strncpy((char *)arch->name, name, sizeof(arch->name) - 1);
   arch->name[sizeof(arch->name) - 1] = '\0';

   return arch;
}

void calc_offset(struct directory *dir){
   if(!dir || !dir->arch)
      return;

   unsigned long dir_size = sizeof(struct archive)*dir->size + sizeof(unsigned long);

   unsigned long current_offset = dir_size;
   for(long unsigned int i = 0; i < dir->size; i++) {
      dir->arch[i].offset = current_offset;
      current_offset += dir->arch[i].discSize;
   }

}


void destroy_directory(struct directory *dir) {
   if (!dir) 
      return;

   if (dir->arch) {
      free(dir->arch);
   }

   free(dir);
}

unsigned long buffer_size(struct directory *dir){

   unsigned long buffer = 0;

   for(long unsigned int i = 0; i < dir->size; i++){
      if (dir->arch[i].oldSize > (off_t )buffer)
         buffer = dir->arch[i].oldSize;
   }

   return buffer;
}


void move_data(FILE *fp, unsigned long from, unsigned long to, unsigned char *buffer, unsigned long size) {
   if (from == to || size == 0 || !buffer) return;

   fseek(fp, from, SEEK_SET);
   fread(buffer, 1, size, fp);
   fseek(fp, to, SEEK_SET);
   fwrite(buffer, 1, size, fp);
   fflush(fp);
}
void update_index(struct directory *dir, int from, int to) {
   if ((long unsigned int ) from >= dir->size) 
      return;

   if ((long unsigned int ) to >= dir->size) {
      for (long unsigned int i = from; i < dir->size - 1; i++) {
         dir->arch[i] = dir->arch[i + 1];
      }
      dir->size--;  
   } else if (from != to) {
      struct archive temp = dir->arch[from];

      if (from < to) {
         for (int i = from; i < to; i++) {
            dir->arch[i] = dir->arch[i + 1];
         }
      } else {
         for (int i = from; i > to; i--) {
            dir->arch[i] = dir->arch[i - 1];
         }
      }

      dir->arch[to] = temp;
   }

   for (long unsigned int i = 0; i < dir->size; i++) {
      dir->arch[i].udi = i;
   }
}

