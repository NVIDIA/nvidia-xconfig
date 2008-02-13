/*
 * Options table; the fields are:
 *
 * name - this is the long option name
 *
 * shortname - this is the one character short option name
 *
 * flags - bitmask; possible values are NVGETOPT_HAS_ARGUMENT and
 * NVGETOPT_IS_BOOLEAN
 *
 * description - text for use by print_help() to describe the option
 */

#define SCREEN_OPTION                       1
#define LAYOUT_OPTION                       2
#define X_PREFIX_OPTION                     3
#define KEYBOARD_OPTION                     5
#define KEYBOARD_LIST_OPTION                6
#define KEYBOARD_DRIVER_OPTION              7
#define MOUSE_OPTION                        8
#define FORCE_GENERATE_OPTION               9
#define MOUSE_LIST_OPTION                   10
#define MODE_OPTION                         11
#define NVIDIA_CFG_PATH_OPTION              12
#define NVAGP_OPTION                        13
#define SLI_OPTION                          14
#define DISABLE_SCF_OPTION                  15
#define DIGITAL_VIBRANCE_OPTION             16
#define TRANSPARENT_INDEX_OPTION            17
#define STEREO_OPTION                       18
#define ROTATE_OPTION                       19

#define XCONFIG_OPTION_START        128

/*
 * The OPTION_HELP_ALWAYS flag is or'ed into the nvgetopts flags, but
 * is used by print_help() to know whether to print the help
 * information for that option all the time, or only when advanced
 * help is requested.
 */

#define OPTION_HELP_ALWAYS          0x8000

static const NVGetoptOption __options[] = {
    /* These options are printed by "nvidia-xconfig --help" */

    { "xconfig", 'c', NVGETOPT_HAS_ARGUMENT | OPTION_HELP_ALWAYS,
      "Use [XCONFIG] as the input X config file; if this option is not "
      "specified, then the same search path used by the X server will be "
      "used to find the X configuration file." },

    { "output-xconfig", 'o', NVGETOPT_HAS_ARGUMENT | OPTION_HELP_ALWAYS,
      "Use [OUTPUT-XCONFIG] as the output X configuration file; if this "
      "option is not specified, then the input X configuration filename will "
      "also be used as the output filename." },

    { "silent", 's', OPTION_HELP_ALWAYS,
      "Run silently; no messages will be printed to stdout, except for "
      "warning and error messages to stderr." },

    { "tree", 't',  OPTION_HELP_ALWAYS,
      "Read the X configuration file, print to stdout the X "
      "configuration data in a tree format, and exit." },

    { "version", 'v', OPTION_HELP_ALWAYS,
      "Print the nvidia-xconfig version and exit." },

    { "help", 'h', OPTION_HELP_ALWAYS, "Print usage information for the "
      "common commandline options and exit." },

    { "advanced-help", 'A', OPTION_HELP_ALWAYS, "Print usage information "
      "for the common commandline options as well as the advanced options, "
      "and then exit." },

    /* These options are only printed by "nvidia-xconfig --advanced-help" */

    { "allow-ddcci", XCONFIG_OPTION_START + ALLOW_DDCCI_OPTION,
      NVGETOPT_IS_BOOLEAN, "Enables or disables DDC/CI support in the "
      "NV-CONTROL X extension." },

    { "allow-dfp-stereo",
      XCONFIG_OPTION_START + ALLOW_DFP_STEREO_OPTION, NVGETOPT_IS_BOOLEAN,
      "Enable or disable the \"AllowDFPStereo\" X configuration option." },

    { "allow-glx-with-composite",
      XCONFIG_OPTION_START + ALLOW_GLX_WITH_COMPOSITE_OPTION,
      NVGETOPT_IS_BOOLEAN, "Enable or disable the \"AllowGLXWithComposite\" "
      "X configuration option." },

    { "bandwidth-test",
      XCONFIG_OPTION_START + NO_BANDWIDTH_TEST_OPTION, NVGETOPT_IS_BOOLEAN,
      "Disable or enable the \"NoBandWidthTest\" X configuration option." },

    { "dac-8bit", XCONFIG_OPTION_START + DAC_8BIT_OPTION, NVGETOPT_IS_BOOLEAN,
      "Most Quadro parts by default use a 10 bit color look up table (LUT) "
      "by default; setting this option to TRUE forces these graphics chips "
      "to use an 8 bit (LUT)." },
    
    { "ddc",
      XCONFIG_OPTION_START + IGNORE_EDID_OPTION, NVGETOPT_IS_BOOLEAN,
      "Synonym for \"ignore-edid\"" },

    { "depth", 'd', NVGETOPT_HAS_ARGUMENT,
      "Set the default depth to [DEPTH]; valid values are 8, 15, 16 and 24." },

    { "digital-vibrance", DIGITAL_VIBRANCE_OPTION,
      NVGETOPT_IS_INTEGER | NVGETOPT_HAS_ARGUMENT,
      "Enables digital vibrance control.  Valid values are 0-255." },

    { "enable-all-gpus", 'a', 0,
      "Configure an X screen on every GPU in the system." },
    
    { "exact-mode-timings-dvi",
      XCONFIG_OPTION_START + EXACT_MODE_TIMINGS_DVI_OPTION,
      NVGETOPT_IS_BOOLEAN, "Forces the initialization of the X server with "
      "the exact timings specified in the ModeLine." },

    { "flip", XCONFIG_OPTION_START + NOFLIP_OPTION, NVGETOPT_IS_BOOLEAN,
      "Enable or disable OpenGL flipping" },

    { "force-generate", FORCE_GENERATE_OPTION, 0,
      "Force generation of a new X config file, ignoring any existing "
      "system X config file.  This is not typically recommended, as things "
      "like the mouse protocol, keyboard layout, font paths, etc, are setup "
      "by your Unix distribution.  While nvidia-xconfig can attempt to "
      "infer these values, it is best to use your Unix distribution's "
      "X config file for the basis of anything that nvidia-xconfig creates." },

    { "force-stereo-flipping",
      XCONFIG_OPTION_START + FORCE_STEREO_FLIPPING_OPTION, NVGETOPT_IS_BOOLEAN,
      "Normally, stereo flipping is only performed when a stereo drawable is "
      "visible. This option forces stereo flipping even when no stereo "
      "drawables are visible." },

    { "ignore-edid",
      XCONFIG_OPTION_START + IGNORE_EDID_OPTION, NVGETOPT_IS_BOOLEAN,
      "Disable or enable probing of EDID (Extended Display Identification "
      "Data) from your monitor." },

    { "use-edid-freqs",
      XCONFIG_OPTION_START + USE_EDID_FREQS_OPTION, NVGETOPT_IS_BOOLEAN,
      "Allow or disallow the X server to use the HorizSync and VertRefresh "
      "ranges given in a display device's EDID, if any.  EDID provided "
      "range information will override the HorizSync and VertRefresh ranges "
      "specified in the Monitor section." },

    { "keyboard", KEYBOARD_OPTION, NVGETOPT_HAS_ARGUMENT,
      "When generating a new X configuration file (which happens when no "
      "system X configuration file can be found, or the '--force-generate' "
      "option is specified), use [KEYBOARD] as the keyboard type, rather "
      "than attempting to probe the system for the keyboard type.  "
      "For a list of possible keyboard types, see the '--keyboard-list' "
      "option." },

    { "keyboard-driver", KEYBOARD_DRIVER_OPTION, NVGETOPT_HAS_ARGUMENT,
      "In most cases nvidia-xconfig can automatically determine the correct "
      "keyboard driver to use (either 'kbd' or 'keyboard'). Use this "
      "option to override what nvidia-xconfig detects. Typically, if you are "
      "using an X.org X server, use 'kdb'; if you are using an XFree86 X "
      "server, use 'keyboard'." },

    { "keyboard-list", KEYBOARD_LIST_OPTION, 0,
      "Print to stdout the available keyboard types recognized by the "
      "'--keyboard' option, and then exit." },

    { "layout", LAYOUT_OPTION, NVGETOPT_HAS_ARGUMENT,
      "The nvidia-xconfig utility operates on a Server Layout within the X "
      "configuration file.  If this option is specified, the layout named "
      "[LAYOUT] in the X configuration file will be used.  If this option is "
      "not specified, the first Server Layout in the X configuration "
      "file is used." },

    { "screen", SCREEN_OPTION, NVGETOPT_HAS_ARGUMENT,
      "The nvidia-xconfig utility operates on one or more screens within a "
      "Server Layout in the X configuration file.  If this option is "
      "specified, the screen named [SCREEN] in the X configuration file will "
      "be used.  If this option is not specified, all screens within the "
      "selected Server Layout in the X configuration file "
      "will be used used." },

    { "load-kernel-module",
      XCONFIG_OPTION_START + LOAD_KERNEL_MODULE_OPTION, NVGETOPT_IS_BOOLEAN,
      "Allow or disallow NVIDIA Linux X driver module to load the NVIDIA "
      "Linux kernel module automatically."},
    
    { "logo",
      XCONFIG_OPTION_START + NOLOGO_OPTION, NVGETOPT_IS_BOOLEAN,
      "Disable or enable the \"NoLogo\" X configuration option." },

    { "mode", MODE_OPTION, NVGETOPT_IS_BOOLEAN | NVGETOPT_HAS_ARGUMENT,
      "Adds or removes the specified mode from the mode list." },

    { "mouse", MOUSE_OPTION, NVGETOPT_HAS_ARGUMENT,
      "When generating a new X configuration file (which happens when no "
      "system X configuration file can be found, or the '--force-generate' "
      "option is specified), use [MOUSE] as the mouse type, rather than "
      "attempting to probe the system for the mouse type.  For a list of "
      "possible mouse types, see the '--mouse-list' option." },

    { "mouse-list", MOUSE_LIST_OPTION, 0,
      "Print to stdout the available mouse types recognized by the "
      "'--mouse' option, and then exit." },

    { "multisample-compatibility",
      XCONFIG_OPTION_START + MULTISAMPLE_COMPATIBILITY_OPTION,
      NVGETOPT_IS_BOOLEAN, "Enable or disable the use of separate front and "
      "back multisample buffers." },

    { "nvagp", NVAGP_OPTION, NVGETOPT_HAS_ARGUMENT,
      "Set the NvAGP X config option value.  Possible values are 0 (no AGP), "
      "1 (NVIDIA's AGP), 2 (AGPGART), 3 (try AGPGART, then try NVIDIA's AGP); "
      "these values can also be specified as 'none', 'nvagp', 'agpgart', or "
      "'any'." },

    { "nvidia-cfg-path", NVIDIA_CFG_PATH_OPTION, NVGETOPT_HAS_ARGUMENT,
      "The nvidia-cfg library is used to communicate with the NVIDIA kernel "
      "module to query basic properties of every GPU in the system.  This "
      "library is typically only used by nvidia-xconfig when configuring "
      "multiple X screens.  This option tells nvidia-xconfig where to look "
      "for this library (in case it cannot find it on its own).  This option "
      "should normally not be needed." },

    { "only-one-x-screen", '1', 0,
      "Disable all but one X screen." },

    { "overlay",
      XCONFIG_OPTION_START + OVERLAY_OPTION, NVGETOPT_IS_BOOLEAN,
      "Enable or disable the \"Overlay\" X configuration option." },

    { "cioverlay",
      XCONFIG_OPTION_START + CIOVERLAY_OPTION, NVGETOPT_IS_BOOLEAN,
      "Enable or disable the color index overlay." },

    { "transparent-index", TRANSPARENT_INDEX_OPTION,
      NVGETOPT_IS_INTEGER | NVGETOPT_HAS_ARGUMENT,
      "Pixel to use as transparent when using color index overlays.  "
      "Valid values are 0-255."},

    { "overlay-default-visual",
      XCONFIG_OPTION_START + OVERLAY_DEFAULT_VISUAL_OPTION,
      NVGETOPT_IS_BOOLEAN, "Enable or disable the \"OverlayDefaultVisual\" "
      "X configuration option." },

    { "post-tree", 'T', 0,
      "Like the '--tree' option, but goes through the full process of "
      "applying any user requested updates to the X configuration, before "
      "printing the final configuration to stdout in a tree format.  "
      "Effectively, this option just causes the configuration to be printed "
      "to stdout as a tree instead of writing the results to file." },

    { "power-connector-check",
      XCONFIG_OPTION_START + NO_POWER_CONNECTOR_CHECK_OPTION,
      NVGETOPT_IS_BOOLEAN, "Disable or enable the \"NoPowerConnectorCheck\" "
      "X configuration option." },

    { "randr-rotation",
      XCONFIG_OPTION_START + RANDR_ROTATION_OPTION, NVGETOPT_IS_BOOLEAN,
      "Enable or disable the \"RandRRotation\" X configuration option." },

    { "rotate",
      ROTATE_OPTION, NVGETOPT_HAS_ARGUMENT,
      "Enable or disable the \"Rotate\" X configuration option.  Valid values "
      "are 'normal', 'left', 'inverted', and 'right'." },

    { "render-accel",
      XCONFIG_OPTION_START + RENDER_ACCEL_OPTION, NVGETOPT_IS_BOOLEAN,
      "Enable or disable the \"RenderAccel\" X configuration option." },

    { "render-extension",
      XCONFIG_OPTION_START + NO_RENDER_EXTENSION_OPTION, NVGETOPT_IS_BOOLEAN,
      "Disable or enable the \"NoRenderExtension\" X configuration option." },

    { "separate-x-screens",
      XCONFIG_OPTION_START + SEPARATE_X_SCREENS_OPTION, NVGETOPT_IS_BOOLEAN,
      "A GPU that supports multiple simultaneous display devices can either "
      "drive these display devices in TwinView, or as separate X screens.  "
      "When the '--separate-x-screens' option is specified, each GPU on which "
      "an X screen is currently configured will be updated to have two X "
      "screens configured.  The '--no-separate-x-screens' option will remove "
      "the second configured X screen on each GPU.  Please see the NVIDIA "
      "README description of \"Separate X Screens on One GPU\" for further "
      "details." },

    { "sli", SLI_OPTION, NVGETOPT_HAS_ARGUMENT,
      "Enable or disable SLI.  Valid options are 'Off', 'Auto', 'AFR', 'SFR', "
      "and 'SLIAA'." },

    { "stereo", STEREO_OPTION, NVGETOPT_IS_INTEGER | NVGETOPT_HAS_ARGUMENT,
      "Enable/Disable the stereo mode.  Valid options are: 1 (DCC glasses), "
      "2 (Blueline glasses), 3 (Onboard stereo), 4 (TwinView clone mode "
      "stereo), 5 (SeeReal digital flat panel), 6 (Sharp3D digital flat "
      "panel)." },

    { "twinview", XCONFIG_OPTION_START + TWINVIEW_OPTION, NVGETOPT_IS_BOOLEAN,
      "Enable or disable TwinView." },

    { "twinview-xinerama-info",
      XCONFIG_OPTION_START + NO_TWINVIEW_XINERAMA_INFO_OPTION,
      NVGETOPT_IS_BOOLEAN,
      "Prohibits providing Xinerama information when in TwinView." },

    { "ubb",
      XCONFIG_OPTION_START + UBB_OPTION, NVGETOPT_IS_BOOLEAN,
      "Enable or disable the \"UBB\" X configuration option." },

    { "use-int10-module",
      XCONFIG_OPTION_START + USE_INT10_MODULE_OPTION, NVGETOPT_IS_BOOLEAN,
      "Enable use of the X Int10 module to soft-boot all secondary cards, "
      "rather than POSTing the cards through the NVIDIA kernel module." },

    { "x-prefix", X_PREFIX_OPTION, NVGETOPT_HAS_ARGUMENT,
      "The X installation prefix; the default is /usr/X11R6/.  Only "
      "under rare circumstances should this option be needed." },

    { "xinerama", XCONFIG_OPTION_START + XINERAMA_OPTION, NVGETOPT_IS_BOOLEAN,
      "Enable or disable Xinerama." },

    { "xvmc-uses-textures",
      XCONFIG_OPTION_START + XVMC_USES_TEXTURES_OPTION, NVGETOPT_IS_BOOLEAN,
      "Forces XvMC to use the 3D engine for XvMCPutSurface requests rather "
      "than the video overlay." },
      
#if defined(NV_SUNOS)
    { "disable-scf", DISABLE_SCF_OPTION, 0,
      "On Solaris, nvidia-xconfig updates the service configuration repository "
      "with the default depth being set in the X configuration file. " 
      "The property 'default_depth' of the group 'options' in the "
      "selection 'application/x11/x11-server' is set to the default depth. "
      "Use this option to disable the service configuration repository update." },
#endif
     
    { NULL, 0 ,  0, NULL },
};
