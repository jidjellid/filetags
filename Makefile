CC=gcc
CFLAGS=-Wall -g -std=c11
MAIN=main
INIT=init
TAG=tag
SRC=src/
BUILD=build/

compile:
	$(CC) $(CFLAGS) $(SRC)$(MAIN).c -o $(BUILD)$(TAG) -lsqlite3

config: 
	$(CC) $(CFLAGS) $(SRC)$(INIT).c -o $(BUILD)$(INIT) -lsqlite3
	./build/$(INIT)

install: 
	make compile
	mkdir ~/.tags
	cp ./docs/tag.txt ~/.tags/tag.txt
	make config
	sudo cp build/tag /usr/bin/tag
	sudo cp docs/tag.1.gz  /usr/share/man/man1/tag.1.gz
	sudo chmod 444 /usr/share/man/man1/tag.1.gz
	
uninstall:
	make clean
	rm -rf ~/.tags
	sudo rm /usr/bin/tag
	sudo rm /usr/share/man/man1/tag.1.gz

require:
	sudo apt-get install sqlite3
	sudo apt-get install libsqlite3-dev

tests:
	$(CC) $(CFLAGS) test/test.c -o $(BUILD)test

clean:
	rm -rf *~ $(BUILD)*