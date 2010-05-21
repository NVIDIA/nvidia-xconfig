#
# nvidia-xconfig: A tool for manipulating X config files,
# specifically for use by the NVIDIA Linux graphics driver.
#
# Copyright (C) 2008 NVIDIA Corporation.
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of Version 2 of the GNU General Public
# License as published by the Free Software Foundation.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See Version 2
# of the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the:
#
#           Free Software Foundation, Inc.
#           59 Temple Place - Suite 330
#           Boston, MA 02111-1307, USA
#

##############################################################################
# define the list of files that should be distributed in the
# nvidia-xconfig tarball; this is used by the NVIDIA driver build
# when packaging the tarball, and by the nvidia-xconfig makefile when
# building nvidia-xconfig.
#
# Defines SRC and DIST_FILES
##############################################################################

XCONFIG_PARSER_DIR = XF86Config-parser

include $(XCONFIG_PARSER_DIR)/src.mk

SRC := $(addprefix $(XCONFIG_PARSER_DIR)/,$(XCONFIG_PARSER_SRC))
SRC += util.c
SRC += nvidia-xconfig.c
SRC += make_usable.c
SRC += multiple_screens.c
SRC += tree.c
SRC += nvgetopt.c
SRC += options.c
SRC += lscf.c
SRC += query_gpu_info.c
SRC += extract_edids.c

DIST_FILES := $(SRC)
DIST_FILES += $(addprefix $(XCONFIG_PARSER_DIR)/,$(XCONFIG_PARSER_EXTRA_DIST))
DIST_FILES += nvgetopt.h
DIST_FILES += nvidia-xconfig.h
DIST_FILES += option_table.h
DIST_FILES += nvidia-xconfig.1.m4
DIST_FILES += gen-manpage-opts.c
DIST_FILES += dist-files.mk
