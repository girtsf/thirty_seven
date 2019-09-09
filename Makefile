# Adapted from imgui examples.
#
# You will need SDL2 (http://www.libsdl.org):
# Linux:
#   apt-get install libsdl2-dev
# Mac OS X:
#   brew install sdl2

EXE = thirty_seven
IMGUI_DIR = third_party/imgui
OUT_DIR := build

SOURCES = main.cpp
SOURCES += $(IMGUI_DIR)/examples/imgui_impl_sdl.cpp
SOURCES += $(IMGUI_DIR)/examples/imgui_impl_opengl3.cpp
SOURCES += $(IMGUI_DIR)/imgui.cpp
SOURCES += $(IMGUI_DIR)/imgui_demo.cpp
SOURCES += $(IMGUI_DIR)/imgui_draw.cpp
SOURCES += $(IMGUI_DIR)/imgui_widgets.cpp
SOURCES += third_party/fastled_hsv2rgb/hsv2rgb.cpp

DEFINES =

OBJS = $(addprefix build/,$(addsuffix .o,$(SOURCES)))
UNAME_S := $(shell uname -s)

INCLUDE_DIRS = . $(IMGUI_DIR) $(IMGUI_DIR)/examples

CCFLAGS = $(addprefix -I,$(INCLUDE_DIRS)) $(addprefix -D,$(DEFINES))
DEPFLAGS += -MT $$@ -MMD -MP -MF $(OUT_DIR)/$$<.d
CLAGS += -g -Wall -Wformat
CXXFLAGS = $(CCFLAGS) -std=c++11
LIBS =

##---------------------------------------------------------------------
## OPENGL LOADER
##---------------------------------------------------------------------

## Using OpenGL loader: gl3w [default]
SOURCES += $(IMGUI_DIR)/examples/libs/gl3w/GL/gl3w.c
CXXFLAGS += -I$(IMGUI_DIR)/examples/libs/gl3w

##---------------------------------------------------------------------
## BUILD FLAGS PER PLATFORM
##---------------------------------------------------------------------

ifeq ($(UNAME_S), Linux) #LINUX
	ECHO_MESSAGE = "Linux"
	LIBS += -lGL -ldl `sdl2-config --libs`

	INCLUDE_DIRS += $(IMGUI_DIR)/examples/libs/gl3w
	CXXFLAGS += `sdl2-config --cflags`
	CCFLAGS += `sdl2-config --cflags`
endif

ifeq ($(UNAME_S), Darwin) #APPLE
	ECHO_MESSAGE = "Mac OS X"
	LIBS += -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo `sdl2-config --libs`
	LIBS += -L/usr/local/lib

	INCLUDE_DIRS += $(IMGUI_DIR)/examples/libs/gl3w /usr/local/include /opt/local/include
	CXXFLAGS += `sdl2-config --cflags`
	CCFLAGS += `sdl2-config --cflags`
endif

##---------------------------------------------------------------------
## BUILD RULES
##---------------------------------------------------------------------

$(EXE): $(OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)


# Given a file and CC or CXX, generate a compile rule.
define gen_compile_rule
$(OUT_DIR)/$(1).o : $(1) $(OUT_DIR)/$(1).d | $(dir $(OUT_DIR)/$(1))
	$$($(2)) $$($(2)FLAGS) $(DEPFLAGS) -c -o $$@ $$<
endef

# Given a directory, make a rule to mkdir it.
define gen_mkdir
$(1):
	mkdir -p $(1)
endef

$(foreach src,$(filter %.cpp,$(SOURCES)),$(eval $(call gen_compile_rule,$(src),CXX)))
$(foreach src,$(filter %.c,$(SOURCES)),$(eval $(call gen_compile_rule,$(src),CC)))

$(foreach dir,$(sort $(foreach src,$(SOURCES),$(dir $(OUT_DIR)/$(src)))),$(eval $(call gen_mkdir,$(dir))))

all: $(EXE)
	@echo Build complete for $(ECHO_MESSAGE)

clean:
	rm -rf $(EXE) $(OUT_DIR)

DEPFILES := $(addprefix $(OUT_DIR)/,$(addsuffix .d,$(SOURCES)))
$(DEPFILES):

include $(wildcard $(DEPFILES))
