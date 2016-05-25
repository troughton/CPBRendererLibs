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

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S), Linux) #LINUX
	ECHO_MESSAGE = "Linux"

	OUTPUTNAME = PBRendererCLibs.so
	CXXFLAGS = -Wall
	CXXFLAGS += -shared -fPIC
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(UNAME_S), Darwin) #APPLE
	ECHO_MESSAGE = "Mac OS X"

	OUTPUTNAME = PBRendererCLibs.dylib
	CXXFLAGS = -I/usr/local/include
	CXXFLAGS += -Wall
	LINKFLAGS = -dynamiclib
	CFLAGS = $(CXXFLAGS)
endif

.cpp.o:
	$(CXX) $(CXXFLAGS) -c -o $@ $<

all:PBRendererCLibs
	@echo Build complete for $(ECHO_MESSAGE)

PBRendererCLibs:$(OBJS)
	$(CXX) -o $(OUTPUTNAME) $(OBJS) $(CXXFLAGS) $(LINKFLAGS)

clean:
	rm $(OBJS)
