#ifndef __ARCHIVE_H__
#define __ARCHIVE_H__

#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "tadArch.h"

void insert_member(FILE *fp, struct directory *dir, unsigned char *bufferAux);

void same_member(FILE *fp, struct directory *dir, struct archive *arch, int index, unsigned char *bufferAux);

void compress_member(FILE *fp, struct directory *dir, unsigned long buffer, unsigned char *bufferAux, int index);

void same_compress(FILE *fp, struct directory *dir, struct archive *arch, int index, unsigned char *bufferAux, unsigned long buffer); 

void write_directory(FILE *fp, struct directory *dir);

struct directory *read_directory(FILE *fp);

void print_directory(struct directory *dir);

void extract_directory(struct directory *dir, FILE *fp, int i);

void move_member(struct directory *dir, FILE *fp, int from_index, int to_index, unsigned char *buffer);

void remove_member(int index, struct directory *dir, FILE *fp, unsigned char *buffer);

#endif
