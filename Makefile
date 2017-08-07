#Makefile previsto per il progetto moka
#writted by Riccardo Fontanini
OBJECTS=

CC=gcc

CFLAGS= -g -Wall
FONTAUTILO= build/linux/fontautil.o
NAMEFILE= mocamain.c
PROGRAM_NAME=moca

LIBS = $(FONTAUTILO)

fontautil:
	$(CC) $(CFLAGS) -o $(FONTAUTILO) -c libs/fontautil.c

$(PROGRAM_NAME): fontautil
	@mkdir -p build/linux
	$(CC) $(CFLAGS) $(NAMEFILE) -o build/linux/$(PROGRAM_NAME) $(OBJECTS) $(LIBS)
	@echo " "
	@echo "Compilazione completata! il file si trova nella cartella build/linux!"
	@echo " "