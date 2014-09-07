
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
    bsdftranslucent.o \
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
    scene2014.o \
    sceneobject.o \
    sphere.o \
    texture.o \
    tonemapper.o \
    wavefrontobj.o

TESTOBJ = main.o
MAINOBJ = main.o

#MAINEXE = ray&bow-
MAINEXE = raynbow-

MAINTARGET = raynbowdash
TESTTARGET = r30m

# main
$(MAINTARGET) : $(MAINOBJ) $(OBJS)
	$(CC) -o "$(MAINEXE)" $(MAINOBJ) $(OBJS) $(LDFLAGS)

# test
$(TESTTARGET) : $(TESTOBJ) $(OBJS)
	$(CC) -o "$(TESTTARGET)" $(TESTOBJ) $(OBJS) $(LDFLAGS)

clean :
	rm -f $(OBJS) $(TESTOBJ) $(MAINOBJ) "$(MAINEXE)" "$(TESTTARGET)"
