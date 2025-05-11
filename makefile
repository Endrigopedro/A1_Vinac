# Nome do executável
TARGET = vinac

# Compilador e flags
CC = gcc
CFLAGS = -Wall -Wextra -std=gnu99 -O3  # Ajuste para otimização no nível 3
LDFLAGS =

# Fontes e cabeçalhos
SRC = vinac.c tadArch.c archive.c lz/lz.c
OBJ = $(SRC:%.c=%.o)

# Regra principal
all: $(TARGET)

# Como gerar o executável
$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LDFLAGS)

# Regras para compilar arquivos .c em .o
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

