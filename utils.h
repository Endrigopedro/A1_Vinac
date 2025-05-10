#ifndef __UTILS_H__
#define __UTILS_H__

#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "tadArch.h"
#include "archive.h"

unsigned long buffer_size(struct directory *dir);

int move_data(FILE *fp, size_t from_offset, size_t to_offset, size_t size, unsigned char *buffer);

void update_index(struct directory*dir, int from, int to);

#endif 

