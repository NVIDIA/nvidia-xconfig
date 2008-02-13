/*
 * nvidia-xconfig: A tool for manipulating X config files,
 * specifically for use by the NVIDIA Linux graphics driver.
 *
 * Copyright (C) 2004 NVIDIA Corporation
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the:
 *
 *      Free Software Foundation, Inc.
 *      59 Temple Place - Suite 330
 *      Boston, MA 02111-1307, USA
 *
 *
 * nvidia-xconfig.h
 */

#ifndef __NVIDIA_XCONFIG_H__
#define __NVIDIA_XCONFIG_H__

#define PROGRAM_NAME "nvidia-xconfig"

#include "xf86Parser.h"
#include "nvidia-cfg.h"

#include <sys/types.h>

#if !defined(TRUE)
#define TRUE 1
#endif

#if !defined(FALSE)
#define FALSE 0
#endif


typedef struct {
    char **t; /* the text rows */
    int n;    /* number of rows */
    int m;    /* maximum row length */
} TextRows;


/* Boolean options */
#define NOLOGO_OPTION                      1
#define UBB_OPTION                         2
#define RENDER_ACCEL_OPTION                3
#define NO_RENDER_EXTENSION_OPTION         4
#define OVERLAY_OPTION                     5
#define CIOVERLAY_OPTION                   6
#define OVERLAY_DEFAULT_VISUAL_OPTION      7
#define NO_BANDWIDTH_TEST_OPTION           8
#define NO_POWER_CONNECTOR_CHECK_OPTION    9
#define ALLOW_DFP_STEREO_OPTION            10
#define ALLOW_GLX_WITH_COMPOSITE_OPTION    11
#define RANDR_ROTATION_OPTION              12
#define TWINVIEW_OPTION                    13
#define SEPARATE_X_SCREENS_OPTION          14
#define XINERAMA_OPTION                    15
#define NO_TWINVIEW_XINERAMA_INFO_OPTION   16
#define NOFLIP_OPTION                      17
#define DAC_8BIT_OPTION                    18
#define USE_EDID_FREQS_OPTION              19
#define IGNORE_EDID_OPTION                 20
#define USE_INT10_MODULE_OPTION            21
#define FORCE_STEREO_FLIPPING_OPTION       22
#define MULTISAMPLE_COMPATIBILITY_OPTION   23
#define XVMC_USES_TEXTURES_OPTION          24
#define EXACT_MODE_TIMINGS_DVI_OPTION      25
#define ALLOW_DDCCI_OPTION                 26
#define LOAD_KERNEL_MODULE_OPTION          27

#define XCONFIG_BOOL_OPTION_COUNT LOAD_KERNEL_MODULE_OPTION

/* # of 32-bit variables needed to hold all the boolean options (bits) */
#define XCONFIG_BOOL_OPTION_SLOTS  \
  ((XCONFIG_BOOL_OPTION_COUNT)/32) +      \
  (((XCONFIG_BOOL_OPTION_COUNT)%32)?1:0)

#define GET_BOOL_OPTION_SLOT(BLOCKS, VAR)  \
  ((BLOCKS)[(u32)((VAR)/32)])

#define GET_BOOL_OPTION_BIT(VAR)  \
  (1<<(u32)((VAR)%32))

#define GET_BOOL_OPTION(BLOCKS, VAR)        \
  (GET_BOOL_OPTION_SLOT((BLOCKS), (VAR)) &  \
   GET_BOOL_OPTION_BIT(VAR))

/* 32 bit unsigned variable (used to pack booleans) */
typedef unsigned int u32;


typedef struct __options {
    int force_generate;
    int tree;
    int post_tree;
    int silent;
    int keyboard_list;
    int mouse_list;
    int enable_all_gpus;
    int only_one_screen;
    int disable_scf;
    
    /*
     * the option parser will set bits in boolean_options to indicate
     * whether an option was set, and will then turn the corresponding
     * bit in boolean_option_values on or off to indicate if the
     * option was true or false.
     */
    
    u32 boolean_options[XCONFIG_BOOL_OPTION_SLOTS];
    u32 boolean_option_values[XCONFIG_BOOL_OPTION_SLOTS];

    int depth;
    int nvagp;
    int digital_vibrance;
    int transparent_index;
    int stereo;

    char *xconfig;
    char *output_xconfig;
    char *layout;
    char *screen;
    char *sli;
    char *rotate;

    char *nvidia_cfg_path;

    TextRows add_modes;
    TextRows remove_modes;

    GenerateOptions gop;

} Options;


/* util.c */

void *nvalloc(size_t size);
char *nvstrcat(const char *str, ...);
void *nvrealloc(void *ptr, size_t size);
char *nvstrdup(const char *s);
void nvfree(char *s);
int copy_file(const char *srcfile, const char *dstfile, mode_t mode);

int directory_exists(const char *dir);
char *tilde_expansion(char *str);

void reset_current_terminal_width(unsigned short new_val);

TextRows *nv_format_text_rows(const char *prefix,
                              const char *str,
                              int width, int word_boundary);
void nv_text_rows_append(TextRows *t, const char *msg);
void nv_free_text_rows(TextRows *t);
void nv_concat_text_rows(TextRows *t0, TextRows *t1);

char *fget_next_line(FILE *fp, int *eof);

void fmtout(const char *fmt, ...);
void fmtoutp(const char *prefix, const char *fmt, ...);
void fmtmsg(const char *fmt, ...);
void fmterr(const char *fmt, ...);
void fmtwarn(const char *fmt, ...);


/* make_usable.c */

int update_modules(XConfigPtr config);
int update_screen(Options *op, XConfigPtr config, XConfigScreenPtr screen);
XConfigLayoutPtr get_layout(Options *op, XConfigPtr config);

/* multiple_screens.c */

int apply_multi_screen_options(Options *op, XConfigPtr config,
                               XConfigLayoutPtr layout);

/* tree.c */

int print_tree(Options *op, XConfigPtr config);

/* options.c */

void remove_option_from_list(XConfigOptionPtr *list, const char *name);
void update_options(Options *op, XConfigScreenPtr screen);

/* lscf.c */
int update_scf_depth(int depth);

#endif /* __NVIDIA_XCONFIG_H__ */
