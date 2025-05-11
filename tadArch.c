#include "tadArch.h"

struct directory *create_directory(){
   struct directory *dir = malloc(sizeof(struct directory));
   if (!dir) {
      perror("Erro ao alocar struct directory");
      return NULL;
   }

   dir->arch = malloc(sizeof(struct archive) * 1);
   if (!dir->arch) {
      perror("Erro ao alocar dir->arch");
      free(dir);
      return NULL;
   }

   dir->size = 1;
   return dir;
}

int add_arch(struct directory *dir, struct archive *arch){

   for (unsigned long i = 0; i < dir->size; ++i) {
      if (strncmp((char *)dir->arch[i].name, (char *)arch->name, 1024) == 0) {
         return i;
      }
   }

   int new_capacity = dir->size + 2;
   struct archive *new_arch = realloc(dir->arch, sizeof(struct archive) * new_capacity);
   if (!new_arch) {
      perror("Erro ao alocar arch");
      return -2;
   }
   dir->arch = new_arch;
   dir->size = new_capacity;

   dir->arch[dir->size - 1] = *arch;
   return -1;

}


struct archive *create_arch(char *name, int i){

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
   arch->udi = i;
   arch->lastMod = st.st_mtim;
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
      if (dir->arch[i].discSize > (off_t )buffer)
         buffer = dir->arch[i].discSize;
   }

   return buffer;
}


void move_data(FILE *fp, unsigned long from_offset, size_t to_offset, size_t size, unsigned char *buffer) {
   if (size == 0 || from_offset == to_offset) 
      return;

   if (!buffer) {
      fprintf(stderr, "Erro: buffer inválido.\n");
      return;
   }

   if (to_offset > from_offset) {
      unsigned long remaining = size;
      while (remaining > 0) {
         unsigned long chunk;
         if (remaining > size) {
            chunk = size;
         } else {
            chunk = remaining;
         }
         remaining -= chunk;

         fseek(fp, from_offset + remaining, SEEK_SET);
         fread(buffer, 1, chunk, fp);

         fseek(fp, to_offset + remaining, SEEK_SET);
         fwrite(buffer, 1, chunk, fp);
      }
   }

   else {
      unsigned long processed = 0;
      while (processed < size) {
         unsigned long chunk;
         if (size - processed > size) {
            chunk = size;
         } else {
            chunk = size - processed;
         }

         fseek(fp, from_offset + processed, SEEK_SET);
         fread(buffer, 1, chunk, fp);

         fseek(fp, to_offset + processed, SEEK_SET);
         fwrite(buffer, 1, chunk, fp);

         processed += chunk;
      }
   }

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

