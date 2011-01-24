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
#include <ctype.h>

#include "nvidia-xconfig.h"
#include "xf86Parser.h"


typedef struct {
    unsigned int num;
    int invert;
    const char *name;
} NvidiaXConfigOption;

static const NvidiaXConfigOption __options[] = {
    
    { NOLOGO_BOOL_OPTION,                    TRUE,  "NoLogo" },
    { UBB_BOOL_OPTION,                       FALSE, "UBB" },
    { RENDER_ACCEL_BOOL_OPTION,              FALSE, "RenderAccel" },
    { NO_RENDER_EXTENSION_BOOL_OPTION,       TRUE,  "NoRenderExtension" },
    { OVERLAY_BOOL_OPTION,                   FALSE, "Overlay" },
    { CIOVERLAY_BOOL_OPTION,                 FALSE, "CIOverlay" },
    { OVERLAY_DEFAULT_VISUAL_BOOL_OPTION,    FALSE, "OverlayDefaultVisual" },
    { NO_BANDWIDTH_TEST_BOOL_OPTION,         TRUE,  "NoBandWidthTest" },
    { NO_POWER_CONNECTOR_CHECK_BOOL_OPTION,  TRUE,  "NoPowerConnectorCheck" },
    { THERMAL_CONFIGURATION_CHECK_BOOL_OPTION, FALSE, "ThermalConfigurationCheck" },
    { ALLOW_GLX_WITH_COMPOSITE_BOOL_OPTION,  FALSE, "AllowGLXWithComposite" },
    { RANDR_ROTATION_BOOL_OPTION,            FALSE, "RandRRotation" },
    { TWINVIEW_BOOL_OPTION,                  FALSE, "TwinView" },
    { XINERAMA_BOOL_OPTION,                  FALSE, "Xinerama" },
    { NO_TWINVIEW_XINERAMA_INFO_BOOL_OPTION, TRUE,  "NoTwinViewXineramaInfo" },
    { NOFLIP_BOOL_OPTION,                    TRUE,  "NoFlip" },
    { DAC_8BIT_BOOL_OPTION,                  FALSE, "Dac8Bit" },
    { USE_EDID_FREQS_BOOL_OPTION,            FALSE, "UseEdidFreqs" },
    { USE_EDID_BOOL_OPTION,                  FALSE, "UseEdid" },
    { USE_INT10_MODULE_BOOL_OPTION,          FALSE, "UseInt10Module" },
    { FORCE_STEREO_FLIPPING_BOOL_OPTION,     FALSE, "ForceStereoFlipping" },
    { MULTISAMPLE_COMPATIBILITY_BOOL_OPTION, FALSE, "MultisampleCompatibility" },
    { XVMC_USES_TEXTURES_BOOL_OPTION,        FALSE, "XvmcUsesTextures" },
    { EXACT_MODE_TIMINGS_DVI_BOOL_OPTION,    FALSE, "ExactModeTimingsDVI" },
    { ADD_ARGB_GLX_VISUALS_BOOL_OPTION,      FALSE, "AddARGBGLXVisuals" },
    { DISABLE_GLX_ROOT_CLIPPING_BOOL_OPTION, FALSE, "DisableGLXRootClipping" },
    { USE_EDID_DPI_BOOL_OPTION,              FALSE, "UseEdidDpi" },
    { DAMAGE_EVENTS_BOOL_OPTION,             FALSE, "DamageEvents" },
    { CONSTANT_DPI_BOOL_OPTION,              FALSE, "ConstantDPI" },
    { PROBE_ALL_GPUS_BOOL_OPTION,            FALSE, "ProbeAllGpus" },
    { DYNAMIC_TWINVIEW_BOOL_OPTION,          FALSE, "DynamicTwinView" },
    { INCLUDE_IMPLICIT_METAMODES_BOOL_OPTION,FALSE, "IncludeImplicitMetaModes" },
    { USE_EVENTS_BOOL_OPTION,                FALSE, "UseEvents" },
    { CONNECT_TO_ACPID_BOOL_OPTION,          FALSE, "ConnectToAcpid" },
    { ENABLE_ACPI_HOTKEYS_BOOL_OPTION,       FALSE, "EnableACPIHotkeys" },
    { MODE_DEBUG_BOOL_OPTION,                FALSE, "ModeDebug" },
    { 0,                                     FALSE, NULL },
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
 * set_boolean_option() - set boolean option 'c' to the given 'boolval'
 */

void set_boolean_option(Options *op, const int c, const int boolval)
{
    u32 bit;
    
    bit = GET_BOOL_OPTION_BIT(c);
    
    GET_BOOL_OPTION_SLOT(op->boolean_options, c) |= bit;
    
    if (boolval) {
        GET_BOOL_OPTION_SLOT(op->boolean_option_values, c) |= bit;
    } else {
        GET_BOOL_OPTION_SLOT(op->boolean_option_values, c) &= ~bit;
    }
} /* set_boolean_option() */



/*
 * validate_composite() - check whether any options conflict with the
 * Composite extension; update the composite option value, if
 * appropriate.
 */

void validate_composite(Options *op, XConfigPtr config)
{
    int i, n, opt, disable_composite;
    char scratch[256], *s;
    
    
    /*
     * the composite_incompatible_options[] array lists all the
     * options that are incompatible with the composite extension; we
     * list boolean options and then special-case any non-boolean options
     */

    static int composite_incompatible_options[] = {
        XINERAMA_BOOL_OPTION,
        OVERLAY_BOOL_OPTION,
        CIOVERLAY_BOOL_OPTION,
        UBB_BOOL_OPTION,
        -1, /* stereo */
        -2 /* end */
    };
    
    disable_composite = FALSE;
    s = scratch;
    n = 0;
    
    /*
     * loop through all the incompatible options, and check if the
     * user specified any of them
     */

    for (i = 0; composite_incompatible_options[i] != -2; i++) {

        int present = 0;
        const char *name;
        
        opt = composite_incompatible_options[i];
     
        if (opt == -1) { /* special case stereo */
            
            present = (op->stereo > 0);
            name = "Stereo";
            
        } else {
            const NvidiaXConfigOption *o;
            
            present = (GET_BOOL_OPTION(op->boolean_options, opt) &&
                       GET_BOOL_OPTION(op->boolean_option_values, opt));
            
            o = get_option(opt);
            name = o->name;
        }
        
        /*
         * if the option is present, then we have to disable
         * composite; append to the scratch string that lists all the
         * present conflicting options
         */
        
        if (present) {
            disable_composite = TRUE;
            n++;
            s += sprintf(s, "%s%s", (n > 1) ? " or " : "", name);
        }
    }
    
    /*
     * if we have to disable the composite extension, print a warning
     * and set the option value.
     *
     * We need to be careful to only set the option value if the X
     * server is going to recognize the Extension section and the
     * composite option.  We guess whether the server will recognize
     * the option: if get_xserver_in_use() thinks the X server
     * supports the "Composite" extension, or the current config
     * already has an extension section, or the user specified the
     * composite option.
     */
    
    if (disable_composite &&
        (op->gop.supports_extension_section ||
         config->extensions ||
         GET_BOOL_OPTION(op->boolean_options, COMPOSITE_BOOL_OPTION))) {
        
        fmtwarn("The Composite X extension does not currently interact well "
                "with the %s option%s; the Composite X extension will be "
                "disabled.", scratch, (n > 1) ? "s": "");
        
        set_boolean_option(op, COMPOSITE_BOOL_OPTION, FALSE);
    }
} /* validate_composite() */



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

    if (!screen) return;

    if (screen->device) {
        xconfigRemoveNamedOption(&screen->device->options, name, NULL);
    }
    if (screen->monitor) {
        xconfigRemoveNamedOption(&screen->monitor->options, name, NULL);
    }
    xconfigRemoveNamedOption(&screen->options, name, NULL);
    
    for (display = screen->displays; display; display = display->next) {
        xconfigRemoveNamedOption(&display->options, name, NULL);
    }
} /* remove_option() */



/*
 * get_screen_option() - get the option structure with the specified
 * name, searching all the option lists associated with this screen
 */

static XConfigOptionPtr get_screen_option(XConfigScreenPtr screen,
                                          const char *name)
{
    XConfigDisplayPtr display;
    XConfigOptionPtr opt;

    if (!screen) return NULL;

    if (screen->device) {
        opt = xconfigFindOption(screen->device->options, name);
        if (opt) return opt;
    }
    if (screen->monitor) {
        opt = xconfigFindOption(screen->monitor->options, name);
        if (opt) return opt;
    }

    opt = xconfigFindOption(screen->options, name);
    if (opt) return opt;

    for (display = screen->displays; display; display = display->next) {
        opt = xconfigFindOption(display->options, name);
        if (opt) return opt;
    }

    return NULL;

} /* get_screen_option() */



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

    xconfigAddNewOption(&screen->options, name, val);

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

    if (GET_BOOL_OPTION(op->boolean_options, TWINVIEW_BOOL_OPTION)) {
        remove_option(screen, "TwinViewOrientation");
        remove_option(screen, "SecondMonitorHorizSync");
        remove_option(screen, "SecondMonitorVertRefresh");
        remove_option(screen, "MetaModes");
        
        if (GET_BOOL_OPTION(op->boolean_option_values, TWINVIEW_BOOL_OPTION)) {
            set_option_value(screen, "MetaModes",
                             "nvidia-auto-select, nvidia-auto-select");
        }
    }
} /* update_twinview_options() */



/*
 * find_metamode_offset() - find the first metamode offset in
 * 'string'; returns a pointer to the start of the offset
 * specification and assigns 'end' (if non-NULL) to the first character
 * beyond the offset specification.
 */

static char *find_metamode_offset(char *string, char **end)
{
    enum {
        StateBeforeOffset,
        StateInFirstPlus,
        StateInFirstNumber,
        StateInSecondPlus,
        StateInSecondNumber
    } state = StateBeforeOffset;

    char *s, *start = NULL, c;

    for (s = string; s && *s; s++) {
        c = *s;
        switch (state) {

        case StateBeforeOffset:
            if ((c == '-') || (c == '+')) {
                start = s;
                state = StateInFirstPlus;
            }
            break;

        case StateInFirstPlus:
            if (isspace(c)) state = StateInFirstPlus;
            else if (isdigit(c)) state = StateInFirstNumber;
            else state = StateBeforeOffset;
            break;

        case StateInFirstNumber:
            if (isdigit(c) || isspace(c)) state = StateInFirstNumber;
            else if ((c == '-') || (c == '+')) state = StateInSecondPlus;
            else state = StateBeforeOffset;
            break;

        case StateInSecondPlus:
            if (isspace(c)) state = StateInSecondPlus;
            else if (isdigit(c)) state = StateInSecondNumber;
            else state = StateBeforeOffset;
            break;

        case StateInSecondNumber:
            if (isdigit(c)) state = StateInSecondNumber;
            else goto done;
            break;
        }
    }

 done:
    if (state == StateInSecondNumber) {
        if (end) *end = s;
        return start;
    }

    return NULL;

} /* find_metamode_offset() */



/*
 * remove_metamode_offsets() - remove any offset specifications from
 * the MetaMode option for this screen; if we find any offsets, return
 * TRUE and assign old_metamodes and new_metamodes to copies of the
 * MetaModes string before and after removing the offsets.  If no
 * offsets appear in the MetaModes string, return FALSE.
 */

static int remove_metamode_offsets(XConfigScreenPtr screen,
                                   char **old_metamodes, char **new_metamodes)
{
    char *start, *end = NULL;
    char *new_string;
    char *n, *o, *tmp;

    XConfigOptionPtr opt = get_screen_option(screen, "MetaModes");

    /* return if no MetaModes option */

    if (!opt || !opt->val) return FALSE;

    /* return if no explicit offsets in the MetaModes option */

    if (!find_metamode_offset(opt->val, NULL)) return FALSE;

    if (old_metamodes) *old_metamodes = nvstrdup(opt->val);

    /*
     * if we get this far, there are offsets in the MetaModes string;
     * build a new string without the offsets
     */

    new_string = nvstrdup(opt->val);

    o = start = opt->val;
    n = new_string;

    while (1) {

        tmp = find_metamode_offset(start, &end);

        if (tmp) *tmp = '\0';

        while (*o) *n++ = *o++;

        *n = '\0';

        if (!tmp) break;

        o = start = end;
    }

    nvfree(opt->val);

    opt->val = new_string;

    if (new_metamodes) *new_metamodes = nvstrdup(opt->val);

    return TRUE;

} /* remove_metamode_offsets() */



/*
 * update_display_options() - update the Display SubSection options
 */

static void update_display_options(Options *op, XConfigScreenPtr screen)
{
    XConfigDisplayPtr display;
    int i;
    
    /* update the mode list, based on what we have on the commandline */
    
    for (display = screen->displays; display; display = display->next) {

        /*
         * if virtual.[xy] are less than 0, then clear the virtual
         * screen size; if they are greater than 0, assign the virtual
         * screen size; if they are 0, leave the virtual screen size
         * alone
         */

        if ((op->virtual.x < 0) || (op->virtual.y < 0)) {
            display->virtualX = display->virtualY = 0;
        } else if (op->virtual.x || op->virtual.y) {
            display->virtualX = op->virtual.x;
            display->virtualY = op->virtual.y;
        }
        
        for (i = 0; i < op->remove_modes.n; i++) {
            xconfigRemoveMode(&display->modes, op->remove_modes.t[i]);
        }
        for (i = 0; i < op->add_modes.n; i++) {
            xconfigAddMode(&display->modes, op->add_modes.t[i]);
        }
        if (op->add_modes_list.n) {
            int mode_list_size = op->add_modes_list.n;

            xconfigFreeModeList(&display->modes);
            display->modes = NULL;

            /*
             * xconfigAddMode() prepends, rather than appends, so add the
             * modes in reverse order
             */

            for (i = 0; i < op->add_modes_list.n; i++) {
                xconfigAddMode(&display->modes,
                               op->add_modes_list.t[mode_list_size-i-1]);
            }
        }
        
        /* XXX should we sort the mode list? */

        /*
         * XXX should we update the mode list with what we can get
         * through libnvidia-cfg?
         */
    }
    
} /* update_display_options() */



/*
 * update_options() - update the X Config options, based on the
 * command line arguments.
 */

void update_options(Options *op, XConfigScreenPtr screen)
{
    int i;
    const NvidiaXConfigOption *o;
    char *val;
    char scratch[8];

    /* update any boolean options specified on the commandline */

    for (i = 0; i < XCONFIG_BOOL_OPTION_COUNT; i++) {
        if (GET_BOOL_OPTION(op->boolean_options, i)) {
            
            /*
             * SEPARATE_X_SCREENS_BOOL_OPTION, XINERAMA_BOOL_OPTION,
             * COMPOSITE_BOOL_OPTION, and PRESERVE_BUSID_BOOL_OPTION
             * are handled separately
             */

            if (i == SEPARATE_X_SCREENS_BOOL_OPTION) continue;
            if (i == XINERAMA_BOOL_OPTION) continue;
            if (i == COMPOSITE_BOOL_OPTION) continue;
            if (i == PRESERVE_BUSID_BOOL_OPTION) continue;
            
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
    
    /* update the Display SubSection options */
    
    update_display_options(op, screen);

    /* add the nvagp option */
    
    if (op->nvagp != -1) {
        remove_option(screen, "nvagp");
        if (op->nvagp != -2) {
            snprintf(scratch, 8, "%d", op->nvagp);
            set_option_value(screen, "NvAGP", scratch);
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

    /* add the MultiGPU option */

    if (op->multigpu) {
        remove_option(screen, "MultiGPU");
        if (op->multigpu != NV_DISABLE_STRING_OPTION) {
            set_option_value(screen, "MultiGPU", op->multigpu);
        }
    }

    /* add the SLI option */

    if (op->sli) {
        remove_option(screen, "SLI");
        if (op->sli != NV_DISABLE_STRING_OPTION) {
            set_option_value(screen, "SLI", op->sli);
        }
    }

    /* add the metamodes option */

    if (op->metamodes_str) {
        remove_option(screen, "MetaModes");
        if (op->metamodes_str != NV_DISABLE_STRING_OPTION) {
            set_option_value(screen, "MetaModes", op->metamodes_str);
        }
    }

    /* add the rotate option */

    if (op->rotate) {
        remove_option(screen, "Rotate");
        if (op->rotate != NV_DISABLE_STRING_OPTION) {
            set_option_value(screen, "Rotate", op->rotate);
        }
    }

    /* add acpid socket path option*/
 
    if (op->acpid_socket_path) {
        remove_option(screen, "AcpidSocketPath");
        if (op->acpid_socket_path != NV_DISABLE_STRING_OPTION) {
            set_option_value(screen, "AcpidSocketPath", op->acpid_socket_path);
        }
    }
        

    /* add the twinview xinerama info order option */
    
    if (op->twinview_xinerama_info_order) {
        remove_option(screen, "TwinViewXineramaInfoOrder");
        if (op->twinview_xinerama_info_order != NV_DISABLE_STRING_OPTION) {
            set_option_value(screen, "TwinViewXineramaInfoOrder",
                             op->twinview_xinerama_info_order);
        }
    }

    /* add the twinview orientation option */
    
    if (op->twinview_orientation) {
        remove_option(screen, "TwinViewOrientation");
        if (op->twinview_orientation != NV_DISABLE_STRING_OPTION) {
            char *old_metamodes, *new_metamodes;
            set_option_value(screen, "TwinViewOrientation",
                             op->twinview_orientation);
            if (remove_metamode_offsets(screen,
                                        &old_metamodes, &new_metamodes)) {
                fmtmsg("");
                fmtwarn("The MetaModes option contained explicit offsets, "
                        "which would have overridden the specified "
                        "TwinViewOrientation; in order to honor the "
                        "requested TwinViewOrientation, the explicit offsets "
                        "have been removed from the MetaModes option.\n\n"
                        "Old MetaModes option: \"%s\"\n"
                        "New MetaModes option: \"%s\".",
                        old_metamodes, new_metamodes);
                fmtmsg("");
                nvfree(old_metamodes);
                nvfree(new_metamodes);
            }
        }
    }
    
    /* add the LogoPath option */

    if (op->logo_path) {
        remove_option(screen, "LogoPath");
        if (op->logo_path != NV_DISABLE_STRING_OPTION) {
            set_option_value(screen, "LogoPath", op->logo_path);
        }
    }
   
    /* add the UseDisplayDevice option */
 
    if (op->use_display_device) {
        remove_option(screen, "UseDisplayDevice");
        if (op->use_display_device != NV_DISABLE_STRING_OPTION) {
            set_option_value(screen, "UseDisplayDevice",
                             op->use_display_device);
        }
    }

    /* add the CustomEDID option */

    if (op->custom_edid) {
        remove_option(screen, "CustomEDID");
        if (op->custom_edid != NV_DISABLE_STRING_OPTION) {
            set_option_value(screen, "CustomEDID", op->custom_edid);
        }
    }

    /* add the TVStandard option */

    if (op->tv_standard) {
        remove_option(screen, "TVStandard");
        if (op->tv_standard != NV_DISABLE_STRING_OPTION) {
           set_option_value(screen, "TVStandard", op->tv_standard);
        }
    }

    /* add the TVOutFormat option */

    if (op->tv_out_format) {
        remove_option(screen, "TVOutFormat");
        if (op->tv_out_format != NV_DISABLE_STRING_OPTION) {
           set_option_value(screen, "TVOutFormat", op->tv_out_format);
        }
    }

    /* add the TVOverScan option */

    if (op->tv_over_scan != -1.0) {
        remove_option(screen, "TVOverScan");
        if (op->tv_over_scan != -2.0) {
            snprintf(scratch, 8, "%f", op->tv_over_scan);
            set_option_value(screen, "TVOverScan", scratch);
        }
    }

    /* add the Coolbits option */

    if (op->cool_bits != -1) {
        remove_option(screen, "Coolbits");
        if (op->cool_bits != -2) {
            snprintf(scratch, 8, "%d", op->cool_bits);
            set_option_value(screen, "Coolbits", scratch);
        }
    }

    /* add the ConnectedMonitor option */

    if (op->connected_monitor) {
        remove_option(screen, "ConnectedMonitor");
        if (op->connected_monitor != NV_DISABLE_STRING_OPTION) {
            set_option_value(screen, "ConnectedMonitor", op->connected_monitor);
        }
    }

    if (op->registry_dwords) {
        remove_option(screen, "RegistryDwords");
        if (op->registry_dwords != NV_DISABLE_STRING_OPTION) {
            set_option_value(screen, "RegistryDwords", op->registry_dwords);
        }
    }

    /* add the ColorSpace option */

    if (op->color_space) {
        remove_option(screen, "ColorSpace");
        if (op->color_space != NV_DISABLE_STRING_OPTION) {
            set_option_value(screen, "ColorSpace", op->color_space);
        }
    }

    if (op->color_range) {
        remove_option(screen, "ColorRange");
        if (op->color_range != NV_DISABLE_STRING_OPTION) {
            set_option_value(screen, "ColorRange", op->color_range);
        }
    }

    /* add the flatpanel properties option */

    if (op->flatpanel_properties) {
        remove_option(screen, "FlatPanelProperties");
        if (op->flatpanel_properties != NV_DISABLE_STRING_OPTION) {
            set_option_value(screen, "FlatPanelProperties",
                             op->flatpanel_properties);
        }
    }

    /* add the 3DVisionUSBPath option */
    if (op->nvidia_3dvision_usb_path) {
        remove_option(screen, "3DVisionUSBPath");
        if (op->nvidia_3dvision_usb_path != NV_DISABLE_STRING_OPTION) {
            set_option_value(screen, "3DVisionUSBPath", op->nvidia_3dvision_usb_path);
        }
    }

    /* add the 3DVisionProConfigFile option */
    if (op->nvidia_3dvisionpro_config_file) {
        remove_option(screen, "3DVisionProConfigFile");
        if (op->nvidia_3dvisionpro_config_file != NV_DISABLE_STRING_OPTION) {
            set_option_value(screen, "3DVisionProConfigFile", op->nvidia_3dvisionpro_config_file);
        }
    }

    /* add the 3DVisionDisplayType option */

    if (op->nvidia_3dvision_display_type != -1) {
        remove_option(screen, "3DVisionDisplayType");
        if (op->nvidia_3dvision_display_type != -2) {
            snprintf(scratch, 8, "%d", op->nvidia_3dvision_display_type);
            set_option_value(screen, "3DVisionDisplayType", scratch);
        }
    }

} /* update_options() */
