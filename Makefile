MAIN		= $(notdir $(shell pwd))
SOURCEDIR	= src
BUILDDIR	= build
HEADERDIR	= include

CC		= gcc
CFLAGS		= -Wall -std=c++11

SOURCES		= $(wildcard $(SOURCEDIR)/*.cpp)
OBJECTS		= $(patsubst $(SOURCEDIR)/%.cpp,$(BUILDDIR)/%.o,$(SOURCES))
DEPS		= $(patsubst $(BUILDDIR)/%.o,$(BUILDDIR)/%.d,$(OBJECTS))
-include $(DEPS)
DEPFLAGS	= -MMD -MF $(@:.o=.d)
HEADERPATH	= $(shell pwd)/$(HEADERDIR)
COMPILER	= $(CC) $(CFLAGS) -I$(HEADERPATH)

all: $(MAIN)

$(MAIN): $(OBJECTS) $(MAIN).cpp
	$(COMPILER) -lstdc++ -lcurses -o $(MAIN) $(OBJECTS) $(MAIN).cpp

$(BUILDDIR)/%.o: $(SOURCEDIR)/%.cpp build_dir
	$(COMPILER) $(DEPFLAGS) -c $< -o $@

build_dir:
	mkdir -p $(BUILDDIR)

clean:
	rm -rf $(BUILDDIR) $(MAIN)
