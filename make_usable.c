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
 * make_usable.c
 */

#include <unistd.h>
#include <string.h>
#include <sys/types.h>

#include "nvidia-xconfig.h"
#include "xf86Parser.h"


static int update_device(XConfigPtr config, XConfigDevicePtr device);
static void update_depth(Options *op, XConfigScreenPtr screen);
static void update_display(Options *op, XConfigScreenPtr screen);


/*
 * update_modules() - make sure the glx module is present, and remove
 * the GLcore and dri modules if they are present.
 */

int update_modules(XConfigPtr config)
{
    XConfigLoadPtr load, next;
    int found;

    /* make sure glx is loaded */

    found = FALSE;
    for (load = config->modules->loads; load; load = load->next) {
        if (xconfigNameCompare("glx", load->name) == 0) found = TRUE;
    }

    if (!found) {
        config->modules->loads =
            xconfigAddNewLoadDirective(config->modules->loads,
                                       "glx", XCONFIG_LOAD_MODULE,
                                       NULL, FALSE);
    }

    /* make sure GLcore and dri are not loaded */

    load = config->modules->loads;
    while (load) {
        next = load->next;
        if (xconfigNameCompare("GLcore", load->name) == 0) {
            config->modules->loads =
                xconfigRemoveLoadDirective(config->modules->loads, load);
        } else if (xconfigNameCompare("dri", load->name) == 0) {
            config->modules->loads =
                xconfigRemoveLoadDirective(config->modules->loads, load);
        }
        load = next;
    }

    return TRUE;
    
} /* update_modules() */



/*
 * update_screen() - apply any requested updates to the given screen
 */

int update_screen(Options *op, XConfigPtr config, XConfigScreenPtr screen)
{
    /* migrate any options from device to screen to avoid conflicts */
    screen->options = xconfigOptionListMerge(screen->options,
                                             screen->device->options);
    screen->device->options = NULL;
    
    update_display(op, screen);
    update_depth(op, screen);
    update_device(config, screen->device);
    update_options(op, screen);
    
    return TRUE;

} /* update_screen() */



/*
 * get_layout() - get the right layout from the config that we should
 * edit
 */

XConfigLayoutPtr get_layout(Options *op, XConfigPtr config)
{
    XConfigLayoutPtr layout;

    /* select a screenLayout to use */
    
    if (op->layout) {
        
        /* if a layout was specified on the commandline, use that */
        
        layout = xconfigFindLayout(op->layout, config->layouts);
        if (!layout) {
            fmterr("Unable to find layout \"%s\".\n", op->layout);
            return NULL;
        }
    } else {
        
        /* otherwise, use the first layout in the config file */
        
        if (!config->layouts) {
            fmterr("unable to select ScreenLayout to use.\n");
            return NULL;
        }
        
        layout = config->layouts;
    }

    return layout;

} /* get_layout() */



/*
 * update_device() - update the device; there is a lot of information
 * in the device that is not relevant to the NVIDIA X driver.  In
 * fact, some options, like "Chipset" can actually prevent XFree86
 * from using the NVIDIA driver.  The simplest solution is to zero out
 * the device, and only retain the few fields that are meaningful for
 * the NVIDIA X driver.
 */

static int update_device(XConfigPtr config, XConfigDevicePtr device)
{
    char *identifier, *vendor, *comment, *board, *busid;
    int screen;
    XConfigDevicePtr next;
    XConfigOptionPtr options;
    
    next = device->next;
    options = device->options;
    identifier = device->identifier;
    vendor = device->vendor;
    comment = device->comment;
    screen = device->screen;
    board = device->board;
    busid = device->busid;
    
    memset(device, 0, sizeof(XConfigDeviceRec));

    device->next = next;
    device->options = options;
    device->identifier = identifier;
    device->vendor = vendor;
    device->comment = comment;
    device->screen = screen;
    device->board = board;
    device->busid = busid;

    device->chipid = -1;
    device->chiprev = -1;
    device->irq = -1;
    
    device->driver = "nvidia";
    
    /*
     * XXX do we really want to preserve the BusID line?  Let's only
     * preserve the BusID if there are multiple screens in this
     * config; not a very good heuristic
     */
    
    if (!config->screens->next) {
        device->busid = NULL;
    }
    
    return TRUE;
    
} /* update_device() */



/*
 * update_depth() - make sure there is a display section with the
 * default depth, and possibly update the default depth
 */

static void update_depth(Options *op, XConfigScreenPtr screen)
{
    XConfigDisplayPtr display;
    int found;
    
    /* update the depth */
    if ((op->depth == 8) || (op->depth == 15) ||
        (op->depth == 16) || (op->depth == 24)) {
        screen->defaultdepth = op->depth;
    }
    
    /*
     * if there is no display at the default depth, force the first
     * display section to that depth
     */
    
    found = FALSE;
        
    for (display = screen->displays; display; display = display->next) {
        if (display->depth == screen->defaultdepth) {
            found = TRUE;
            break;
        }
    }
    
    if (!found) {
        screen->displays->depth = screen->defaultdepth;
    }
    
} /* update_depth() */



/*
 * update_display() - if there are no display subsections, create one
 */

static void update_display(Options *op, XConfigScreenPtr screen)
{
    
    if (!screen->displays) {
        XConfigDisplayPtr display;
        XConfigModePtr mode = NULL;
        
        mode = xconfigAddMode(mode, "640x480");
        mode = xconfigAddMode(mode, "800x600");
        mode = xconfigAddMode(mode, "1024x768");
        mode = xconfigAddMode(mode, "1280x1024");
        mode = xconfigAddMode(mode, "1600x1200");
        
        display = nvalloc(sizeof(XConfigDisplayRec));
        display->depth = screen->defaultdepth;
        display->modes = mode;
        display->frameX0 = -1;
        display->frameY0 = -1;
        display->black.red = -1;
        display->white.red = -1;
        
        screen->displays = display;
    }
} /* update_display() */


