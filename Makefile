#OBSERVE=true
#ANSI=true
ARCH=pentium4

TARGETS = hostile
CC=gcc

TOP = .
OBJS = main.o camera.o vector.o texture.o octree.o octreedraw.o  ppm.o md2.o \
skybox.o aistates.o aievents.o aiutil.o aiflock.o airules.o player.o \
playermove.o playerevents.o projectile.o collision.o terrain.o frustum.o \
fogplane.o font.o misc.o 


LLDLIBS = -lglut -lGLU -lGL -lXmu -lXext -lX11 -lm
#LDDIR = -L/usr/X11R6/lib -L/home/jon/Tarballs/Mesa-6.4/lib
LDDIR = -L/usr/X11R6/lib 

CFLAGS = -g -O2 -Wall -ffast-math -DGL_GLEXT_PROTOTYPES

ifdef ARCH
  CFLAGS += -march=$(ARCH)
endif

ifeq ($(NOFRUST), true)
  CFLAGS += -DNOFRUST
endif

ifeq ($(OBSERVE), true)
  CFLAGS += -DOBSERVE
endif

ifeq ($(ANSI), true)
  CFLAGS += -ansi -pedantic -D_GNU_SOURCE
endif

targets : $(TARGETS)
default: targets

all: default

.c.o: 
	$(CC) $(CFLAGS) -c -I/usr/include $<

$(TARGETS): $(OBJS) Makefile 
	$(CC) $(OBJS) $(LDDIR) $(LLDLIBS) -o $@

clean:  
	-rm -f *.o $(TARGETS) 
