
FULL_OPTIMIZE = 1

ifdef FULL_OPTIMIZE
    CC = cc
    CXX = c++
    EXTRAFLAGS = -O3
else
    CC = cc
    CXX = c++
    EXTRAFLAGS = -g
endif

CFLAGS = -Wall $(EXTRAFLAGS)
CXXFLAGS = -std=c++11 -stdlib=libc++ -Wall $(EXTRAFLAGS)
LDFLAGS = -lc++ $(EXTRAFLAGS)

OBJS = \
    aabb.o \
    aabbgeometry.o \
    abstracttonemapper.o \
    bsdfdiffuse.o \
    bsdfrefraction.o \
    bsdfspecular.o \
    bsdfpaint.o \
    bvhnode.o \
    camera.o \
    commandqueue.o \
    eduptscene.o \
    framebuffer.o \
    intersection.o \
    mesh.o \
    renderer.o \
    scene.o \
    scenesupport.o \
    scene2013.o \
    sceneobject.o \
    sphere.o \
    texture.o \
    tonemapper.o \
    wavefrontobj.o

TESTOBJ = main.o
MAINOBJ = main.o

MAINTARGET = r1h14fnl
TESTTARGET = r1h1401

# main
$(MAINTARGET) : $(MAINOBJ) $(OBJS)
	$(CC) -o $(MAINTARGET) $(MAINOBJ) $(OBJS) $(LDFLAGS)

# test
$(TESTTARGET) : $(TESTOBJ) $(OBJS)
	$(CC) -o $(TESTTARGET) $(TESTOBJ) $(OBJS) $(LDFLAGS)

clean :
	rm -f $(OBJS) $(TESTOBJ) $(MAINOBJ) $(MAINTARGET) $(TESTTARGET)
