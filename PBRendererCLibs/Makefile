#
# Cross Platform Makefile
# Compatible with Ubuntu 14.04.1 and Mac OS X

OBJS = Implementation.o
OBJS += cimgui/cimgui.o
OBJS += cimgui/fontAtlas.o 
OBJS += cimgui/drawList.o
OBJS += imgui/imgui.o
OBJS += imgui/imgui_draw.o
OBJS += imgui/imgui_demo.o
OBJS += SOIL2/etc1_utils.c
OBJS += SOIL2/image_DXT.c
OBJS += SOIL2/image_helper.c


UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Linux) #LINUX
	ECHO_MESSAGE = "Linux"

	OUTPUTNAME = libPBRendererCLibs.a
	CXXFLAGS = -Wall
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(UNAME_S), Darwin) #APPLE
	ECHO_MESSAGE = "Mac OS X"

	OUTPUTNAME = libPBRendererCLibs.a
	CXXFLAGS = -I/usr/local/include
	CXXFLAGS += -Wall
	CFLAGS = $(CXXFLAGS)
endif

.cpp.o:
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all:PBRendererCLibs
	@echo Build complete for $(ECHO_MESSAGE)

PBRendererCLibs:$(OBJS)
	ar rcs $(OUTPUTNAME) $(OBJS)

clean:
	rm $(OBJS)
