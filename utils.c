#include "utils.h"

size_t buffer_size(struct directory *dir){

   size_t buffer = 0;

   for(int i = 0; i < dir->size; i++){
      if (dir->arch[i].discSize > buffer)
         buffer = dir->arch[i].discSize;
   }

   return buffer;
}


