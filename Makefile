#
# General
#
EXEC = makeitpixel
CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -Werror -fmax-errors=3 $(INCLUDES) $(LINKDIRS)
CC = $(CXX)
CFLAGS = $(CXXFLAGS)

INCLUDES = -Iinclude
LINKDIRS =
LDFLAGS = 
LIBS = -lreadline -lpthread
LINKAGE = $(LIBS) $(LDFLAGS)
SRC_FILES = $(wildcard src/*.cpp) src/lexer.cpp src/parser.cpp
		
OBJ = $(SRC_FILES:%.cpp=%.o)
DEP = $(OBJ:%.o=%.d)

#
# Debug build settings
#
DBGDIR = build/debug
DBGEXEC = $(DBGDIR)/$(EXEC)
DBGOBJ = $(addprefix $(DBGDIR)/, $(OBJ))
DBGDEP = $(addprefix $(DBGDIR)/, $(DEP))
DBGCFLAGS = -g -O0 -DDEBUG

#
# Release build settings
#
RELDIR = build/release
RELEXEC = $(RELDIR)/$(EXEC)
RELOBJ = $(addprefix $(RELDIR)/, $(OBJ))
RELDEP = $(addprefix $(RELDIR)/, $(DEP))
RELCFLAGS = -O3 -DNDEBUG

#
# Libraries
#
# SFML
LIBS += -lsfml-graphics -lsfml-window -lsfml-system

#
# Custom output functions
#
define print_info
	@echo -e "\033[1;38;2;250;250;50m$(1)\033[0m"
endef
define print_success
	@echo -e "\033[1;38;2;50;250;50m$(1)\033[0m"
endef

#
# Rules
#
.PHONY: all debug release remake clean clean-release clean-debug docs

# Default build
all: release


$(shell mkdir -p build/debug/src build/release/src )
# Include dependency rules
-include $(DBGDEP)
-include $(RELDEP)

#
# Interpreter source rules
#

src/parser.cpp: src/parser.bison
	bison --defines=include/parser.hpp src/parser.bison -o src/parser.cpp

src/lexer.cpp: src/lexer.flex
	flex --nounistd -o src/lexer.cpp src/lexer.flex

#
# Debug rules
#
debug: $(DBGEXEC)

$(DBGEXEC): $(DBGOBJ)
	$(call print_info,Building $@)
	@$(CXX) $(CXXFLAGS) $(DBGCFLAGS) $^ -o $(DBGEXEC) $(LINKAGE)
	$(call print_success,$< ready)

$(DBGDIR)/%.d: %.cpp
	$(call print_info,Checking debug dependencies for $<)
	$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@
	@echo -e '\t$$(call print_info,Building $$@)' >> $@
	@echo -e '\t$$(CXX) -c $$(CXXFLAGS) $$(DBGCFLAGS) -o $$@ $$<' >> $@

#
# Release rules
#
release: $(RELEXEC)
	
$(RELEXEC): $(RELOBJ)
	$(call print_info,Building $@)
	$(CXX) $(CXXFLAGS) $(RELCFLAGS) $^ -o $(RELEXEC) $(LINKAGE)
	$(call print_success,$< ready)

$(RELDIR)/%.d: %.cpp
	$(call print_info,Checking release dependencies for $<)
	$(CPP) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@
	@echo -e '\t$$(call print_info,Building $$@)' >> $@
	@echo -e '\t$$(CXX) -c $$(CXXFLAGS) $$(RELCFLAGS) -o $$@ $$<' >> $@
	
#
# Other rules
#

remake: clean all

clean: clean-release clean-debug

clean-release:
	@rm -f -r $(RELDIR)
	@mkdir -p $(RELDIR)/src
clean-debug:
	@rm -f -r $(DBGDIR)
	@mkdir -p $(DBGDIR)/src
# 
# Documentation rules
# 

DOXYFILE = doc/Doxyfile
	
docs:
	@doxygen $(DOXYFILE)
	
