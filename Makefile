# Adapted from imgui examples.
#
# You will need SDL2 (http://www.libsdl.org):
# Linux:
#   apt-get install libsdl2-dev
# Mac OS X:
#   brew install sdl2

#CXX = g++
#CXX = clang++

EXE = thirty_seven
SOURCES = main.cpp
IMGUI_DIR = third_party/imgui
SOURCES += $(IMGUI_DIR)/examples/imgui_impl_sdl.cpp
SOURCES += $(IMGUI_DIR)/examples/imgui_impl_opengl3.cpp
SOURCES += $(IMGUI_DIR)/imgui.cpp
SOURCES += $(IMGUI_DIR)/imgui_demo.cpp
SOURCES += $(IMGUI_DIR)/imgui_draw.cpp
SOURCES += $(IMGUI_DIR)/imgui_widgets.cpp
SOURCES += third_party/fastled_hsv2rgb/hsv2rgb.cpp
OBJS = $(addprefix build/, $(addsuffix .o, $(basename $(notdir $(SOURCES)))))
# $(error objs = $(OBJS))
UNAME_S := $(shell uname -s)

CXXFLAGS = -I$(IMGUI_DIR) -I$(IMGUI_DIR)/examples
CXXFLAGS += -g -Wall -Wformat
LIBS =

##---------------------------------------------------------------------
## OPENGL LOADER
##---------------------------------------------------------------------

## Using OpenGL loader: gl3w [default]
SOURCES += $(IMGUI_DIR)/examples/libs/gl3w/GL/gl3w.c
CXXFLAGS += -I$(IMGUI_DIR)/examples/libs/gl3w

## Using OpenGL loader: glew
## (This assumes a system-wide installation)
# CXXFLAGS = -lGLEW -DIMGUI_IMPL_OPENGL_LOADER_GLEW

## Using OpenGL loader: glad
## (You'll also need to change the rule at line ~77 of this Makefile to compile/link glad.c/.o)
# SOURCES += ../libs/glad/src/glad.c
# CXXFLAGS = -I../libs/glad/include -DIMGUI_IMPL_OPENGL_LOADER_GLAD

##---------------------------------------------------------------------
## BUILD FLAGS PER PLATFORM
##---------------------------------------------------------------------

ifeq ($(UNAME_S), Linux) #LINUX
	ECHO_MESSAGE = "Linux"
	LIBS += -lGL -ldl `sdl2-config --libs`

	CXXFLAGS += -I$(IMGUI_DIR)/examples/libs/gl3w `sdl2-config --cflags`
	CFLAGS = $(CXXFLAGS)
endif

ifeq ($(UNAME_S), Darwin) #APPLE
	ECHO_MESSAGE = "Mac OS X"
	LIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo `sdl2-config --libs`
	LIBS += -L/usr/local/lib

	CXXFLAGS += -I$(IMGUI_DIR)/examples/libs/gl3w `sdl2-config --cflags`
	CXXFLAGS += -I/usr/local/include -I/opt/local/include
	CFLAGS = $(CXXFLAGS)
endif

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

build:
	mkdir build

build/%.o:%.cpp build
	$(CXX) $(CXXFLAGS) -c -o $@ $<

build/%.o:$(IMGUI_DIR)/%.cpp build
	$(CXX) $(CXXFLAGS) -c -o $@ $<

build/%.o:$(IMGUI_DIR)/examples/%.cpp build
	$(CXX) $(CXXFLAGS) -c -o $@ $<

build/%.o:$(IMGUI_DIR)/examples/libs/gl3w/GL/%.c build
	$(CC) $(CFLAGS) -c -o $@ $<

$(OUT_DIR)/%.o:third_party/fastled_hsv2rgb/%.cpp $(OUT_DIR)/.f
	$(CC) $(CFLAGS) -c -o $@ $<

all: $(EXE)
	@echo Build complete for $(ECHO_MESSAGE)

clean:
	rm -f $(EXE) $(OBJS)
	rm -rf build
