CC = gcc
CFLAGS = -Wall -g
LDLIBS = -lm -ldl  # Añadido -ldl para dynamic linking
OBJS = main.o ABB.o TS.o parser.tab.o lex.yy.o list.o COMMANDS.o importhandler.o error.o
TARGET = INTERPRETE

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDLIBS)

# Reglas de compilación para cada objeto
main.o: main.c TS.h COMMONDEF.h COMMANDS.h
	$(CC) $(CFLAGS) -c $<

ABB.o: ABB.c ABB.h parser.tab.h COMMONDEF.h 
	$(CC) $(CFLAGS) -c $<

TS.o: TS.c TS.h ABB.h parser.tab.h COMMONDEF.h list.h COMMANDS.h importhandler.h
	$(CC) $(CFLAGS) -c $<

parser.tab.o: parser.tab.c COMMONDEF.h list.h error.h
	$(CC) $(CFLAGS) -c $<

lex.yy.o: lex.yy.c parser.tab.h TS.h COMMONDEF.h error.h list.h
	$(CC) $(CFLAGS) -c $<

importhandler.o: importhandler.c importhandler.h TS.h list.h error.h
	$(CC) $(CFLAGS) -c $<

error.o: error.c error.h
	$(CC) $(CFLAGS) -c $<

COMMANDS.o: COMMANDS.c COMMANDS.h TS.h list.h importhandler.h error.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: all clean
