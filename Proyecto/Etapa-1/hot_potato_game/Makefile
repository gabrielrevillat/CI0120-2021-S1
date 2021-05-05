APPNAME = hot_potato_game

CC = gcc
CFLAGS = -g -std=c11 -Wall -Wextra
LIBS = -lpthread

$(APPNAME): $(APPNAME).c
	$(CC) $(CFLAGS) $(APPNAME).c -o $(APPNAME) $(LIBS)

all: $(APPNAME)

.PHONY: gitignore
gitignore:
	echo $(APPNAME) > .gitignore

.PHONY: clean
clean:
	rm -f $(APPNAME)
