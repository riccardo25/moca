#Makefile previsto per il progetto moka
#writted by Riccardo Fontanini
OBJECTS=

CC=gcc

CFLAGS= -g -Wall
FONTAUTILO= build/linux/fontautil.o
FONTACOMMUNICATIONO= build/linux/fontacommunication.o
NAMEFILE= mocamain.c
PROGRAM_NAME=moca

LIBS = $(FONTAUTILO) $(FONTACOMUNICATIONO) -lpthread 


fontacommunication:
	$(CC) $(CFLAGS) -o $(FONTACOMMUNICATIONO) -c libs/fontacommunication.c

fontautil:
	$(CC) $(CFLAGS) -o $(FONTAUTILO) -c libs/fontautil.c -lpthread

$(PROGRAM_NAME): fontautil fontacommunication
	@mkdir -p build/linux
	$(CC) $(CFLAGS) $(NAMEFILE) -o build/linux/$(PROGRAM_NAME) $(OBJECTS) $(LIBS)
	@echo " "
	@echo "Compilazione completata! il file si trova nella cartella build/linux!"
	@echo " "