#
# nvidia-xconfig: A tool for manipulating X config files,
# specifically for use by the NVIDIA Linux graphics driver.
#
# Copyright (C) 2005 NVIDIA Corporation
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License as
# published by the Free Software Foundation; either version 2 of the
# License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the:
#
#      Free Software Foundation, Inc.
#      59 Temple Place - Suite 330
#      Boston, MA 02111-1307, USA
#
#
# Makefile
#

# default definitions; can be overwritten by users

SHELL = /bin/sh
INSTALL = install
BUILD_OS := $(shell uname)
BUILD_ARCH := $(shell uname -m)

ifndef CC
  CC = gcc
endif

ifndef HOST_CC
  HOST_CC = $(CC)
endif

CPP = cpp

ifndef CFLAGS
  CFLAGS = -g -O -Wall
endif
ifdef NV_CFLAGS
  CFLAGS += $(NV_CFLAGS)
endif

# the NVDEBUG environment variable controls whether we build debug or retail

ifeq ($(NVDEBUG),1)
  STRIP = true
  CFLAGS += -g -DDEBUG
else
  STRIP = strip
  CFLAGS += -O
endif

# default prefix
ifdef ROOT
  prefix = $(ROOT)/usr
else
  prefix = /usr/local
endif

# default echo within SunOS sh does not have -n option. Use /usr/ucb/echo instead.
# Solaris install has a different argument syntax
# Also, man pages are not gzipped on Solaris.
ifeq ($(BUILD_OS),SunOS)
ECHO=/usr/ucb/echo
define INSTALL_RULE
	$(INSTALL) -m 755 -f $(bindir) $<
	mkdir -p $(mandir)
	$(INSTALL) -m 644 -f $(mandir) $(MANPAGE)
endef
LLSCF=-lscf
else
ECHO=echo
define INSTALL_RULE
	$(INSTALL) -m 755 $< $(bindir)
	mkdir -p $(mandir)
	$(INSTALL) -m 644 $(MANPAGE) $(mandir)
	gzip -9f $(mandir)/$(MANPAGE)
endef
LLSCF=
endif

exec_prefix = $(prefix)
bindir = $(exec_prefix)/bin
mandir = $(exec_prefix)/share/man/man1

NVIDIA_XCONFIG = nvidia-xconfig
NVIDIA_XCONFIG_VERSION = 1.0

NVIDIA_XCONFIG_DISTDIR = $(NVIDIA_XCONFIG)-$(NVIDIA_XCONFIG_VERSION)

EXTRA_DIST = \
	nvgetopt.h \
	nvidia-xconfig.h \
	nvidia-cfg.h \
	option_table.h \
	Makefile \
	XF86Config-parser/*.c \
	XF86Config-parser/*.h \
	XF86Config-parser/Makefile \
	nvidia-xconfig.1.m4 \
	gen-manpage-opts.c

MANPAGE = nvidia-xconfig.1

STAMP_C = g_stamp.c

SRC =	util.c			\
	nvidia-xconfig.c 	\
	make_usable.c		\
	multiple_screens.c 	\
	tree.c			\
	nvgetopt.c		\
	options.c		\
	lscf.c			\
	query_gpu_info.c

ALL_SRC = $(SRC) $(STAMP_C)

OBJS = $(ALL_SRC:.c=.o)

SUBDIR = XF86Config-parser


CFLAGS += -I $(SUBDIR)
CPPFLAGS = $(CFLAGS)
LDFLAGS = -lm $(LLSCF)

ifneq ($(BUILD_OS),FreeBSD)
LDFLAGS += -ldl
endif


LIB = $(SUBDIR)/libXF86Config-parser.a

default: all

all: $(NVIDIA_XCONFIG) $(MANPAGE)

install: $(NVIDIA_XCONFIG)
	$(STRIP) $<
	$(INSTALL_RULE)

$(NVIDIA_XCONFIG): $(OBJS) $(LIB)
	$(CC) $(CFLAGS) $(OBJS) $(LDFLAGS) -o $@ $(LIB)

.PHONY: $(LIB) dist clean clobber

$(LIB):
	$(MAKE) NV_CFLAGS='$(NV_CFLAGS)' -C $(SUBDIR)

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o $@

%.d: %.c
	@set -e; $(CC) -MM $(CPPFLAGS) $< \
		| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
		[ -s $@ ] || rm -f $@

$(STAMP_C): $(filter-out $(STAMP_C:.c=.o), $(OBJS))
	@ rm -f $@
	@ $(ECHO) -n "const char NV_ID[] = \"nvidia id: " >> $@
	@ $(ECHO) -n "$(NVIDIA_XCONFIG):  " >> $@
	@ $(ECHO) -n "version $(NVIDIA_XCONFIG_VERSION)  " >> $@
	@ $(ECHO) -n "($(shell whoami)@$(shell hostname))  " >> $@
	@ $(ECHO)    "$(shell date)\";" >> $@
	@ $(ECHO)    "const char *pNV_ID = NV_ID + 11;" >> $@

.PHONY: dist print_version clean clobber

dist: $(EXTRA_DIST)
	@ if [ -d $(NVIDIA_XCONFIG_DISTDIR) ]; then \
		chmod 755 $(NVIDIA_XCONFIG_DISTDIR); \
	fi
	@ if [ -f $(NVIDIA_XCONFIG_DISTDIR).tar.gz ]; \
		then chmod 644 $(NVIDIA_XCONFIG_DISTDIR).tar.gz; \
	fi
	rm -rf $(NVIDIA_XCONFIG_DISTDIR) $(NVIDIA_XCONFIG_DISTDIR).tar.gz
	mkdir -p $(NVIDIA_XCONFIG_DISTDIR)/$(SUBDIR)
	@ for i in $(SRC); do \
		if [ $$i ]; then \
			cp $$i $(NVIDIA_XCONFIG_DISTDIR)/$$i ; \
			chmod 644 $(NVIDIA_XCONFIG_DISTDIR)/$$i ; \
		fi ; \
	done
	@ for i in $(EXTRA_DIST); do \
		if [ $$i ]; then \
			cp $$i $(NVIDIA_XCONFIG_DISTDIR)/$$i ; \
			chmod 644 $(NVIDIA_XCONFIG_DISTDIR)/$$i ; \
		fi ; \
	done
	tar czf $(NVIDIA_XCONFIG_DISTDIR).tar.gz $(NVIDIA_XCONFIG_DISTDIR)
	rm -rf $(NVIDIA_XCONFIG_DISTDIR)

print_version:
	@ $(ECHO) $(NVIDIA_XCONFIG_VERSION)

clean clobber:
	rm -rf *.o *~ *.d $(STAMP_C) $(NVIDIA_XCONFIG) $(MANPAGE) gen-manpage-opts options.1.inc
	$(MAKE) -C $(SUBDIR) $@

### Documentation

AUTO_TEXT = ".\\\" WARNING: THIS FILE IS AUTO-GENERATED!  Edit $< instead."

doc: $(MANPAGE)

gen-manpage-opts.o: gen-manpage-opts.c
	$(HOST_CC) $(CFLAGS) -c $<

gen-manpage-opts: gen-manpage-opts.o
	$(HOST_CC) $(CFLAGS) $^ $(LDFLAGS) -o $@

-include gen-manpage-opts.d

options.1.inc: gen-manpage-opts
	./$< > $@

nvidia-xconfig.1: nvidia-xconfig.1.m4 options.1.inc
	m4 -D__HEADER__=$(AUTO_TEXT) $< > $@

###

-include $(SRC:.c=.d)

