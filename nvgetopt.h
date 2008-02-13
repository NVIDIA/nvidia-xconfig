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
 * nvgetopt.h
 */

#ifndef __NVGETOPT_H__
#define __NVGETOPT_H__

#define NVGETOPT_FALSE 0
#define NVGETOPT_TRUE 1


/*
 * indicates that the option is a boolean value; the presence of the
 * option will be interpretted as a TRUE value; if the option is
 * prepended with '--no-', the option will be interpretted as a FALSE
 * value.  On success, nvgetopt will return the parsed boolean value
 * through 'boolval'.
 */

#define NVGETOPT_IS_BOOLEAN       0x1


/*
 * indicates that the option takes an argument to be interpretted as a
 * string; on success, nvgetopt will return the parsed string argument
 * through 'strval'.
 */

#define NVGETOPT_STRING_ARGUMENT  0x2


/*
 * indicates that the option takes an argument to be interpretted as
 * an integer; on success, nvgetopt will return the parsed integer
 * argument through 'intval'.
 */

#define NVGETOPT_INTEGER_ARGUMENT 0x4


/*
 * indicates that the option, which normally takes an argument, can be
 * disabled if the option is prepended with '--no-', in which case,
 * the option does not take an argument.  If the option is disabled,
 * nvgetopt will return TRUE through 'disable_val'.
 *
 * Note that NVGETOPT_ALLOW_DISABLE can only be used with options that
 * take arguments.
 */

#define NVGETOPT_ALLOW_DISABLE    0x8

/*
 * indicates that the option takes an argument to be interpretted as
 * an double; on success, nvgetopt will return the parsed double
 * argument through 'doubleval'.
 */

#define NVGETOPT_DOUBLE_ARGUMENT  0x10

#define NVGETOPT_HAS_ARGUMENT (NVGETOPT_STRING_ARGUMENT | \
                               NVGETOPT_INTEGER_ARGUMENT | \
                               NVGETOPT_DOUBLE_ARGUMENT)

typedef struct {
    const char *name;
    int val;
    unsigned int flags;
    char *arg_name; /* not used by nvgetopt() */
    char *description; /* not used by nvgetopt() */
} NVGetoptOption;

int nvgetopt(int argc, char *argv[], const NVGetoptOption *options,
             char **strval, int *boolval, int *intval, double *doubleval,
             int *disable_val);

#endif /* __NVGETOPT_H__ */
