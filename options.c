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
 * options.c
 */

#include <stdlib.h>
#include <string.h>

#include "nvidia-xconfig.h"
#include "xf86Parser.h"


typedef struct {
    unsigned int num;
    int invert;
    const char *name;
} NvidiaXConfigOption;

static const NvidiaXConfigOption __options[] = {
    
    { NOLOGO_OPTION,                    TRUE,  "NoLogo" },
    { UBB_OPTION,                       FALSE, "UBB" },
    { RENDER_ACCEL_OPTION,              FALSE, "RenderAccel" },
    { NO_RENDER_EXTENSION_OPTION,       TRUE,  "NoRenderExtension" },
    { OVERLAY_OPTION,                   FALSE, "Overlay" },
    { CIOVERLAY_OPTION,                 FALSE, "CIOverlay" },
    { OVERLAY_DEFAULT_VISUAL_OPTION,    FALSE, "OverlayDefaultVisual" },
    { NO_BANDWIDTH_TEST_OPTION,         TRUE,  "NoBandWidthTest" },
    { NO_POWER_CONNECTOR_CHECK_OPTION,  TRUE,  "NoPowerConnectorCheck" },
    { ALLOW_DFP_STEREO_OPTION,          FALSE, "AllowDFPStereo" },
    { ALLOW_GLX_WITH_COMPOSITE_OPTION,  FALSE, "AllowGLXWithComposite" },
    { RANDR_ROTATION_OPTION,            FALSE, "RandRRotation" },
    { TWINVIEW_OPTION,                  FALSE, "TwinView" },
    { XINERAMA_OPTION,                  FALSE, "Xinerama" },
    { NO_TWINVIEW_XINERAMA_INFO_OPTION, TRUE,  "NoTwinViewXineramaInfo" },
    { NOFLIP_OPTION,                    TRUE,  "NoFlip" },
    { DAC_8BIT_OPTION,                  FALSE, "Dac8Bit" },
    { USE_EDID_FREQS_OPTION,            FALSE, "UseEdidFreqs" },
    { IGNORE_EDID_OPTION,               FALSE, "IgnoreEDID" },
    { USE_INT10_MODULE_OPTION,          FALSE, "UseInt10Module" },
    { FORCE_STEREO_FLIPPING_OPTION,     FALSE, "ForceStereoFlipping" },
    { MULTISAMPLE_COMPATIBILITY_OPTION, FALSE, "MultisampleCompatibility" },
    { XVMC_USES_TEXTURES_OPTION,        FALSE, "XvmcUsesTextures" },
    { EXACT_MODE_TIMINGS_DVI_OPTION,    FALSE, "ExactModeTimingsDVI" },
    { ALLOW_DDCCI_OPTION,               FALSE, "AllowDDCCI" },
    { LOAD_KERNEL_MODULE_OPTION,        FALSE, "LoadKernelModule" },
    { 0,                                FALSE, NULL },
};



/*
 * get_option() - get the NvidiaXConfigOption entry for the given
 * option index
 */

static const NvidiaXConfigOption *get_option(const int n)
{
    int i;

    for (i = 0; __options[i].name; i++) {
        if (__options[i].num == n) return &__options[i];
    }
    return NULL;
    
} /* get_option() */



/*
 * remove_option_from_list() - remove the option with the given name
 * from the list
 */

void remove_option_from_list(XConfigOptionPtr *list, const char *name)
{
    XConfigOptionPtr opt = xconfigFindOption(*list, name);
    if (opt) {
        *list = xconfigRemoveOption(*list, opt);
    }
} /* remove_option_from_list() */



/*
 * remove_option() - make sure the named option does not exist in any
 * of the possible option lists:
 *
 * Options related to drivers can be present in the Screen, Device and
 * Monitor sections and the Display subsections.  The order of
 * precedence is Display, Screen, Monitor, Device.
 */

static void remove_option(XConfigScreenPtr screen, const char *name)
{
    XConfigDisplayPtr display;

    remove_option_from_list(&screen->device->options, name);
    remove_option_from_list(&screen->monitor->options, name);
    remove_option_from_list(&screen->options, name);
    
    for (display = screen->displays; display; display = display->next) {
        remove_option_from_list(&display->options, name);
    }
} /* remove_option() */



/*
 * set_option_value() - set the given option to the specified value
 */

static void set_option_value(XConfigScreenPtr screen,
                             const char *name, const char *val)
{
    /* first, remove the option to make sure it doesn't exist
       elsewhere */

    remove_option(screen, name);

    /* then, add the option to the screen's option list */

    screen->options = xconfigAddNewOption(screen->options,
                                          nvstrdup(name), nvstrdup(val));
} /* set_option_value() */



/*
 * update_twinview_options() - update the TwinView options
 */

static void update_twinview_options(Options *op, XConfigScreenPtr screen)
{
    /*
     * if TwinView was specified, enable/disable the other TwinView
     * options, too
     */

    if (GET_BOOL_OPTION(op->boolean_options, TWINVIEW_OPTION)) {
        if (GET_BOOL_OPTION(op->boolean_option_values, TWINVIEW_OPTION)) {
            set_option_value(screen, "TwinViewOrientation", "RightOf");
            set_option_value(screen, "UseEdidFreqs", "True"); /* XXX */
            set_option_value(screen, "MetaModes", "1024x768, 1024x768");
        } else {
            remove_option(screen, "TwinViewOrientation");
            remove_option(screen, "SecondMonitorHorizSync");
            remove_option(screen, "SecondMonitorVertRefresh");
            remove_option(screen, "MetaModes");
        }
    }
} /* update_twinview_options() */



/*
 * update_options() - update the X Config options, based on the
 * command line arguments.
 */

void update_options(Options *op, XConfigScreenPtr screen)
{
    int i;
    XConfigDisplayPtr display;    
    const NvidiaXConfigOption *o;
    char *val;
    char scratch[8];

    /* update any boolean options specified on the commandline */

    for (i = 0; i < XCONFIG_BOOL_OPTION_COUNT; i++) {
        if (GET_BOOL_OPTION(op->boolean_options, i)) {
            
            /*
             * SEPARATE_X_SCREENS_OPTION and XINERAMA_OPTION are
             * handled separately
             */

            if (i == SEPARATE_X_SCREENS_OPTION) continue;
            if (i == XINERAMA_OPTION) continue;
            
            o = get_option(i);
            
            if (!o) {
                fmterr("Unrecognized X Config option %d", i);
                continue;
            }

            if (GET_BOOL_OPTION(op->boolean_option_values, i)) {
                val = o->invert ? "False" : "True";
            } else {
                val = o->invert ? "True" : "False";
            }
            
            set_option_value(screen, o->name, val);
            fmtmsg("Option \"%s\" \"%s\" added to "
                   "Screen \"%s\".", o->name, val, screen->identifier);
        }
    }

    /* update the TwinView-related options */

    update_twinview_options(op, screen);
    
    /* update the mode list, based on what we have on the commandline */
    
    for (display = screen->displays; display; display = display->next) {
        for (i = 0; i < op->remove_modes.n; i++) {
            display->modes = xconfigRemoveMode(display->modes,
                                               op->remove_modes.t[i]);
        }
        for (i = 0; i < op->add_modes.n; i++) {
            display->modes = xconfigAddMode(display->modes,
                                            op->add_modes.t[i]);
        }

        /* XXX should we sort the mode list? */

        /*
         * XXX should we update the mode list with what we can get
         * through libnvidia-cfg?
         */
    }
    
    /* add the nvagp option */
    
    if (op->nvagp != -1) {
        remove_option(screen, "nvagp");
        if (op->nvagp != -2) {
            snprintf(scratch, 8, "%d", op->nvagp);
            set_option_value(screen, "NvAGP", scratch);
        }
    }

    /* add the digital vibrance option */
    
    if (op->digital_vibrance != -1) {
        remove_option(screen, "digitalvibrance");
        if (op->digital_vibrance != -2) {
            snprintf(scratch, 8, "%d", op->digital_vibrance);        
            set_option_value(screen, "DigitalVibrance", scratch);
        }
    }

    /* add the transparent index option */
    
    if (op->transparent_index != -1) {
        remove_option(screen, "transparentindex");
        if (op->transparent_index != -2) {
            snprintf(scratch, 8, "%d", op->transparent_index);
            set_option_value(screen, "TransparentIndex", scratch);
        }
    }

    /* add the stereo option */
    
    if (op->stereo != -1) {
        remove_option(screen, "stereo");
        if (op->stereo != -2) {
            snprintf(scratch, 8, "%d", op->stereo);
            set_option_value(screen, "Stereo", scratch);
        }
    }

    /* add the SLI option */

    if (op->sli) {
        remove_option(screen, "SLI");
        set_option_value(screen, "SLI", op->sli);
    }

    /* add the rotate option */

    if (op->rotate) {
        remove_option(screen, "Rotate");
        set_option_value(screen, "Rotate", op->rotate);
    }
} /* update_options() */
