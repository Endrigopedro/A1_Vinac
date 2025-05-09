#include "utils.h"
#include "tadArch.h"

unsigned long buffer_size(struct directory *dir){

   unsigned long buffer = 0;

   for(int i = 0; i < dir->size; i++){
      if (dir->arch[i].discSize > buffer)
         buffer = dir->arch[i].discSize;
   }

   return buffer;
}


int move_data(FILE *fp, size_t from_offset, size_t to_offset, size_t size, char *buffer) {
   if (size == 0 || from_offset == to_offset) return 0;

   if (!buffer) {
      fprintf(stderr, "Erro: buffer inválido.\n");
      return -1;
   }

   // Caso 1: mover para frente (precisa ler de trás para frente)
   if (to_offset > from_offset) {
      ssize_t remaining = size;
      while (remaining > 0) {
         size_t chunk;
         if (remaining > size) {
            chunk = size;  // Se o restante for maior que o tamanho do arquivo, usa o tamanho do arquivo
         } else {
            chunk = remaining;  // Caso contrário, usa o restante
         }
         remaining -= chunk;

         // Mover dados de trás para frente
         fseek(fp, from_offset + remaining, SEEK_SET);
         fread(buffer, 1, chunk, fp);

         fseek(fp, to_offset + remaining, SEEK_SET);
         fwrite(buffer, 1, chunk, fp);
      }
   }

   // Caso 2: mover para trás (pode ler normalmente)
   else {
      size_t processed = 0;
      while (processed < size) {
         size_t chunk;
         if (size - processed > size) {
            chunk = size;  // Se o restante for maior que o tamanho do arquivo, usa o tamanho do arquivo
         } else {
            chunk = size - processed;  // Caso contrário, usa o restante
         }

         fseek(fp, from_offset + processed, SEEK_SET);
         fread(buffer, 1, chunk, fp);

         fseek(fp, to_offset + processed, SEEK_SET);
         fwrite(buffer, 1, chunk, fp);

         processed += chunk;
      }
   }

   return 0;
}

void update_index(struct directory *dir, int from, int to) {
   if (from == to || from >= dir->size || to >= dir->size) return;

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

   for (int i = 0; i < dir->size; i++) {
      dir->arch[i].udi = i;
   }
}

