CC = gcc 
CFLAGS = `pkg-config --cflags gtk+-3.0` -Wall -Wextra -std=c99 -o main -g `pkg-config --libs gtk+-3.0`

APP=./main

ODIR=build

DEPS = img_edit_gui.h encryption.h client.h ImageEditing/image_edit.h sharedBrowser/shared_browser.h

_OBJ = shared_browser.o img_edit_gui.o encryption.o client.o main_gui.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

#main_gui : $(OBJ) build/image_edit.o build
#	echo done

build: $(OBJ) $(ODIR)/image_edit.o
	$(CC) -o $(APP) $^ $(CFLAGS)

$(ODIR)/shared_browser.o: sharedBrowser/shared_browser.c $(DEPS)
	$(CC) -c $< $(CFLAGS) -o $@

$(ODIR)/image_edit.o: ImageEditing/image_edit.c $(DEPS)
	$(CC) -c $< $(CFLAGS) -o $@

$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c $< $(CFLAGS) -o $@ 

run: $(OBJ) build
	$(APP)

.PHONY: clean default

clean:
	rm -f $(ODIR)/*.o

default: clean
	rm -f $(APP)
