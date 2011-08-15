# flags I should look into...
# -DNDEBUG -pipe -march=i386

PYTHON_LIBS = C:\Python27\libs
PYTHON_INCS = C:\Python27\include


CC = gcc
CFLAGS = -W -Wall -ansi -O3 -mwindows \
		 -DGLYPH_TARGET=GLYPH_TARGET_ALLEGRO \
		 -L$(PYTHON_LIBS) -I$(PYTHON_INCS)
LDFLAGS = -lpython26 -lglyph-alleg -lalleg \
	      -lmingw32 -lscew -lexpat

EDITOR = editor
EDITOR_BIN = editor.exe
EDITOR_SOURCES = mapeditor.c editor_gui.c list_element.c primitives.c gfx.c map.c \
				 sprite.c tile.c
EDITOR_OBJECTS = $(EDITOR_SOURCES:%.c=%.o)

GUNPOWDER = gunpowder
GUNPOWDER_BIN = gunpowder.exe
GUNPOWDER_SOURCES = bullet.c gfx.c gunsmoke.c map.c sprite.c tile.c \
			highscore.c
GUNPOWDER_OBJECTS = $(GUNPOWDER_SOURCES:%.c=%.o)
PYC = ai.pyc

DEPEND_FILE = _depend

TAGS = etags *.c *.h


all: $(GUNPOWDER)

it: $(EDITOR) $(GUNPOWDER)

$(GUNPOWDER): $(GUNPOWDER_OBJECTS)
	$(CC) $(GUNPOWDER_OBJECTS) -o $(GUNPOWDER_BIN) \
	$(CFLAGS) $(LDFLAGS)

$(EDITOR): $(EDITOR_OBJECTS)
	$(CC) $(EDITOR_OBJECTS) -o $(EDITOR_BIN) $(CFLAGS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@


include _depend
.PHONY: clean depend

clean:
	rm -f $(GUNPOWDER_BIN) $(GUNPOWDER_OBJECTS) $(EDITOR_BIN) \
	      $(EDITOR_OBJECTS) $(PYC)
	$(TAGS)

depend:
	$(CC) -MM $(CFLAGS) ./*.c | sed "s/.*\.o[ :]*/$(subst /,\/,.)\/\0/g" > $(DEPEND_FILE)


