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
#define NOLOGO_BOOL_OPTION                      0
#define UBB_BOOL_OPTION                         1
#define RENDER_ACCEL_BOOL_OPTION                2
#define NO_RENDER_EXTENSION_BOOL_OPTION         3
#define OVERLAY_BOOL_OPTION                     4
#define CIOVERLAY_BOOL_OPTION                   5
#define OVERLAY_DEFAULT_VISUAL_BOOL_OPTION      6
#define NO_BANDWIDTH_TEST_BOOL_OPTION           7
#define NO_POWER_CONNECTOR_CHECK_BOOL_OPTION    8
#define ALLOW_GLX_WITH_COMPOSITE_BOOL_OPTION    10
#define RANDR_ROTATION_BOOL_OPTION              11
#define TWINVIEW_BOOL_OPTION                    12
#define SEPARATE_X_SCREENS_BOOL_OPTION          13
#define XINERAMA_BOOL_OPTION                    14
#define NO_TWINVIEW_XINERAMA_INFO_BOOL_OPTION   15
#define NOFLIP_BOOL_OPTION                      16
#define DAC_8BIT_BOOL_OPTION                    17
#define USE_EDID_FREQS_BOOL_OPTION              18
#define USE_EDID_BOOL_OPTION                    19
#define USE_INT10_MODULE_BOOL_OPTION            20
#define FORCE_STEREO_FLIPPING_BOOL_OPTION       21
#define MULTISAMPLE_COMPATIBILITY_BOOL_OPTION   22
#define XVMC_USES_TEXTURES_BOOL_OPTION          23
#define EXACT_MODE_TIMINGS_DVI_BOOL_OPTION      24
#define ADD_ARGB_GLX_VISUALS_BOOL_OPTION        27
#define COMPOSITE_BOOL_OPTION                   28
#define DISABLE_GLX_ROOT_CLIPPING_BOOL_OPTION   29
#define USE_EDID_DPI_BOOL_OPTION                30
#define DAMAGE_EVENTS_BOOL_OPTION               31
#define CONSTANT_DPI_BOOL_OPTION                32
#define PROBE_ALL_GPUS_BOOL_OPTION              33
#define DYNAMIC_TWINVIEW_BOOL_OPTION            34
#define INCLUDE_IMPLICIT_METAMODES_BOOL_OPTION  35
#define USE_EVENTS_BOOL_OPTION                  36
#define CONNECT_TO_ACPID_BOOL_OPTION            37
#define ENABLE_ACPI_HOTKEYS_BOOL_OPTION         38
#define MODE_DEBUG_BOOL_OPTION                  39
#define THERMAL_CONFIGURATION_CHECK_BOOL_OPTION 40
#define PRESERVE_BUSID_BOOL_OPTION              41

#define XCONFIG_BOOL_OPTION_COUNT (PRESERVE_BUSID_BOOL_OPTION + 1)

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


/* define to store in string options */
#define NV_DISABLE_STRING_OPTION ((void *) -1)

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
    int query_gpu_info;
    int preserve_driver;
    
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
    int transparent_index;
    int stereo;
    int cool_bits;

    char *xconfig;
    char *output_xconfig;
    char *layout;
    char *screen;
    char *device;
    char *busid;
    char *multigpu;
    char *sli;
    char *rotate;

    char *nvidia_cfg_path;
    char *extract_edids_from_file;
    char *extract_edids_output_file;
    char *twinview_xinerama_info_order;
    char *logo_path;
    char *twinview_orientation;
    char *use_display_device;
    char *custom_edid;
    char *tv_standard;
    char *tv_out_format;
    char *acpid_socket_path;
    char *handle_special_keys;
    char *connected_monitor;
    char *registry_dwords;
    char *metamodes_str;
    char *color_space;
    char *color_range;
    char *flatpanel_properties;
    char *nvidia_3dvision_usb_path;
    char *nvidia_3dvisionpro_config_file;
    double tv_over_scan;

    struct {
        int x;
        int y;
    } virtual;

    TextRows add_modes;
    TextRows add_modes_list;
    TextRows remove_modes;

    GenerateOptions gop;

} Options;

/* data structures for storing queried GPU information */

typedef struct _display_device_rec {
    NvCfgDisplayDeviceInformation info;
    int info_valid;
    unsigned int mask;
} DisplayDeviceRec, *DisplayDevicePtr;

typedef struct _device_rec {
    NvCfgPciDevice dev;
    NvCfgDeviceHandle handle;
    int crtcs;
    char *name;
    unsigned int displayDeviceMask;
    int nDisplayDevices;
    DisplayDevicePtr displayDevices;    
} DeviceRec, *DevicePtr;

typedef struct {
    int nDevices;
    DevicePtr devices;
} DevicesRec, *DevicesPtr;


/* util.c */

void *nvalloc(size_t size);
char *nvstrcat(const char *str, ...);
void *nvrealloc(void *ptr, size_t size);
char *nvstrdup(const char *s);
void nvfree(void *s);
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
int update_extensions(Options *op, XConfigPtr config);
int update_server_flags(Options *op, XConfigPtr config);

/* multiple_screens.c */

DevicesPtr find_devices(Options *op);
void free_devices(DevicesPtr devs);

int apply_multi_screen_options(Options *op, XConfigPtr config,
                               XConfigLayoutPtr layout);

/* tree.c */

int print_tree(Options *op, XConfigPtr config);

/* options.c */

void set_boolean_option(Options *op, const int c, const int boolval);
void validate_composite(Options *op, XConfigPtr config);
void update_options(Options *op, XConfigScreenPtr screen);

/* lscf.c */
int update_scf_depth(int depth);
int read_scf_depth(int *depth);

/* query_gpu_info.c */

int query_gpu_info(Options *op);

/* extract_edids.c */

int extract_edids(Options *op);



#endif /* __NVIDIA_XCONFIG_H__ */
