#ifndef __ARCHIVE_H__
#define __ARCHIVE_H__

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "tadArch.h"

void insert_arch(FILE *fp, struct directory *dir, size_t buffer);

void write_directory(FILE *fp, struct directory *dir);

struct directory *read_directory(FILE *fp);

#endif
