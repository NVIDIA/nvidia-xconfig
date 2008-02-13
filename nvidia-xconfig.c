/*
 * nvidia-xconfig: A tool for manipulating X config files,
 * specifically for use by the NVIDIA Linux graphics driver.
 *
 * Copyright (C) 2005 NVIDIA Corporation
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
 * nvidia-xconfig.c
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <libgen.h>

#include <sys/types.h>
#include <sys/stat.h>

#include "nvidia-xconfig.h"
#include "nvgetopt.h"

#define TAB    "  "
#define BIGTAB "      "



/*
 * print_version() - print version information
 */

extern const char *pNV_ID;

static void print_version(void)
{
    fmtout("");
    fmtout(pNV_ID);
    fmtoutp(TAB, "The NVIDIA X Configuration Tool.");
    fmtout("");
    fmtoutp(TAB, "This program is used to manipulate X configuration files, "
            "specifically to enable NVIDIA X driver functionality.");
    fmtout("");
    fmtoutp(TAB, "Copyright (C) 2005 NVIDIA Corporation.");
    fmtout("");
    
} /* print_version() */


static void print_summary(void)
{
    fmtout("");
    fmtoutp(TAB, "In its normal operation, nvidia-xconfig finds the system "
            "X configuration file (or generates a new X configuration if it "
            "cannot find the system file), makes sure the configuration is "
            "usable by the NVIDIA X driver, applies any updates requested "
            "on the commandline, and writes the new configuration to file.");
    fmtout("");
    fmtoutp(TAB, "Please see the NVIDIA README for a description of NVIDIA "
            "X configuration file options.");
    fmtout("");
}


#include "option_table.h"


/*
 * cook_description() - the description string may contain text within
 * brackets, which is used by the manpage generator to denote text to
 * be italicized.  We want to omit the bracket characters here.
 */

static char *cook_description(const char *description)
{
    int len;
    char *s, *dst;
    const char *src;
    
    len = strlen(description);
    s = nvalloc(len + 1);
    
    for (src = description, dst = s; *src; src++) {
        if (*src != '[' && (*src != ']')) {
            *dst = *src;
            dst++;
        }
    }

    *dst = '\0';

    return s;
    
} /* cook_description() */


/*
 * print_help() - loop through the __options[] table, and print the
 * description of each option.
 */

static void print_help(int advanced)
{
    int i, j, len;
    char *msg, *tmp, scratch[8], arg[64];
    const NVGetoptOption *o;
    
    print_version();
    print_summary();

    fmtout("");
    fmtout("nvidia-xconfig [options]");
    fmtout("");
    
    for (i = 0; __options[i].name; i++) {
        o = &__options[i];

        /*
         * if non-advanced help is requested, and the ALWAYS flag is
         * not set, then skip this option
         */

        if (!advanced && !(o->flags & OPTION_HELP_ALWAYS)) continue;

        /* if we are going to need the argument, process it now */

        if (o->flags & NVGETOPT_HAS_ARGUMENT) {
            if (o->arg_name) {
                strcpy(arg, o->arg_name);
            } else {
                len = strlen(o->name);
                for (j = 0; j < len; j++) arg[j] = toupper(o->name[j]);
                arg[len] = '\0';
            }
        }
        
        msg = nvstrcat("--", o->name, NULL);

        if (isalpha(o->val)) {
            sprintf(scratch, "%c", o->val);

            if (o->flags & NVGETOPT_HAS_ARGUMENT) {
                tmp = nvstrcat("-", scratch, " ", arg, ", ", msg, NULL);
            } else {
                tmp = nvstrcat("-", scratch, ", ", msg, NULL);
            }
            free(msg);
            msg = tmp;
        }
        
        if (o->flags & NVGETOPT_HAS_ARGUMENT) {
            tmp = nvstrcat(msg, "=", arg, NULL);
            free(msg);
            msg = tmp;
        }
        if (((o->flags & NVGETOPT_IS_BOOLEAN) &&
             !(o->flags & NVGETOPT_HAS_ARGUMENT)) ||
            (o->flags & NVGETOPT_ALLOW_DISABLE)) {
            tmp = nvstrcat(msg, ", --no-", o->name, NULL);
            free(msg);
            msg = tmp;
        }

        fmtoutp(TAB, msg);
        if (o->description) {
            tmp = cook_description(o->description);
            fmtoutp(BIGTAB, tmp);
            free(tmp);
        }
        fmtout("");
        free(msg);
    }
} /* print_help() */



/*
 * get_default_project_root() - scan some common directories for the X
 * project root
 *
 * Users of this information should be careful to account for the
 * modular layout.
 */

char *get_default_project_root(void)
{
    char *paths[] = { "/usr/X11R6", "/usr/X11", NULL };
    struct stat stat_buf;
    int i;
        
    for (i = 0; paths[i]; i++) {
        
        if (stat(paths[i], &stat_buf) == -1) {
            continue;
        }
    
        if (S_ISDIR(stat_buf.st_mode)) {
            return paths[i];
        }
    }
    
    /* default to "/usr/X11R6", I guess */

    return paths[0];

} /* get_default_project_root() */



/*
 * parse_commandline() - malloc an Options structure, initialize it,
 * and fill in any pertinent data from the commandline arguments
 */

Options *parse_commandline(int argc, char *argv[])
{
    Options *op;
    int c, boolval;
    u32 bit;
    char *strval;
    int intval, disable;

    op = (Options *) nvalloc(sizeof(Options));
    
    op->gop.x_project_root = get_default_project_root();
    op->nvagp = -1;
    op->transparent_index = -1;
    op->stereo = -1;
    
    while (1) {
        
        c = nvgetopt(argc, argv, __options, &strval,
                     &boolval, &intval, &disable);

        if (c == -1)
            break;

        /* catch the boolean options */

        if ((c >= XCONFIG_BOOL_OPTION_START) &&
            (c <= (XCONFIG_BOOL_OPTION_START + XCONFIG_BOOL_OPTION_COUNT))) {
            
            bit = GET_BOOL_OPTION_BIT(c - XCONFIG_BOOL_OPTION_START);
            GET_BOOL_OPTION_SLOT(op->boolean_options,
                                 c - XCONFIG_BOOL_OPTION_START) |= bit;
            if (boolval) {
                GET_BOOL_OPTION_SLOT(op->boolean_option_values,
                                     c - XCONFIG_BOOL_OPTION_START) |= bit;
            } else {
                GET_BOOL_OPTION_SLOT(op->boolean_option_values,
                                     c - XCONFIG_BOOL_OPTION_START) &= ~bit;
            }
            continue;
        }
        
        switch (c) {

        case 'v': print_version(); exit(0); break;
        case 'c': op->xconfig = strval; break;
        case 'o': op->output_xconfig = strval; break;
        case 't': op->tree = TRUE; break;
        case 'T': op->post_tree = TRUE; break;
        case 'h': print_help(FALSE); exit(0); break;
        case 'A': print_help(TRUE); exit(0); break;
        case 's': op->silent = TRUE; break;
        case 'a': op->enable_all_gpus = TRUE; break;
        case '1': op->only_one_screen = TRUE; break;
        
        case 'd': op->depth = intval;
            if ((op->depth != 8) &&
                (op->depth != 15) &&
                (op->depth != 16) &&
                (op->depth != 24)) {
                fprintf(stderr, "\n");
                fprintf(stderr, "Invalid depth: %d.\n", op->depth);
                fprintf(stderr, "\n");
                goto fail;
            }
            break;

        case LAYOUT_OPTION: op->layout = strval; break;
        case SCREEN_OPTION: op->screen = strval; break;

        case X_PREFIX_OPTION: op->gop.x_project_root = strval; break;

        case KEYBOARD_OPTION: op->gop.keyboard = strval; break;
        case KEYBOARD_LIST_OPTION: op->keyboard_list = TRUE; break;
        case KEYBOARD_DRIVER_OPTION: op->gop.keyboard_driver = strval; break;
            
        case MOUSE_OPTION: op->gop.mouse = strval; break;
        case MOUSE_LIST_OPTION: op->mouse_list = TRUE; break;
            
        case NVIDIA_CFG_PATH_OPTION: op->nvidia_cfg_path = strval; break;

        case FORCE_GENERATE_OPTION: op->force_generate = TRUE; break;

        case NVAGP_OPTION:

            /* mark as disabled, so we can remove the option later */
            
            if (disable) {
                op->nvagp = -2;
                break;
            }

            if      (strcasecmp(strval, "none")    == 0) op->nvagp = 0;
            else if (strcasecmp(strval, "nvagp")   == 0) op->nvagp = 1;
            else if (strcasecmp(strval, "agpgart") == 0) op->nvagp = 2;
            else if (strcasecmp(strval, "any")     == 0) op->nvagp = 3;
            else if ((strval[0] == '0') && (strval[1] == '\0')) op->nvagp = 0;
            else if ((strval[0] == '1') && (strval[1] == '\0')) op->nvagp = 1;
            else if ((strval[0] == '2') && (strval[1] == '\0')) op->nvagp = 2;
            else if ((strval[0] == '3') && (strval[1] == '\0')) op->nvagp = 3;
            else {
                fprintf(stderr, "\n");
                fprintf(stderr, "Invalid nvagp: %s.\n", strval);
                fprintf(stderr, "\n");
                goto fail;
            }
            break;

        case TRANSPARENT_INDEX_OPTION:

            /* mark as disabled, so we can remove the option later */
            
            if (disable) {
                op->transparent_index = -2;
                break;
            }

            if (intval < 0 || intval > 255) {
                fprintf(stderr, "\n");
                fprintf(stderr, "Invalid transparent index: %d.\n", intval);
                fprintf(stderr, "\n");
                goto fail;
            }
            op->transparent_index = intval;
            break;

        case STEREO_OPTION:

            /* mark as disabled, so we can remove the option later */
            
            if (disable) {
                op->stereo = -2;
                break;
            }

            if (intval < 1 || intval > 6) {
                fprintf(stderr, "\n");
                fprintf(stderr, "Invalid stereo: %d.\n", intval);
                fprintf(stderr, "\n");
                goto fail;
            }
            op->stereo = intval;
            break;

        case MODE_OPTION:
            if (boolval) {
                /* add this mode */
                nv_text_rows_append(&op->add_modes, strval);
            } else {
                /* remove this mode */
                nv_text_rows_append(&op->remove_modes, strval);
            }
            break;

        case REMOVE_MODE_OPTION:
            nv_text_rows_append(&op->remove_modes, strval);
            break;

        case MULTI_GPU_OPTION:
            {
                const char* valid_values[] = {
                    "0",
                    "no",
                    "off",
                    "false",
                    "single",
                    "1",
                    "yes",
                    "on",
                    "true",
                    "auto",
                    "afr",
                    "sfr",
                    "aa",
                    NULL
                };
                int i;

                /* mark as disabled, so we can remove the option later */
                
                if (disable) {
                    op->multigpu = NV_DISABLE_STRING_OPTION;
                    break;
                }

                for (i = 0; valid_values[i]; i++) {
                    if (!strcasecmp(strval, valid_values[i]))
                        break;
                }

                if (valid_values[i]) {
                    op->multigpu = strval;
                } else {
                    fprintf(stderr, "Invalid MultiGPU option: %s.\n", strval);
                    goto fail;
                }
            }
            break;

        case SLI_OPTION:
            {
                const char* valid_values[] = {
                    "0",
                    "no",
                    "off",
                    "false",
                    "single",
                    "1",
                    "yes",
                    "on",
                    "true",
                    "auto",
                    "afr",
                    "sfr",
                    "aa",
                    "afrofaa",
                    NULL
                };
                int i;

                /* mark as disabled, so we can remove the option later */
                
                if (disable) {
                    op->sli = NV_DISABLE_STRING_OPTION;
                    break;
                }

                for (i = 0; valid_values[i]; i++) {
                    if (!strcasecmp(strval, valid_values[i]))
                        break;
                }

                if (valid_values[i]) {
                    op->sli = strval;
                } else {
                    fprintf(stderr, "Invalid SLI option: %s.\n", strval);
                    goto fail;
                }
            }
            break;

        case ROTATE_OPTION:
            {
                const char* valid_values[] = {
                    "0",
                    "no",
                    "off",
                    "normal",
                    "left",
                    "CCW",
                    "inverted",
                    "right",
                    "CW",
                    NULL
                };
                int i;

                /* mark as disabled, so we can remove the option later */

                if (disable) {
                    op->rotate = NV_DISABLE_STRING_OPTION;
                    break;
                }

                for (i = 0; valid_values[i]; i++) {
                    if (!strcasecmp(strval, valid_values[i]))
                        break;
                }

                if (valid_values[i]) {
                    op->rotate = strval;
                } else {
                    fprintf(stderr, "Invalid Rotate option: %s.\n", strval);
                    goto fail;
                }
            }
            break;

        case DISABLE_SCF_OPTION: op->disable_scf = TRUE; break;
        
        case QUERY_GPU_INFO_OPTION: op->query_gpu_info = TRUE; break;

        case 'E':
            op->extract_edids_from_log = strval;
            break;

        case EXTRACT_EDIDS_OUTPUT_FILE_OPTION:
            op->extract_edids_output_file = strval;
            break;

        case TWINVIEW_XINERAMA_INFO_ORDER_OPTION:
            op->twinview_xinerama_info_order =
                disable ? NV_DISABLE_STRING_OPTION : strval;
            break;

        case TWINVIEW_ORIENTATION_OPTION:
            {
                const char* valid_values[] = {
                    "RightOf",
                    "LeftOf",
                    "Above",
                    "Below",
                    "Clone",
                    NULL
                };
                int i;

                if (disable) {
                    op->twinview_orientation = NV_DISABLE_STRING_OPTION;
                    break;
                }
                
                for (i = 0; valid_values[i]; i++) {
                    if (!strcasecmp(strval, valid_values[i]))
                        break;
                }

                if (!valid_values[i]) {
                    fprintf(stderr, "Invalid TwinViewOrientation option: "
                            "\"%s\".\n", strval);
                    goto fail;
                }
                
                op->twinview_orientation = strval;
            }
            break;

        case VIRTUAL_OPTION:
            {
                int ret, x, y;

                if (disable) {
                    op->virtual.x = op->virtual.y = -1;
                    break;
                }

                ret = sscanf(strval, "%dx%d", &x, &y);
                
                if (ret != 2) {
                    fprintf(stderr, "Invalid Virtual option: \"%s\".\n",
                            strval);
                    goto fail;
                }
                
                op->virtual.x = x;
                op->virtual.y = y;
                
                break;
            }

        case LOGO_PATH_OPTION:
            op->logo_path = disable ? NV_DISABLE_STRING_OPTION : strval;
            break;
  
        default:
            goto fail;
        }
    }

    /* do tilde expansion on the filenames given */
    
    op->xconfig = tilde_expansion(op->xconfig);
    op->output_xconfig = tilde_expansion(op->output_xconfig);
    
    /*
     * XXX save the option structure so that printing routines can
     * access it (and so that we don't have to carry the op everywhere
     * just so that we can pass it to printing routines).
     */
    {
        extern Options *__op;
        
        __op = op;
    }
    
    return (op);
    
 fail:
    
    fprintf(stderr, "\n");
    fprintf(stderr, "Invalid commandline, please run `%s --help` "
            "for usage information.\n", argv[0]);
    fprintf(stderr, "\n");
    exit(1);
    
    return NULL;

} /* parse_commandline() */




/*
 * backup_file() - create a backup of orig_filename, naming the backup
 * file "<original>.backup".
 *
 * XXX If we fail to write to the backup file (eg, it is in a
 * read-only directory), then we should do something intelligent like
 * write the backup to the user's home directory.
 */

int backup_file(Options *op, const char *orig_filename)
{
    char *filename;
    int ret = FALSE;
    
    /* construct the backup filename */
    
    filename = nvstrcat(orig_filename, ".backup", NULL);
    
    /* if the backup file already exists, remove it */

    if (access(filename, F_OK) == 0) {
        if (unlink(filename) != 0) {
            fmterr("Unable to create backup file '%s' (%s)",
                   filename, strerror(errno));
            goto done;
        }
    }

    /* copy the file */

    if (!copy_file(orig_filename, filename, 0644)) {
        /* copy_file() prints out its own error messages */
        goto done;
    }
    
    fmtmsg("Backed up file '%s' as '%s'", orig_filename, filename);
    ret = TRUE;
    
 done:

    free(filename);
    
    return ret;
    
} /* backup_file() */



/*
 * write_xconfig() - write the Xconfig to file.
 *
 * We search for the filename is this order:
 *
 * 1) "--output-xconfig" option
 *
 * 2) config->filename
 *
 * 3) use xf86openConfigFile()
 *
 * 4) If the detected X server is XFree86, we use use "/etc/X11/XF86Config"
 *    Otherwise we use "/etc/X11/xorg.conf",.
 */

int write_xconfig(Options *op, XConfigPtr config)
{
    char *filename = NULL;
    char *d, *tmp = NULL;
    int ret = FALSE;
    
    /* 1) "--output-xconfig" option */
    
    if (op->output_xconfig) {
        filename = nvstrdup(op->output_xconfig);
    }
    
    /* config->filename */

    if (!filename && config->filename) {
        filename = nvstrdup(config->filename);
    }
    
    /* use xf86openConfigFile() */
    
    if (!filename) {
        const char *f;
        f = xconfigOpenConfigFile(NULL, op->gop.x_project_root);
        if (f) {
            /* dup the string since closing the config file will free
               the string */
            filename = nvstrdup(f);
            xconfigCloseConfigFile();
        }
    }
    
    /* fall back to "/etc/X11/XF86Config" or "/etc/X11/xorg.conf" */
    
    if (!filename) {
        if (op->gop.xserver == X_IS_XF86) {
            filename = nvstrdup("/etc/X11/XF86Config");
        } else {
            filename = nvstrdup("/etc/X11/xorg.conf");
        }
    }
    
    /*
     * XXX it's strange that lack of permission to write to the target
     * location (the likely case with users not having write
     * permission on /etc/X11/ will fail at backup time, rather than
     * when we try to write the new file.  Perhaps we should backup to
     * a temporary location where we know we will have write access,
     * try to do the write, and only if we succeed in the write, move
     * the backup file into place.
     */
    
    /* check that we can write to this location */
    
    tmp = nvstrdup(filename);
    d = dirname(tmp);
    if (access(d, W_OK) != 0) {
        fmterr("Unable to write to directory '%s'.", d);
        goto done;
    }
    
    /* if the file already exists, create a backup first */
    
    if (access(filename, F_OK) == 0) {
        if (!backup_file(op, filename)) goto done;
    }
    
    /* write the config file */

    if (!xconfigWriteConfigFile(filename, config)) {
        fmterr("Unable to write file \"%s\"; please use the "
               "\"--output-xconfig\" commandline "
               "option to specify an alternative output file.", filename);
        goto done;
    }

    fmtmsg("New X configuration file written to '%s'", filename);
    fmtmsg("");
    
    
    /* Set the default depth in the Solaris Management Facility 
     * to the default depth of the first screen 
     */
    if (op->disable_scf == FALSE) {
        if(!update_scf_depth(config->screens[0].defaultdepth)) {
            goto done;
        }
    }
    
    ret = TRUE;
    
 done:

    if (filename) free(filename);
    if (tmp) free(tmp);

    return ret;

} /* write_xconfig() */



/*
 * update_banner() - add our banner at the top of the config, but
 * first we need to remove any lines that already include our prefix
 * (because presumably they are a banner from an earlier run of
 * nvidia-xconfig)
 */

static void update_banner(XConfigPtr config)
{
    static const char *banner =
        "X configuration file generated by nvidia-xconfig\n";
    static const char *prefix =
        "# nvidia-xconfig: ";

    char *s = config->comment;
    char *line, *eol, *tmp;
    
    /* remove all lines that begin with the prefix */
    
    while (s && (line = strstr(s, prefix))) {
        
        eol = strchr(line, '\n'); /* find the end of the line */
        
        if (eol) {
            eol++;
            if (*eol == '\0') eol = NULL;
        }
        
        if (line == s) { /* the line with the prefix is at the start */
            if (eol) {   /* there is more after the prefix line */
                tmp = strdup(eol);
                free(s);
                s = tmp;
            } else {     /* the prefix line is the only line */
                free(s);
                s = NULL;
            }
        } else {         /* prefix line is in the middle or end */
            *line = '\0';
            tmp = nvstrcat(s, eol, NULL);
            free(s);
            s = tmp;
        }
    }
    
    /* add our prefix lines at the start of the comment */
    
    config->comment = nvstrcat(prefix, banner, "# ", pNV_ID, "\n", s, NULL);
    
    if (s) free(s);
    
} /* update_banner() */



/*
 * find_system_xconfig() - find the system X config file and parse it;
 * returns XConfigPtr if successful, otherwise returns NULL.
 */

static XConfigPtr find_system_xconfig(Options *op)
{
    const char *filename;
    XConfigPtr config;
    XConfigError error;

    /* Find and open the existing X config file */
    
    filename = xconfigOpenConfigFile(op->xconfig, op->gop.x_project_root);
    
    if (filename) {
        fmtmsg("");
        fmtmsg("Using X configuration file: \"%s\".", filename);
    } else {
        fmtwarn("Unable to locate/open X configuration file.");
        return NULL;
    }
    
    /* Read the opened X config file */
    
    error = xconfigReadConfigFile(&config);
    if (error != XCONFIG_RETURN_SUCCESS) {
        xconfigCloseConfigFile();
        return NULL;;
    }

    /* Close the X config file */
    
    xconfigCloseConfigFile();
    
    /* Sanitize the X config file */
    
    if (!xconfigSanitizeConfig(config, op->screen, &(op->gop))) {
        xconfigFreeConfig(config);
        return NULL;
    }

    return config;

} /* find_system_xconfig() */



int update_xconfig(Options *op, XConfigPtr config)
{
    XConfigLayoutPtr layout;
    XConfigScreenPtr screen;
    XConfigAdjacencyPtr adj;

    /* get the layout to update */
    
    layout = get_layout(op, config);
    if (!layout) {
        return FALSE;
    }

    /* apply multi-display options */
    
    if (!apply_multi_screen_options(op, config, layout)) {
        return FALSE;
    }

    /*
     * update the device and option for all screens, or the screen
     * that was requested
     */
    
    if (op->screen) {
        screen = xconfigFindScreen(op->screen, config->screens);
        if (!screen) {
            fmterr("Unable to find screen '%s'", op->screen);
            return FALSE;
        }
        update_screen(op, config, screen);
    } else {
        for (adj = layout->adjacencies; adj; adj = adj->next) {
            update_screen(op, config, adj->screen);
        }
    }
    
    update_extensions(op, config);

    update_modules(config);

    update_banner(config);
    
    return TRUE;

} /* update_xconfig() */



/*
 * get_xserver_in_use() - try to determine which X server is in use
 * (XFree86, Xorg)
 *
 * XXX need to update for modular layout
 */

# define NV_LINE_LEN 1024
static void get_xserver_in_use(Options *op)
{
#if defined(NV_SUNOS)    
    op->gop.xserver=X_IS_XORG;
#else
    char *cmd;
    FILE *stream = NULL;
    int xserver = -1;

    cmd = xconfigStrcat(op->gop.x_project_root, "/bin/X -version 2>&1", NULL);
    if ((stream = popen(cmd, "r"))) {
         char buf[NV_LINE_LEN];

         while (1) {
            if (fgets(buf, NV_LINE_LEN-1, stream) == NULL) break;
            
            if (xserver == -1) {
                if (strcmp(buf, "XFree86") >= 0) {
                    xserver = X_IS_XF86;
                } else if (strcmp(buf, "X Window System") >= 0) {
                    xserver = X_IS_XORG;
                }
            }
        } 
    }
    /* Close the popen()'ed stream. */
    pclose(stream);
    free(cmd);

    if (xserver == -1) {
        char *xorgpath;

        xorgpath = xconfigStrcat(op->gop.x_project_root, "/bin/Xorg", NULL);
        if (access(xorgpath, F_OK)==0) {
            xserver = X_IS_XORG;
        } else {
            xserver = X_IS_XF86;
        }
        free(xorgpath);
    }
    
    op->gop.xserver=xserver;
#endif
} /* get_xserver_in_use */



/*
 * main program entry point
 *
 * The intended behavior is that, by default, nvidia-xconfig make the
 * system's X config file usable by the NVIDIA X driver.  If
 * nvidia-xconfig cannot file the system's X config file, then it
 * attempts to create one and make that usable.
 *
 *
 */

int main(int argc, char *argv[])
{
    Options *op;
    int ret;
    XConfigPtr config = NULL;
    
    /* parse the commandline */

    op = parse_commandline(argc, argv);
    
    /*
     * first, check for any of special options that cause us to exit
     * early
     */

    if (op->keyboard_list) {
        fmtout("\nPossible keyboard types; the short name is what should be "
               "passed to the \"--keyboard\" option.\n\n");
        xconfigGeneratePrintPossibleKeyboards();
        return 0;
    }

    if (op->mouse_list) {
        fmtout("\nPossible mouse types; the short name is what should be "
               "passed to the \"--mouse\" option.\n\n");
        xconfigGeneratePrintPossibleMice();
        return 0;
    }

    if (op->query_gpu_info) {
        ret = query_gpu_info(op);
        return (ret ? 0 : 1);
    }
 
    if (op->extract_edids_from_log) {
        ret = extract_edids(op);
        return (ret ? 0 : 1);
    }

    /*
     * we want to open and parse the system's existing X config file,
     * if possible
     */

    if (!op->force_generate) {
        config = find_system_xconfig(op);
    }
    
    /*
     * pass the system config (if any) to the tree printer
     */

    if (op->tree) {
        ret = print_tree(op, config);
        return (ret ? 0 : 1);
    }
    
    /*
     * Get which X server is in use: Xorg or XFree86
     */
    get_xserver_in_use(op);
    
    /*
     * if we failed to find the system's config file, generate a new
     * one
     */
    
    if (!config) {
        config = xconfigGenerate(&op->gop);
    }

    /*
     * if we don't have a valid config by now, something catestrophic
     * happened
     */

    if (!config) {
        fmterr("Unable to generate a usable X configuration file.");
        return 1;
    }

    /* now, we have a good config; apply whatever the user requested */
    
    update_xconfig(op, config);

    /* print the config in tree format, if requested */

    if (op->post_tree) {
        ret = print_tree(op, config);
        return (ret ? 0 : 1);
    }
    
    /* write the config back out to file */

    if (!write_xconfig(op, config)) {
        return 1;
    }

    return 0;
    
} /* main() */
