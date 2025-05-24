#include "archive.h"  
#include "tadArch.h" 
#include "lz/lz.h"     
#include <unistd.h>


void insert_member(FILE *fp, struct directory *dir, unsigned long buffer) {

   unsigned char *bufferAux = malloc(buffer);
   if (!bufferAux) {
      perror("Erro ao alocar buffer");
      return;
   }

   unsigned long move_size = sizeof(struct archive);

   for (int i = dir->size - 2; i >= 0; i--) {
      move_data(fp, dir->arch[i].offset, dir->arch[i].offset + move_size , bufferAux, dir->arch[i].discSize);
   }

   FILE *current_file = fopen((char *)dir->arch[dir->size - 1].name, "rb");
   if (!current_file) {
      perror("Erro ao abrir arquivo de origem");
      free(bufferAux);
      return;
   }

   calc_offset(dir);

   fread(bufferAux, 1, dir->arch[dir->size - 1].discSize, current_file);
   fseek(fp, dir->arch[dir->size - 1].offset, SEEK_SET);
   fwrite(bufferAux, 1, dir->arch[dir->size - 1].discSize, fp);

   ftruncate(fileno(fp), dir->arch[dir->size - 1].offset + dir->arch[dir->size - 1].discSize);

   fclose(current_file);

   free(bufferAux);

}

void same_member(FILE *fp, struct directory *dir, struct archive *arch, int index, unsigned long buffer){

   if ((off_t)buffer < arch->discSize)
      buffer = arch->discSize;

   unsigned char *bufferAux = malloc(buffer);
   if (!bufferAux) {
      perror("Erro ao alocar buffer");
      return;
   }

   FILE *current_file = fopen((char *)arch->name, "rb");
   if (!current_file) {
      perror("Erro ao abrir arquivo para leitura");
      free(bufferAux);
      return;
   }


   if(arch->discSize < dir->arch[index].discSize){

      unsigned long read_size = fread(bufferAux, 1, arch->discSize, current_file);
      if ((off_t)read_size != arch->discSize) {
         fprintf(stderr, "Erro na leitura do arquivo %s\n", arch->name);
         fclose(current_file);
         free(bufferAux);
         return;
      }

      unsigned long rest_size = dir->arch[index].discSize - arch->discSize;

      fseek(fp, dir->arch[index].offset, SEEK_SET);
      fwrite(bufferAux, 1, arch->discSize, fp);

      for (long unsigned int i = index + 1; i < dir->size; i++) {
         move_data(fp, dir->arch[i].offset, dir->arch[i].offset - rest_size, bufferAux, dir->arch[i].discSize);
         dir->arch[i].offset -= rest_size;
      }

      memcpy(&dir->arch[index], arch, sizeof(struct archive));

      int fd = fileno(fp);
      ftruncate(fd, dir->arch[dir->size - 1].offset + dir->arch[dir->size - 1].discSize);

   } else if(arch->discSize > dir->arch[index].discSize){

      unsigned long move_size = arch->discSize - dir->arch[index].discSize;

      for (int i = dir->size - 1; i > index; i--) {
         move_data(fp, dir->arch[i].offset, dir->arch[i].offset + move_size, bufferAux, dir->arch[i].discSize);
         dir->arch[i].offset += move_size;
      }

      unsigned long read_size = fread(bufferAux, 1, arch->discSize, current_file);
      if ((off_t)read_size != arch->discSize) {
         fprintf(stderr, "Erro na leitura do arquivo %s\n", arch->name);
         fclose(current_file);
         free(bufferAux);
         return;
      }

      fseek(fp, dir->arch[index].offset, SEEK_SET);
      fwrite(bufferAux, 1, arch->discSize, fp);
      printf("writing at offset %lu\n", dir->arch[index].offset);

      printf("first 5 bytes: %02x %02x %02x %02x %02x\n", bufferAux[0], bufferAux[1], bufferAux[2], bufferAux[3], bufferAux[4]);

      memcpy(&dir->arch[index], arch, sizeof(struct archive));


   } else{

      unsigned long read_size = fread(bufferAux, 1, arch->discSize, current_file);
      if ((off_t)read_size != arch->discSize) {
         fprintf(stderr, "Erro na leitura do arquivo %s\n", arch->name);
         fclose(current_file);
         free(bufferAux);
         return;
      }


      fseek(fp, dir->arch[index].offset, SEEK_SET);
      fwrite(bufferAux, 1, arch->discSize, fp);
      memcpy(&dir->arch[index], arch, sizeof(struct archive));
   }


   ftruncate(fileno(fp), dir->arch[dir->size - 1].offset + dir->arch[dir->size - 1].discSize);

   fclose(current_file);
   free(bufferAux);
}

void compress_member(struct directory *dir, unsigned long buffer, int index){

   if (dir->size == 0) 
      return;

   unsigned char *bufferAux = malloc(buffer);
   unsigned char *bufferCompress = malloc(buffer);

   if (!bufferAux || !bufferCompress) {
      perror("Erro ao alocar buffers de compressão");
      free(bufferAux);
      free(bufferCompress);
      return;
   } 


   FILE *current_file = fopen((char *)dir->arch[index].name, "rb");
   if (!current_file) {
      perror("Erro ao abrir arquivo para compressão");
      free(bufferAux);
      free(bufferCompress);
      return;
   }

   unsigned long uncompress = fread(bufferAux, 1, dir->arch[index].oldSize, current_file);

   fclose(current_file);

   unsigned long compress = LZ_Compress(bufferAux, bufferCompress, uncompress);

   if(compress > uncompress){
      printf("Tamanho comprimido é maior\n");
      free(bufferAux);
      free(bufferCompress);
      return; 
   }

   dir->arch[index].discSize = compress;

   dir->arch[index].isCompress = 1;

   free(bufferAux);
   free(bufferCompress);

}


void write_directory(FILE *fp, struct directory *dir){

   rewind(fp);
   fwrite(&dir->size, sizeof(unsigned long), 1, fp);
   fwrite(dir->arch, sizeof(struct archive), dir->size, fp);

}
struct directory *read_directory(FILE *fp) {


   rewind(fp);
   unsigned long size;

   fread(&size, sizeof(unsigned long), 1, fp);

   struct directory *dir = malloc(sizeof(struct directory));
   if (!dir) {
      perror("Erro ao alocar dir");
      return NULL;
   }
   dir->arch = malloc(sizeof(struct archive) * size);
   if (!dir->arch) {
      perror("Erro ao alocar dir>arch");
      free(dir);
      return NULL;
   }
   dir->size = size;

   fread(dir->arch, sizeof(struct archive), size, fp);

   return dir;
}


void print_directory(struct directory *dir) {

   if (!dir || !dir->arch) {
      printf("Diretório vazio ou inválido.\n");
      return;
   }

   for (unsigned long i = 0; i < dir->size; i++) {
      printf(
         "Nome: %s, Id: %d, Tamanho original: %lu, Tamanho armazenado: %lu, "
         "Última modificação: %s, Offset: %lu, Comprimido: ",
         dir->arch[i].name,
         dir->arch[i].udi,
         dir->arch[i].oldSize,
         dir->arch[i].discSize,
         ctime(&dir->arch[i].lastMod),
         dir->arch[i].offset
      );

      if (dir->arch[i].isCompress) {
         printf("Sim\n");
      } else {
         printf("Não\n");
      }
   }

}

void extract_directory(struct directory *dir, FILE *fp, int i){

   FILE *current_file = fopen((char *)dir->arch[i].name, "w+b");
   if (!current_file) {
      perror("Erro ao abrir arquivo de destino para escrita");
      return;
   }

   unsigned char *auxBuffer = malloc(dir->arch[i].oldSize);
   if (!auxBuffer) {
      perror("Erro ao alocar buffer para extração");
      fclose(current_file);
      return;
   }

   fseek(fp, dir->arch[i].offset, SEEK_SET);


   if(dir->arch[i].isCompress == 0){
      fread(auxBuffer, 1, dir->arch[i].discSize, fp);
      fwrite(auxBuffer, 1, dir->arch[i].discSize, current_file);
   }else{
      unsigned char *compress = malloc(dir->arch[i].discSize);
      if (!compress) {
         perror("Erro ao alocar buffer para dados comprimidos");
         free(auxBuffer);
         fclose(current_file);
         return;
      }


      fread(compress, 1, dir->arch[i].discSize, fp);

      LZ_Uncompress(compress, auxBuffer, dir->arch[i].oldSize);

      fwrite(auxBuffer, 1, dir->arch[i].oldSize, current_file);
      free(compress);

   }

   free(auxBuffer);
   fclose(current_file);

}

void move_member(struct directory *dir, FILE *fp, int from_index, int to_index, unsigned char *buffer) {
   if (!dir || !fp || !buffer || from_index == to_index) return;

   fseek(fp, 0, SEEK_END);
   unsigned long end = ftell(fp);
   if(to_index == 0){

      move_data(fp, dir->arch[from_index].offset, end, buffer, dir->arch[from_index].discSize);

      for (long i = from_index - 1; (long)i >= 0; i--) {
         if ((int)i != from_index) {
            move_data(fp, dir->arch[i].offset, dir->arch[i].offset + dir->arch[from_index].discSize, buffer, dir->arch[i].discSize);
         }
      }

      update_index(dir, from_index, to_index);
      calc_offset(dir);

      move_data(fp, end, dir->arch[0].offset, buffer, dir->arch[0].discSize);

   } else if (from_index > to_index) {
      for (unsigned long i = to_index + 1; i < dir->size; i++) {
         if ((int)i != from_index) {
            move_data(fp, dir->arch[i].offset, end, buffer, dir->arch[i].discSize);
            end += dir->arch[i].discSize;
         }
      }

      move_data(fp, dir->arch[from_index].offset,dir->arch[to_index].offset + dir->arch[to_index].discSize, buffer, dir->arch[from_index].discSize);

      update_index(dir, from_index, to_index);
      calc_offset(dir);

      for (int i = dir->size - 1; i > to_index + 1; i--) {
         end -= dir->arch[i].discSize;
         move_data(fp, end, dir->arch[i].offset, buffer, dir->arch[i].discSize);
      }

   } else {

      move_data(fp, dir->arch[from_index].offset, end, buffer, dir->arch[from_index].discSize);
      end += dir->arch[from_index].discSize;

      for(unsigned long i = to_index + 1; i < dir->size; i++){
         move_data(fp, dir->arch[i].offset, end, buffer, dir->arch[i].discSize);
         end += dir->arch[i].discSize;
      }

      unsigned long move_size = dir->arch[from_index].offset;
      for (int i = from_index + 1; i <= to_index; i++) {
         move_data(fp, dir->arch[i].offset, move_size, buffer, dir->arch[i].discSize);
         move_size += dir->arch[i].discSize;
      }

      update_index(dir, from_index, to_index);
      calc_offset(dir);

      for (int i = dir->size - 1; i >= to_index; i--) {
         end -= dir->arch[i].discSize;
         move_data(fp, end, dir->arch[i].offset, buffer, dir->arch[i].discSize);
      }
   }

   ftruncate(fileno(fp), end);
   fflush(fp);
   write_directory(fp, dir);
}


void remove_member(int index, struct directory *dir, FILE *fp, unsigned char *buffer){

   unsigned long removed_size = dir->arch[index].discSize;

   for (unsigned long int i = index + 1; i < dir->size; i++) {
      move_data(fp, dir->arch[i].offset, dir->arch[i].offset - removed_size, buffer, dir->arch[i].discSize);
      dir->arch[i].offset -= removed_size;
   }   

   update_index(dir, index, dir->size);

   write_directory(fp, dir);

   unsigned long new_dir_size = sizeof(unsigned long) + sizeof(struct archive) * dir->size;

   for (unsigned long int i = 0; i < dir->size; i++) {
      move_data(fp, dir->arch[i].offset, new_dir_size, buffer, dir->arch[i].discSize);
      new_dir_size += dir->arch[i].discSize;
   } 

   calc_offset(dir);
   fflush(fp);
   int fd = fileno(fp);
   if(index == 0 && dir->size == 0){
      ftruncate(fd, 0);
      return;
   }
   ftruncate(fd, dir->arch[dir->size - 1].offset + dir->arch[dir->size - 1].discSize); 
}

