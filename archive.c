#include "archive.h"
#include "tadArch.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

void insert_member(FILE *fp, struct directory *dir, unsigned long buffer) {

   unsigned char *bufferAux = malloc(buffer);

   fseek(fp, 0, SEEK_END);
   FILE *current_file = fopen(dir->arch[dir->size - 1].name, "rb");
   fread(bufferAux, 1, dir->arch[dir->size - 1].discSize, current_file);
   fwrite(bufferAux, 1, dir->arch[dir->size - 1].discSize, fp);

   unsigned long move_size = sizeof(struct directory) + sizeof(struct archive) * dir->size;

   for (int i = dir->size - 1; i >= 0; i--) {
      move_data(fp, dir->arch[i].offset, dir->arch[i].offset + move_size, dir->arch[i].discSize, bufferAux);
      dir->arch[i].offset += move_size;
   }

   fclose(current_file);
   free(bufferAux);

}

void same_member(FILE *fp, struct directory *dir, struct archive *arch, int index, unsigned long buffer){

   if (buffer < arch->discSize)
      buffer = arch->discSize;

   unsigned char *bufferAux = malloc(buffer);
   if (!bufferAux) {
      perror("Erro ao alocar buffer");
      return;
   }

   FILE *current_file = fopen(arch->name, "rb");
   if (!current_file) {
      perror("Erro ao abrir arquivo para leitura");
      free(bufferAux);
      return;
   }

   if(arch->discSize < dir->arch[index].discSize){

      fseek(fp, 0, SEEK_END);
      unsigned long end = ftell(fp);

      fseek(fp, dir->arch[index].offset, SEEK_SET);
      fread(bufferAux, 1, arch->discSize, current_file);
      fwrite(bufferAux, 1, arch->discSize, fp);

      unsigned long rest_size = dir->arch[index].discSize - arch->discSize;

      for (int i = index + 1; i < dir->size; i++) {
         move_data(fp, dir->arch[i].offset, dir->arch[i].offset - rest_size, dir->arch[i].discSize, bufferAux);
         dir->arch[i].offset -= rest_size;
      }
      memcpy(&dir->arch[index], arch, sizeof(struct archive));


      fseek(fp, 0, SEEK_END);
      int fd = fileno(fp);
      ftruncate(fd, ftell(fp) - rest_size);

   } else if (arch->discSize > dir->arch[index].discSize){

      unsigned long move_size = arch->discSize - dir->arch[index].discSize;

      for (int i = dir->size - 1; i > index; i--) {
         move_data(fp, dir->arch[i].offset, dir->arch[i].offset + move_size, dir->arch[i].discSize, bufferAux);
         dir->arch[i].offset += move_size;
      }

      fseek(fp, dir->arch[index].offset, SEEK_SET);
      fread(bufferAux, 1, arch->discSize, current_file);
      fwrite(bufferAux, 1, arch->discSize, fp);

      memcpy(&dir->arch[index], arch, sizeof(struct archive)); 

   } else{

      fseek(fp, dir->arch[index].offset, SEEK_SET);
      fread(bufferAux, 1, arch->discSize, current_file);
      fwrite(bufferAux, 1, arch->discSize, fp);
      memcpy(&dir->arch[index], arch, sizeof(struct archive));
   }

   fclose(current_file);
   free(bufferAux);
}

void compress_member(struct directory *dir, unsigned long buffer){

   unsigned char *bufferAux = malloc(buffer);
   unsigned char *bufferCompress = malloc(buffer);

   FILE *current_file = fopen(dir->arch[dir->size - 1].name, "rb");
   unsigned long uncompress = fread(bufferAux, 1, dir->arch[dir->size - 1].discSize, current_file);

   unsigned long compress = LZ_Compress(bufferAux, bufferCompress, dir->arch[dir->size- 1].discSize);

   if(compress > uncompress){
      return; 
   }

   dir->arch[dir->size - 1].discSize = compress;

   dir->arch[dir->size].isCompress = 1;

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
         (long)dir->arch[i].oldSize,
         (long)dir->arch[i].discSize,
         (long)dir->arch[i].lastMod.tv_sec,
         (long)dir->arch[i].offset
      );
   }
}

void extract_directory(struct directory *dir, FILE *fp, int i){

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
      unsigned char *auxBuffer = malloc(dir->arch[i].oldSize);
      fread(compress, 1, dir->arch[i].discSize, fp);

      LZ_Uncompress(compress, auxBuffer, dir->arch[i].discSize);

      fwrite(auxBuffer, 1, dir->arch[i].oldSize, current_file);
      free(compress);

      free(auxBuffer);
      fclose(current_file);
   }
}

void move_member(struct directory *dir, FILE *fp, int from_index, int to_index, unsigned char *buffer){

   fseek(fp, 0, SEEK_END);
   unsigned long end = ftell(fp);

   if(from_index > to_index){
      for(int i = to_index + 1; i < dir->size; i++){
         if(i != from_index){
            move_data(fp, dir->arch[i].offset, end, dir->arch[i].discSize, buffer);
            end += dir->arch[i].discSize;
         }
      }
      move_data(fp, dir->arch[to_index].offset, dir->arch[from_index].offset + dir->arch[from_index].discSize, dir->arch[to_index].discSize, buffer);
      update_index(dir, from_index, to_index); 
      calc_offset(dir);
      for(int i = dir->size - 1; i > to_index + 1; i--){
         end -= dir->arch[i].discSize;         
         move_data(fp, end, dir->arch[i-1].offset, dir->arch[i].discSize, buffer);
      }
      fseek(fp, 0, SEEK_END);
      unsigned long end = ftell(fp);
      int fd = fileno(fp);
      ftruncate(fd, end);
      write_directory(fp, dir);
   }else{
      for (int i = from_index + 1; i <= to_index; i++) {
         move_data(fp, dir->arch[i].offset, end, dir->arch[i].discSize, buffer);
         end += dir->arch[i].discSize;
      }

      move_data(fp, dir->arch[from_index].offset, dir->arch[to_index].offset, dir->arch[from_index].discSize, buffer);

      update_index(dir, from_index, to_index);
      calc_offset(dir);

      for (int i = to_index; i > from_index; i--) {
         end -= dir->arch[i].discSize;         
         move_data(fp, end, dir->arch[i].offset, dir->arch[i].discSize, buffer);
      }
      fseek(fp, 0, SEEK_END);
      unsigned long end = ftell(fp);
      int fd = fileno(fp);
      ftruncate(fd, end);
      write_directory(fp, dir);
   }

}

void remove_member(int index, struct directory *dir, FILE *fp, unsigned char *buffer){

   unsigned long removed_size = dir->arch[index].discSize;

   for (int i = index + 1; i < dir->size; i++) {
      move_data(fp, dir->arch[i].offset, dir->arch[i].offset - removed_size, dir->arch[i].discSize, buffer);
      dir->arch[i].offset -= removed_size;
   }   

   update_index(dir, index, dir->size);

   write_directory(fp, dir);

   unsigned long new_dir_size = sizeof(size_t) + sizeof(struct archive) * dir->size;

   for (int i = 0; i < dir->size; i++) {
      move_data(fp, dir->arch[i].offset, new_dir_size, dir->arch[i].discSize, buffer);
      new_dir_size += dir->arch[i].discSize;
   } 

   calc_offset(dir);

   size_t final_data_end = dir->arch[dir->size - 1].offset + dir->arch[dir->size - 1].discSize;
   size_t final_dir_size = sizeof(struct archive) * dir->size + sizeof(size_t);
   ftruncate(fileno(fp), final_data_end + final_dir_size);
}

