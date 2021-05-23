APPNAME = $(shell basename $(shell pwd))

CXX = g++
CFLAGS = -g -std=gnu++11 -Wall -Wextra

$(APPNAME): controller/main.o controller/arguments.o controller/cache.o
	$(CXX) $(CFLAGS) $^ -o $(APPNAME)

%.o: %.c
	$(CXX) -c $(CFLAGS) $< -o $@

.PHONY: gitignore
gitignore:
	echo $(APPNAME) > .gitignore
	echo *.o >> .gitignore

.PHONY: clean
clean:
	rm -f $(APPNAME) controller/*.o