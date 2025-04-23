#ifndef __TADARCH__
#define __TADARCH__

#include <unistd.h>
#include <sys/stat.h>

struct arch {
   char name[181]; //pergunrar pros prof o limite do nome
   uid_t udi;
   off_t ogSize;
   blkcnt_t discSize;
   struct timespec lastMod;

};

struct arch *insert_arch();

struct arch *remove_arch();

void move_arch();

void extract_arch();

void print_arch();

#endif
