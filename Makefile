#
# nvidia-xconfig: A tool for manipulating X config files,
# specifically for use by the NVIDIA Linux graphics driver.
#
# Copyright (C) 2008 NVIDIA Corporation
#
# This program is free software; you can redistribute it and/or modify it
# under the terms and conditions of the GNU General Public License,
# version 2, as published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful, but WITHOUT
# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
# more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, see <http://www.gnu.org/licenses>.
#
#
# Makefile
#


##############################################################################
# include common variables and functions
##############################################################################

include utils.mk


##############################################################################
# The calling Makefile may export any of the following variables; we
# assign default values if they are not exported by the caller
##############################################################################

NVIDIA_CFG_DIR        ?= .


##############################################################################
# assign variables
##############################################################################

NVIDIA_XCONFIG = $(OUTPUTDIR)/nvidia-xconfig

NVIDIA_XCONFIG_PROGRAM_NAME = "nvidia-xconfig"

NVIDIA_XCONFIG_VERSION := $(NVIDIA_VERSION)

MANPAGE_GZIP ?= 1

MANPAGE_not_gzipped  = $(OUTPUTDIR)/nvidia-xconfig.1
MANPAGE_gzipped      = $(OUTPUTDIR)/nvidia-xconfig.1.gz
ifeq ($(MANPAGE_GZIP),1)
  MANPAGE            = $(MANPAGE_gzipped)
else
  MANPAGE            = $(MANPAGE_not_gzipped)
endif
GEN_MANPAGE_OPTS   = $(OUTPUTDIR)/gen-manpage-opts
OPTIONS_1_INC      = $(OUTPUTDIR)/options.1.inc


##############################################################################
# The common-utils directory may be in one of two places: either
# elsewhere in the driver source tree when building nvidia-xconfig as
# part of the NVIDIA driver build (in which case, COMMON_UTILS_DIR
# should be defined by the calling makefile), or directly in the
# source directory when building from the nvidia-xconfig source
# tarball (in which case, the below conditional assignment should be
# used)
##############################################################################

COMMON_UTILS_DIR          ?= common-utils

include dist-files.mk

SRC += $(STAMP_C)

include $(COMMON_UTILS_DIR)/src.mk
SRC += $(addprefix $(COMMON_UTILS_DIR)/,$(COMMON_UTILS_SRC))

OBJS = $(call BUILD_OBJECT_LIST,$(SRC))

CFLAGS += -I XF86Config-parser
CFLAGS += -I $(OUTPUTDIR)
CFLAGS += -I $(NVIDIA_CFG_DIR)
CFLAGS += -I $(COMMON_UTILS_DIR)
CFLAGS += -DPROGRAM_NAME=\"nvidia-xconfig\"

HOST_CFLAGS += $(CFLAGS)

LDFLAGS += -lm

ifneq ($(TARGET_OS),FreeBSD)
  LDFLAGS += -ldl
endif

ifeq ($(TARGET_OS),SunOS)
  LDFLAGS += -lscf
endif


##############################################################################
# build rules
##############################################################################

.PNONY: all install NVIDIA_XCONFIG_install MANPAGE_install clean clobber

all: $(NVIDIA_XCONFIG) $(MANPAGE)

install: NVIDIA_XCONFIG_install MANPAGE_install

NVIDIA_XCONFIG_install: $(NVIDIA_XCONFIG)
	$(MKDIR) $(bindir)
	$(INSTALL) $(INSTALL_BIN_ARGS) $< $(bindir)/$(notdir $<)

MANPAGE_install: $(MANPAGE)
	$(MKDIR) $(mandir)
	$(INSTALL) $(INSTALL_DOC_ARGS) $< $(mandir)/$(notdir $<)

$(NVIDIA_XCONFIG): $(OBJS)
	$(call quiet_cmd,LINK) -o $@ $(OBJS) $(CFLAGS) \
	  $(LDFLAGS) $(BIN_LDFLAGS)
	$(call quiet_cmd,STRIP_CMD) $@

# define the rule to build each object file
$(foreach src, $(SRC), $(eval $(call DEFINE_OBJECT_RULE,CC,$(src))))

# define the rule to generate $(STAMP_C)
$(eval $(call DEFINE_STAMP_C_RULE, $(OBJS),$(NVIDIA_XCONFIG_PROGRAM_NAME)))

clean clobber:
	$(RM) -rf $(NVIDIA_XCONFIG) $(MANPAGE) *~ $(STAMP_C) \
		$(OUTPUTDIR)/*.o $(OUTPUTDIR)/*.d \
		$(GEN_MANPAGE_OPTS) $(OPTIONS_1_INC)


##############################################################################
# Documentation
##############################################################################

AUTO_TEXT = ".\\\" WARNING: THIS FILE IS AUTO-GENERATED!  Edit $< instead."

doc: $(MANPAGE)

$(eval $(call DEFINE_OBJECT_RULE,HOST_CC,gen-manpage-opts.c))

$(GEN_MANPAGE_OPTS): $(call BUILD_OBJECT_LIST,gen-manpage-opts.c)
	$(call quiet_cmd,HOST_LINK) $< -o $@ \
		$(HOST_CFLAGS) $(HOST_LDFLAGS) $(HOST_BIN_LDFLAGS)

$(OPTIONS_1_INC): $(GEN_MANPAGE_OPTS)
	@./$< > $@

$(MANPAGE_not_gzipped): nvidia-xconfig.1.m4 $(OPTIONS_1_INC)
	$(call quiet_cmd,M4) -D__HEADER__=$(AUTO_TEXT) -I $(OUTPUTDIR) \
	  -D__VERSION__=$(NVIDIA_VERSION) \
	  -D__DATE__="`$(DATE) +%F`" \
	  $< > $@

$(MANPAGE_gzipped): $(MANPAGE_not_gzipped)
	$(GZIP_CMD) -9f < $< > $@
