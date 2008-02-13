/*
 * Options table; the fields are:
 *
 * name - this is the long option name
 *
 * shortname - this is the one character short option name
 *
 * flags - bitmask; see NVGETOPT_ constants in nvgetopt.h
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
#define MODE_LIST_OPTION                    12
#define REMOVE_MODE_OPTION                  13
#define NVIDIA_CFG_PATH_OPTION              14
#define NVAGP_OPTION                        15
#define SLI_OPTION                          16
#define DISABLE_SCF_OPTION                  17
#define TRANSPARENT_INDEX_OPTION            18
#define STEREO_OPTION                       19
#define ROTATE_OPTION                       20
#define QUERY_GPU_INFO_OPTION               21
#define EXTRACT_EDIDS_OUTPUT_FILE_OPTION    22
#define MULTI_GPU_OPTION                    23
#define TWINVIEW_XINERAMA_INFO_ORDER_OPTION 24
#define LOGO_PATH_OPTION                    25
#define TWINVIEW_ORIENTATION_OPTION         26
#define VIRTUAL_OPTION                      27
#define USE_DISPLAY_DEVICE_OPTION           28
#define CUSTOM_EDID_OPTION                  29
#define TV_STANDARD_OPTION                  30
#define TV_OUT_FORMAT_OPTION                31
#define TV_OVER_SCAN_OPTION                 32
#define COOL_BITS_OPTION                    33

/*
 * To add a boolean option to nvidia-xconfig:
 *
 * 1) Add the definition of the constant to the "Boolean options" list
 *    in nvidia-xconfig.h
 *
 * 2) add an entry in the below (alphabetized) __options[] table; for
 *    the second field in the __options[] table, specify
 *    XCONFIG_BOOL_VAL(<your-new-constant>)
 *
 * 3) add an entry to the __options[] table at the top of options.c
 *    with the constant and the option name as it should appear in the X
 *    config file.
 *
 * nvidia-xconfig.c:parse_commandline() will record in
 * op->boolean_options whether the commandline option was specified,
 * and will record in op->boolean_option_values whether the option was
 * specified to be true or false.  options.c:update_options() will
 * apply your boolean option to the X config file.
 */


#define XCONFIG_BOOL_OPTION_START        128


/*
 * The XCONFIG_BOOL_VAL() macro packs boolean options into the val
 * field of the __option[] table; these are above 128, so that
 * isalpha(3) returns FALSE for them.
 */

#define XCONFIG_BOOL_VAL(x) (XCONFIG_BOOL_OPTION_START + (x))

/*
 * The OPTION_HELP_ALWAYS flag is or'ed into the nvgetopts flags, but
 * is only used by print_help() to know whether to print the help
 * information for that option all the time, or only when advanced
 * help is requested.
 */

#define OPTION_HELP_ALWAYS          0x8000

static const NVGetoptOption __options[] = {
    /* These options are printed by "nvidia-xconfig --help" */

    { "xconfig", 'c', NVGETOPT_STRING_ARGUMENT | OPTION_HELP_ALWAYS, NULL,
      "Use [XCONFIG] as the input X config file; if this option is not "
      "specified, then the same search path used by the X server will be "
      "used to find the X configuration file." },

    { "output-xconfig", 'o',
      NVGETOPT_STRING_ARGUMENT | OPTION_HELP_ALWAYS, NULL,
      "Use [OUTPUT-XCONFIG] as the output X configuration file; if this "
      "option is not specified, then the input X configuration filename will "
      "also be used as the output X configuration filename." },

    { "silent", 's', OPTION_HELP_ALWAYS, NULL,
      "Run silently; no messages will be printed to stdout, except for "
      "warning and error messages to stderr." },

    { "tree", 't',  OPTION_HELP_ALWAYS, NULL,
      "Read the X configuration file, print to stdout the X "
      "configuration data in a tree format, and exit." },

    { "version", 'v', OPTION_HELP_ALWAYS, NULL,
      "Print the nvidia-xconfig version and exit." },

    { "help", 'h', OPTION_HELP_ALWAYS,  NULL,
      "Print usage information for the common commandline options and exit." },

    { "advanced-help", 'A', OPTION_HELP_ALWAYS,  NULL,
      "Print usage information for the common commandline options as well "
      "as the advanced options, and then exit." },

    /* These options are only printed by "nvidia-xconfig --advanced-help" */

    { "add-argb-glx-visuals",
      XCONFIG_BOOL_VAL(ADD_ARGB_GLX_VISUALS_BOOL_OPTION),
      NVGETOPT_IS_BOOLEAN,  NULL,
      "Enables or disables support for OpenGL rendering into 32-bit ARGB "
      "windows and pixmaps." },

    { "allow-ddcci", XCONFIG_BOOL_VAL(ALLOW_DDCCI_BOOL_OPTION),
      NVGETOPT_IS_BOOLEAN,  NULL, "Enables or disables DDC/CI support in the "
      "NV-CONTROL X extension." },

    { "allow-dfp-stereo", XCONFIG_BOOL_VAL(ALLOW_DFP_STEREO_BOOL_OPTION),
      NVGETOPT_IS_BOOLEAN, NULL,
      "Enable or disable the \"AllowDFPStereo\" X configuration option." },

    { "allow-glx-with-composite",
      XCONFIG_BOOL_VAL(ALLOW_GLX_WITH_COMPOSITE_BOOL_OPTION),
      NVGETOPT_IS_BOOLEAN, NULL,
      "Enable or disable the \"AllowGLXWithComposite\" X configuration "
      "option." },

    { "bandwidth-test", XCONFIG_BOOL_VAL(NO_BANDWIDTH_TEST_BOOL_OPTION),
      NVGETOPT_IS_BOOLEAN, NULL,
      "Disable or enable the \"NoBandWidthTest\" X configuration option." },

    { "cool-bits", COOL_BITS_OPTION,
      NVGETOPT_INTEGER_ARGUMENT | NVGETOPT_ALLOW_DISABLE, NULL,
      "Enable or disable the \"Coolbits\" X configuration option. Setting this "
      "option will enable support in the NV-CONTROL X extension for manipulating "
      "GPU clock settings. Default value is 0.\n"
      "WARNING: this may cause system damage and void warranties." },

    { "composite",
      XCONFIG_BOOL_VAL(COMPOSITE_BOOL_OPTION), NVGETOPT_IS_BOOLEAN, NULL,
      "Enable or disable the \"Composite\" X extension." },

    { "constant-dpi",
      XCONFIG_BOOL_VAL(CONSTANT_DPI_BOOL_OPTION), NVGETOPT_IS_BOOLEAN, NULL,
      "Enable or disable the \"ConstantDPI\" X configuration option, "
      "which controls whether the NVIDIA X driver maintains a constant "
      "dots per inch (DPI) value by recomputing the reported size in "
      "millimeters of the X screen when XRandR changes the size in pixels "
      "of the X screen." },

    { "custom-edid", CUSTOM_EDID_OPTION,
      NVGETOPT_STRING_ARGUMENT | NVGETOPT_ALLOW_DISABLE, "CUSTOM-EDID",
      "Enable or disable the  \"CustomEDID\" X configuration option; "
      "setting this option forces the X driver to use the EDID specified."
      "This option is a semicolon-separated list of pairs of display device names "
      "and filename pairs; e.g \"CRT-0:\\tmp\\edid.bin\". Note that a display "
      "device name must always be specified even if only one EDID is"
      " specified. " },

    { "dac-8bit", XCONFIG_BOOL_VAL(DAC_8BIT_BOOL_OPTION),
      NVGETOPT_IS_BOOLEAN, NULL,
      "Most Quadro parts by default use a 10 bit color look up table (LUT) "
      "by default; setting this option to TRUE forces these graphics chips "
      "to use an 8 bit (LUT)." },
    
    { "depth", 'd', NVGETOPT_INTEGER_ARGUMENT, NULL,
      "Set the default depth to [DEPTH]; valid values for [DEPTH] are "
      "8, 15, 16 and 24." },

    { "disable-glx-root-clipping",
      XCONFIG_BOOL_VAL(DISABLE_GLX_ROOT_CLIPPING_BOOL_OPTION),
      NVGETOPT_IS_BOOLEAN, NULL, "Disable or enable clipping OpenGL rendering "
      "to the root window via the \"DisableGLXRootClipping\" "
      "X configuration option." },

    { "damage-events",
      XCONFIG_BOOL_VAL(DAMAGE_EVENTS_BOOL_OPTION),
      NVGETOPT_IS_BOOLEAN, NULL, "Use OS-level events to notify the X server "
      "when a direct-rendering client has performed rendering that needs to be "
      "composited to the screen.  Improves performance when using GLX with the "
      "composite extension." },
    
#if defined(NV_SUNOS)
    { "disable-scf", DISABLE_SCF_OPTION, 0, NULL,
      "On Solaris, nvidia-xconfig updates the service configuration "
      "repository with the default depth being set in the X configuration "
      "file.  The property 'default_depth' of the group 'options' in the "
      "selection 'application/x11/x11-server' is set to the default depth. "
      "Use this option to disable the service configuration repository "
      "update." },
#endif

    { "dynamic-twinview", XCONFIG_BOOL_VAL(DYNAMIC_TWINVIEW_BOOL_OPTION),
      NVGETOPT_IS_BOOLEAN, NULL,
      "Enable or disable support for dynamically configuring TwinView." },
    
    { "enable-all-gpus", 'a', 0, NULL,
      "Configure an X screen on every GPU in the system." },
    
    { "exact-mode-timings-dvi",
      XCONFIG_BOOL_VAL(EXACT_MODE_TIMINGS_DVI_BOOL_OPTION),
      NVGETOPT_IS_BOOLEAN, NULL,
      "Forces the initialization of the X server with "
      "the exact timings specified in the ModeLine." },

    { "extract-edids-from-file", 'E', NVGETOPT_STRING_ARGUMENT, "FILE",
      "Extract any raw EDID byte blocks contained in the specified X "
      "log file [LOG]; raw EDID bytes are printed by the NVIDIA X driver to "
      "the X log as hexidecimal when verbose logging is enabled with the "
      "\"-logverbose 6\" X server commandline option.  Any extracted EDIDs "
      "are then written as binary data to individual files.  These files "
      "can later be used by the NVIDIA X driver through the \"CustomEDID\" "
      "X configuration option." },
    
    { "extract-edids-output-file", 
      EXTRACT_EDIDS_OUTPUT_FILE_OPTION, NVGETOPT_STRING_ARGUMENT, "FILENAME",
      "When the '--extract-edids-from-log' option is used, nvidia-xconfig "
      "writes any extracted EDID to a file, typically \"edid.bin\" in the "
      "current directory.  Use this option to specify an alternate "
      "filename.  Note that nvidia-xconfig, if necessary, will append a "
      "unique number to the EDID filename, to avoid overwriting existing "
      "files (e.g., \"edid.bin.1\" if \"edid.bin\" already exists)." },

    { "flip", XCONFIG_BOOL_VAL(NOFLIP_BOOL_OPTION), NVGETOPT_IS_BOOLEAN, NULL,
      "Enable or disable OpenGL flipping" },

    { "force-generate", FORCE_GENERATE_OPTION, 0, NULL,
      "Force generation of a new X config file, ignoring any existing "
      "system X config file.  This is not typically recommended, as things "
      "like the mouse protocol, keyboard layout, font paths, etc, are setup "
      "by your Unix distribution.  While nvidia-xconfig can attempt to "
      "infer these values, it is best to use your Unix distribution's "
      "X config file for the basis of anything that nvidia-xconfig creates." },

    { "force-stereo-flipping",
      XCONFIG_BOOL_VAL(FORCE_STEREO_FLIPPING_BOOL_OPTION),
      NVGETOPT_IS_BOOLEAN, NULL,
      "Normally, stereo flipping is only performed when a stereo drawable is "
      "visible. This option forces stereo flipping even when no stereo "
      "drawables are visible." },

    { "include-implicit-metamodes",
      XCONFIG_BOOL_VAL(INCLUDE_IMPLICIT_METAMODES_BOOL_OPTION),
      NVGETOPT_IS_BOOLEAN, NULL,
      "Enable or disable the \"IncludeImplicitMetaModes\" X configuration "
      "option." },

    { "keyboard", KEYBOARD_OPTION, NVGETOPT_STRING_ARGUMENT, NULL,
      "When generating a new X configuration file (which happens when no "
      "system X configuration file can be found, or the '--force-generate' "
      "option is specified), use [KEYBOARD] as the keyboard type, rather "
      "than attempting to probe the system for the keyboard type.  "
      "For a list of possible keyboard types, see the '--keyboard-list' "
      "option." },

    { "keyboard-driver", KEYBOARD_DRIVER_OPTION,
      NVGETOPT_STRING_ARGUMENT, "DRIVER",
      "In most cases nvidia-xconfig can automatically determine the correct "
      "keyboard driver to use (either 'kbd' or 'keyboard'). Use this "
      "option to override what nvidia-xconfig detects. Typically, if you are "
      "using an X.Org X server, use 'kdb'; if you are using an XFree86 X "
      "server, use 'keyboard'." },

    { "keyboard-list", KEYBOARD_LIST_OPTION, 0, NULL,
      "Print to stdout the available keyboard types recognized by the "
      "'--keyboard' option, and then exit." },

    { "layout", LAYOUT_OPTION, NVGETOPT_STRING_ARGUMENT, NULL,
      "The nvidia-xconfig utility operates on a Server Layout within the X "
      "configuration file.  If this option is specified, the layout named "
      "[LAYOUT] in the X configuration file will be used.  If this option is "
      "not specified, the first Server Layout in the X configuration "
      "file is used." },

    { "load-kernel-module",
      XCONFIG_BOOL_VAL(LOAD_KERNEL_MODULE_BOOL_OPTION),
      NVGETOPT_IS_BOOLEAN, NULL,
      "Allow or disallow NVIDIA Linux X driver module to load the NVIDIA "
      "Linux kernel module automatically."},
    
    { "logo",
      XCONFIG_BOOL_VAL(NOLOGO_BOOL_OPTION), NVGETOPT_IS_BOOLEAN, NULL,
      "Disable or enable the \"NoLogo\" X configuration option." },

    { "logo-path", LOGO_PATH_OPTION,
      NVGETOPT_STRING_ARGUMENT | NVGETOPT_ALLOW_DISABLE, "PATH",
      "Set the path to the PNG file to be used as the logo splash screen at X "
      "server startup." },

    { "mode",
      MODE_OPTION, NVGETOPT_IS_BOOLEAN | NVGETOPT_STRING_ARGUMENT, NULL,
      "Add the specified mode to the mode list." },

    { "mode-list", MODE_LIST_OPTION, NVGETOPT_STRING_ARGUMENT, "MODELIST",
      "Remove all existing modes from the X configuration's modelist and "
      "add the one(s) specified in the [MODELIST] string." },

    { "remove-mode", REMOVE_MODE_OPTION, NVGETOPT_STRING_ARGUMENT, "MODE",
      "Remove the specified mode from the mode list." },

    { "mouse", MOUSE_OPTION, NVGETOPT_STRING_ARGUMENT, NULL,
      "When generating a new X configuration file (which happens when no "
      "system X configuration file can be found, or the '--force-generate' "
      "option is specified), use [MOUSE] as the mouse type, rather than "
      "attempting to probe the system for the mouse type.  For a list of "
      "possible mouse types, see the '--mouse-list' option." },

    { "mouse-list", MOUSE_LIST_OPTION, 0, NULL,
      "Print to stdout the available mouse types recognized by the "
      "'--mouse' option, and then exit." },

    { "multigpu", MULTI_GPU_OPTION,
      NVGETOPT_STRING_ARGUMENT | NVGETOPT_ALLOW_DISABLE, NULL,
      "Enable or disable MultiGPU.  Valid values for [MULTIGPU] are 'Off', 'On',"
      " 'Auto', 'AFR', 'SFR', 'AA'." },

    { "multisample-compatibility",
      XCONFIG_BOOL_VAL(MULTISAMPLE_COMPATIBILITY_BOOL_OPTION),
      NVGETOPT_IS_BOOLEAN, NULL,
      "Enable or disable the use of separate front and "
      "back multisample buffers." },

    { "nvagp", NVAGP_OPTION,
      NVGETOPT_STRING_ARGUMENT | NVGETOPT_ALLOW_DISABLE, NULL,
      "Set the NvAGP X config option value.  Possible values are 0 (no AGP), "
      "1 (NVIDIA's AGP), 2 (AGPGART), 3 (try AGPGART, then try NVIDIA's AGP); "
      "these values can also be specified as 'none', 'nvagp', 'agpgart', or "
      "'any'." },

    { "nvidia-cfg-path",
      NVIDIA_CFG_PATH_OPTION, NVGETOPT_STRING_ARGUMENT, "PATH",
      "The nvidia-cfg library is used to communicate with the NVIDIA kernel "
      "module to query basic properties of every GPU in the system.  This "
      "library is typically only used by nvidia-xconfig when configuring "
      "multiple X screens.  This option tells nvidia-xconfig where to look "
      "for this library (in case it cannot find it on its own).  This option "
      "should normally not be needed." },

    { "only-one-x-screen", '1', 0,
      "Disable all but one X screen." },

    { "overlay",
      XCONFIG_BOOL_VAL(OVERLAY_BOOL_OPTION), NVGETOPT_IS_BOOLEAN, NULL,
      "Enable or disable the \"Overlay\" X configuration option." },

    { "cioverlay",
      XCONFIG_BOOL_VAL(CIOVERLAY_BOOL_OPTION), NVGETOPT_IS_BOOLEAN, NULL,
      "Enable or disable the color index overlay." },

    { "overlay-default-visual",
      XCONFIG_BOOL_VAL(OVERLAY_DEFAULT_VISUAL_BOOL_OPTION),
      NVGETOPT_IS_BOOLEAN, NULL,
      "Enable or disable the \"OverlayDefaultVisual\" "
      "X configuration option." },

    { "transparent-index", TRANSPARENT_INDEX_OPTION,
      NVGETOPT_INTEGER_ARGUMENT | NVGETOPT_ALLOW_DISABLE, "INDEX",
      "Pixel to use as transparent when using color index overlays.  "
      "Valid values for [TRANSPARENT-INDEX] are 0-255."},

    { "post-tree", 'T', 0, NULL,
      "Like the '--tree' option, but goes through the full process of "
      "applying any user requested updates to the X configuration, before "
      "printing the final configuration to stdout in a tree format.  "
      "Effectively, this option just causes the configuration to be printed "
      "to stdout as a tree instead of writing the results to file." },

    { "power-connector-check",
      XCONFIG_BOOL_VAL(NO_POWER_CONNECTOR_CHECK_BOOL_OPTION),
      NVGETOPT_IS_BOOLEAN, NULL,
      "Disable or enable the \"NoPowerConnectorCheck\" "
      "X configuration option." },

    { "probe-all-gpus", XCONFIG_BOOL_VAL(PROBE_ALL_GPUS_BOOL_OPTION),
      NVGETOPT_IS_BOOLEAN, NULL,
      "Disable or enable the \"ProbeAllGpus\" X configuration option." },
      
      
    { "query-gpu-info", QUERY_GPU_INFO_OPTION, 0, NULL,
      "Print information about all recognized NVIDIA GPUs in the system." },
    
    { "randr-rotation",
      XCONFIG_BOOL_VAL(RANDR_ROTATION_BOOL_OPTION), NVGETOPT_IS_BOOLEAN, NULL,
      "Enable or disable the \"RandRRotation\" X configuration option." },

    { "render-accel",
      XCONFIG_BOOL_VAL(RENDER_ACCEL_BOOL_OPTION), NVGETOPT_IS_BOOLEAN, NULL,
      "Enable or disable the \"RenderAccel\" X configuration option." },

    { "render-extension",
      XCONFIG_BOOL_VAL(NO_RENDER_EXTENSION_BOOL_OPTION),
      NVGETOPT_IS_BOOLEAN, NULL,
      "Disable or enable the \"NoRenderExtension\" X configuration option." },

    { "rotate",
      ROTATE_OPTION, NVGETOPT_STRING_ARGUMENT | NVGETOPT_ALLOW_DISABLE, NULL,
      "Enable or disable the \"Rotate\" X configuration option.  Valid values "
      "for [ROTATE] are 'normal', 'left', 'CCW', 'inverted', "
      "'right', and 'CW'.  Rotation can be disabled " },
 
    { "screen", SCREEN_OPTION, NVGETOPT_STRING_ARGUMENT, NULL,
      "The nvidia-xconfig utility operates on one or more screens within a "
      "Server Layout in the X configuration file.  If this option is "
      "specified, the screen named [SCREEN] in the X configuration file will "
      "be used.  If this option is not specified, all screens within the "
      "selected Server Layout in the X configuration file "
      "will be used used." },

    { "separate-x-screens",
      XCONFIG_BOOL_VAL(SEPARATE_X_SCREENS_BOOL_OPTION),
      NVGETOPT_IS_BOOLEAN, NULL,
      "A GPU that supports multiple simultaneous display devices can either "
      "drive these display devices in TwinView, or as separate X screens.  "
      "When the '--separate-x-screens' option is specified, each GPU on which "
      "an X screen is currently configured will be updated to have two X "
      "screens configured.  The '--no-separate-x-screens' option will remove "
      "the second configured X screen on each GPU.  Please see the NVIDIA "
      "README description of \"Separate X Screens on One GPU\" for further "
      "details." },

    { "sli", SLI_OPTION,
      NVGETOPT_STRING_ARGUMENT | NVGETOPT_ALLOW_DISABLE, NULL,
      "Enable or disable SLI.  Valid values for [SLI] are 'Off', 'On', 'Auto', "
      "'AFR', 'SFR', 'AA', 'AFRofAA'." },

    { "stereo", STEREO_OPTION,
      NVGETOPT_INTEGER_ARGUMENT | NVGETOPT_ALLOW_DISABLE, NULL,
      "Enable or disable the stereo mode.  Valid values for [STEREO] are: 1 "
      "(DCC glasses), 2 (Blueline glasses), 3 (Onboard stereo), 4 (TwinView "
      "clone mode stereo), 5 (SeeReal digital flat panel), 6 (Sharp3D "
      "digital flat panel)." },

    { "tv-standard", TV_STANDARD_OPTION,
      NVGETOPT_STRING_ARGUMENT | NVGETOPT_ALLOW_DISABLE, "TV-STANDARD",
      "Enable or disable the \"TVStandard\" X configuration option. Valid " 
      "values for \"TVStandard\" are: \"PAL-B\", \"PAL-D\", \"PAL-G\", "
      "\"PAL-H\", \"PAL-I\", \"PAL-K1\", \"PAL-M\", \"PAL-N\", \"PAL-NC\", " 
      "\"NTSC-J\", \"NTSC-M\", \"HD480i\", \"HD480p\", \"HD720p\", "
      "\"HD1080i\", \"HD1080p\", \"HD576i\", \"HD576p\"." },
    
    { "tv-out-format", TV_OUT_FORMAT_OPTION,
      NVGETOPT_STRING_ARGUMENT | NVGETOPT_ALLOW_DISABLE, "TV-OUT-FORMAT",
      "Enable or disable the \"TVOutFormat\" X configuration option. Valid "
      "values for \"TVOutFormat\" are: \"SVIDEO\" and \"COMPOSITE\"." },
  
    { "tv-over-scan", TV_OVER_SCAN_OPTION,
      NVGETOPT_DOUBLE_ARGUMENT | NVGETOPT_ALLOW_DISABLE, NULL,
      "Enable or disable the \"TVOverScan\" X configuration option. Valid "
      "values are decimal values in the range 1.0 and 0.0." }, 
          
    { "twinview", XCONFIG_BOOL_VAL(TWINVIEW_BOOL_OPTION),
      NVGETOPT_IS_BOOLEAN, NULL, "Enable or disable TwinView." },

    { "twinview-orientation", TWINVIEW_ORIENTATION_OPTION,
      NVGETOPT_STRING_ARGUMENT | NVGETOPT_ALLOW_DISABLE, "ORIENTATION",
      "Specify the TwinViewOrientation.  Valid values for [ORIENTATION] are: "
      "\"RightOf\" (the default), \"LeftOf\", \"Above\", \"Below\", or "
      "\"Clone\"." },
    
    { "twinview-xinerama-info",
      XCONFIG_BOOL_VAL(NO_TWINVIEW_XINERAMA_INFO_BOOL_OPTION),
      NVGETOPT_IS_BOOLEAN, NULL,
      "Prohibits providing Xinerama information when in TwinView." },

    { "twinview-xinerama-info-order",
      TWINVIEW_XINERAMA_INFO_ORDER_OPTION,
      NVGETOPT_STRING_ARGUMENT | NVGETOPT_ALLOW_DISABLE, NULL,
      "Enable or disable the \"TwinViewXineramaInfoOrder\" X configuration "
      "option.  [TWINVIEW-XINERAMA-INFO-ORDER] is a comma-separated list "
      "of display device names that describe the order in which "
      "TwinViewXineramaInfo should be reported.  E.g., \"CRT, DFP, TV\"." },

    { "ubb",
      XCONFIG_BOOL_VAL(UBB_BOOL_OPTION), NVGETOPT_IS_BOOLEAN, NULL,
      "Enable or disable the \"UBB\" X configuration option." },

    { "use-edid",
      XCONFIG_BOOL_VAL(USE_EDID_BOOL_OPTION), NVGETOPT_IS_BOOLEAN, NULL,
      "Enable or disable use of the EDID (Extended Display Identification "
      "Data) from your display device(s).  The EDID will be used for driver "
      "operations such as building lists of available modes, determining "
      "valid frequency ranges, and computing the DPI (Dots Per Inch).  "
      "This option defaults to TRUE (the NVIDIA X driver will use the EDID, "
      "when available).  It is NOT recommended that you use this option to "
      "globally disable use of the EDID; instead, use '--no-use-edid-freqs' "
      "or '--no-use-edid-dpi' to disable specific uses of the EDID." },

    { "use-edid-dpi",
      XCONFIG_BOOL_VAL(USE_EDID_DPI_BOOL_OPTION), NVGETOPT_IS_BOOLEAN, NULL,
      "Enable or disable use of the physical size information in the display "
      "device's EDID, if any, to compute the DPI (Dots Per Inch) of the X "
      "screen.  This option defaults to TRUE (the NVIDIA X driver uses the "
      "EDID's physical size, when available, to compute the DPI)." },
      
    { "use-edid-freqs",
      XCONFIG_BOOL_VAL(USE_EDID_FREQS_BOOL_OPTION), NVGETOPT_IS_BOOLEAN, NULL,
      "Enable or disable use of the HorizSync and VertRefresh "
      "ranges given in a display device's EDID, if any.  EDID provided "
      "range information will override the HorizSync and VertRefresh ranges "
      "specified in the Monitor section.  This option defaults to TRUE (the "
      "NVIDIA X driver will use frequency information from the EDID, when "
      "available)." },

    { "use-int10-module",
      XCONFIG_BOOL_VAL(USE_INT10_MODULE_BOOL_OPTION),
      NVGETOPT_IS_BOOLEAN, NULL,
      "Enable use of the X Int10 module to soft-boot all secondary cards, "
      "rather than POSTing the cards through the NVIDIA kernel module." },

    { "use-display-device", USE_DISPLAY_DEVICE_OPTION,
      NVGETOPT_STRING_ARGUMENT | NVGETOPT_ALLOW_DISABLE, "DISPLAY-DEVICE",
      "Force the X driver to use the display device specified." },

    { "use-events", 
      XCONFIG_BOOL_VAL(USE_EVENTS_BOOL_OPTION), NVGETOPT_IS_BOOLEAN, NULL,
      "Enable or disable \"UseEvents\" X configuration option. Setting this " 
      "option will enable the X driver to use the system events in some cases "
      "when it is waiting for the hardware. With this option X driver sets an "
      "event handler and waits for the hardware through the poll() system "
      "call. This option defaults to FALSE." },
     
    { "virtual", VIRTUAL_OPTION,
      NVGETOPT_STRING_ARGUMENT | NVGETOPT_ALLOW_DISABLE, "WIDTHxHEIGHT",
      "Specify the virtual screen resolution." },
      
    { "x-prefix", X_PREFIX_OPTION, NVGETOPT_STRING_ARGUMENT, NULL,
      "The X installation prefix; the default is /usr/X11R6/.  Only "
      "under rare circumstances should this option be needed." },

    { "xinerama", XCONFIG_BOOL_VAL(XINERAMA_BOOL_OPTION),
      NVGETOPT_IS_BOOLEAN, NULL, "Enable or disable Xinerama." },

    { "xvmc-uses-textures",
      XCONFIG_BOOL_VAL(XVMC_USES_TEXTURES_BOOL_OPTION),
      NVGETOPT_IS_BOOLEAN, NULL,
      "Forces XvMC to use the 3D engine for XvMCPutSurface requests rather "
      "than the video overlay." },
      
    { NULL, 0, 0, NULL, NULL },
};
