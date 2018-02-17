## Makefile supports following:
# make all # or link   - to compile and link
# make build           - to build only
# make clean           - to delete all output files
# make cleanall        - to delete output files as well as TAGS and cscope files
# make TAGS            - to create emacs TAGS
# make cscope          - to create cscope files
# make dependencies    - to install dependencies (debian system)

CC = gcc
LD = ld

SRCDIR := src
INCDIR := inc
BUILDDIR := build
DEPDIR := deps
OUTPUTDIR := bin
TARGET := $(OUTPUTDIR)/final

VPATH = $(SRCDIR)

WARNINGS = -Wall -W -Wstrict-prototypes -Wmissing-prototypes -Wsystem-headers
CFLAGS = -g
CPPFLAGS = -I$(INCDIR)
LDFLAGS =
LDLIB = -lreadline
DEPFLAGS = -MT $@ -MMD -MP -MF $(DEPDIR)/$*.Td

SOURCES = $(shell find $(SRCDIR) -type f -name "*.c")
OBJECTS = $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.c=.o))
DEPS = $(patsubst $(SRCDIR)/%,$(DEPDIR)/%,$(SOURCES:.c=.d))

OUTPUT_OPTION = -o $@
COMPILE.c = $(CC) -c $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) $(WARNINGS)
POSTCOMPILE = @mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d && touch $@

$(shell mkdir -p $(DEPDIR) > /dev/null)
$(shell mkdir -p $(BUILDDIR) > /dev/null)
$(shell mkdir -p $(OUTPUTDIR) > /dev/null)

all: build link

dependencies:
	sudo apt-get install libreadline-dev

build: $(OBJECTS)
link: $(TARGET)

.PHONY: clean TAGS install cscope

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OUTPUT_OPTION) $^ $(LDLIB)

%.o: %.c
$(BUILDDIR)/%.o: $(SRCDIR)/%.c $(DEPDIR)/%.d
	$(COMPILE.c) $(OUTPUT_OPTION) $<
	$(POSTCOMPILE)

$(DEPDIR)/%.d: ;
.PRECIOUS: $(DEPDIR)/%.d

-include $(DEPS)

TAGS:
	find . -name "*.[chS]" | xargs etags -a

cscope:
	find . -name "*.[chS]" > cscope.files
	cscope -b -q -k

clean:
	$(RM) -r -v $(BUILDDIR) $(OUTPUTDIR) $(DEPDIR)
	$(RM) out.s lex.txt parse.txt

cleanall: clean
	$(RM) -v cscope.* TAGS

help:
	@echo 'make <all, clean, build, link, dependencies, TAGS, cscope>'
