
# Nome do executável
TARGET = vinac

# Diretório de build (opcional)
BUILD_DIR = build

# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
LDFLAGS =

# Fontes e cabeçalhos
SRC = vinac.c tadArch.c archive.c utils.c lz.c
OBJ = $(SRC:.c=.o)

# Regra principal
all: $(TARGET)

# Como gerar o executável
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

# Regras para objetos individuais (opcional com dependência de headers)
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Limpeza dos arquivos gerados
clean:
	rm -f *.o $(TARGET)

# Executar o programa (exemplo)
run: $(TARGET)
	./$(TARGET)

# Força recompilação total
rebuild: clean all
