#Makefile previsto per il progetto moka
#writted by Riccardo Fontanini
OBJECTS=

CC=gcc

CFLAGS= -g -Wall
FONTAUTILO= build/linux/fontautil.o
FONTACOMMUNICATIONO= build/linux/fontacommunication.o
FONTAHTTP = build/linux/fontahttp.o
FONTABOTCONNECTOR = build/linux/fontabotconnector.o
NAMEFILE= mocamain.c
PROGRAM_NAME=moca

LIBS = $(FONTAUTILO) $(FONTABOTCONNECTOR) $(FONTAHTTP) -lpthread -lcurl -ljson -std=gnu99

fontabotconnector:
	$(CC) $(CFLAGS) -o $(FONTABOTCONNECTOR) -c libs/fontabotconnector.c -ljson -std=gnu99

fontahttp:
	$(CC) $(CFLAGS) -o $(FONTAHTTP) -c libs/fontahttp.c -lcurl -lpthread

fontacommunication:
	$(CC) $(CFLAGS) -o $(FONTACOMMUNICATIONO) -c libs/fontacommunication.c

fontautil:
	$(CC) $(CFLAGS) -o $(FONTAUTILO) -c libs/fontautil.c -lpthread

$(PROGRAM_NAME): fontautil fontahttp fontabotconnector
	@mkdir -p build/linux
	$(CC) $(CFLAGS) $(NAMEFILE) -o build/linux/$(PROGRAM_NAME) $(OBJECTS) $(LIBS)
	@echo " "
	@echo "Compilazione completata! il file si trova nella cartella build/linux!"
	@echo " "