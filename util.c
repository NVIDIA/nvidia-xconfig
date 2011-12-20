/*
 * nvidia-xconfig: A tool for manipulating XF86Config files,
 * specifically for use by the NVIDIA Linux graphics driver.
 *
 * Copyright (C) 2004 NVIDIA Corporation
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 * 
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses>.
 *
 *
 * util.c
 */


#include <stdio.h>
#include <stdarg.h>

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <ctype.h>
#include <pwd.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/termios.h>

#include "nvidia-xconfig.h"

Options *__op = NULL;

static void vformat(FILE *stream, const char *prefix, const char *msg);


/*
 * NV_VSNPRINTF(): macro that assigns b using vsnprintf().  This is
 * supposedly correct for differing semantics of vsnprintf() in
 * different versions of glibc:
 *
 * different semantics of the return value from (v)snprintf:
 *
 * -1 when the buffer is not long enough (glibc < 2.1)
 *
 *   or
 *
 * the length the string would have been if the buffer had been large
 * enough (glibc >= 2.1)
 *
 * This macro allocates memory for b; the caller should free it when
 * done.
 */

#define NV_FMT_BUF_LEN 64

#define NV_VSNPRINTF(b, f)                                  \
{                                                           \
    va_list ap;                                             \
    int len, current_len = NV_FMT_BUF_LEN;                  \
                                                            \
    (b) = nvalloc(current_len);                             \
                                                            \
    while (1) {                                             \
        va_start(ap, f);                                    \
        len = vsnprintf((b), current_len, (f), ap);         \
        va_end(ap);                                         \
                                                            \
        if ((len > -1) && (len < current_len)) {            \
            break;                                          \
        } else if (len > -1) {                              \
            current_len = len + 1;                          \
        } else {                                            \
            current_len += NV_FMT_BUF_LEN;                  \
        }                                                   \
                                                            \
        free(b);                                            \
        (b) = nvalloc(current_len);                         \
    }                                                       \
}


/*
 * copy_file() - copy the file specified by srcfile to dstfile, using
 * mmap and memcpy.  The destination file is created with the
 * permissions specified by mode.  Roughly based on code presented by
 * Richard Stevens, in Advanced Programming in the Unix Environment,
 * 12.9.
 */

int copy_file(const char *srcfile, const char *dstfile, mode_t mode)
{
    int src_fd = -1, dst_fd = -1;
    struct stat stat_buf;
    char *src, *dst;
    int ret = FALSE;

    if ((src_fd = open(srcfile, O_RDONLY)) == -1) {
        fmterr("Unable to open '%s' for copying (%s)",
               srcfile, strerror (errno));
        goto done;
    }
    if ((dst_fd = open(dstfile, O_RDWR | O_CREAT | O_TRUNC, mode)) == -1) {
        fmterr("Unable to create '%s' for copying (%s)",
               dstfile, strerror (errno));
        goto done;
    }
    if (fstat(src_fd, &stat_buf) == -1) {
        fmterr("Unable to determine size of '%s' (%s)",
               srcfile, strerror (errno));
        goto done;
    }
    if (stat_buf.st_size == 0) {
        /* src file is empty; silently ignore */
        ret = TRUE;
        goto done;
    }
    if (lseek(dst_fd, stat_buf.st_size - 1, SEEK_SET) == -1) {
        fmterr("Unable to set file size for '%s' (%s)",
               dstfile, strerror (errno));
        goto done;
    }
    if (write(dst_fd, "", 1) != 1) {
        fmterr("Unable to write file size for '%s' (%s)",
               dstfile, strerror (errno));
        goto done;
    }
    if ((src = mmap(0, stat_buf.st_size, PROT_READ,
                    MAP_SHARED, src_fd, 0)) == (void *) -1) {
        fmterr("Unable to map source file '%s' for "
               "copying (%s)", srcfile, strerror (errno));
        goto done;
    }
    if ((dst = mmap(0, stat_buf.st_size, PROT_READ | PROT_WRITE,
                    MAP_SHARED, dst_fd, 0)) == (void *) -1) {
        fmterr("Unable to map destination file '%s' for "
               "copying (%s)", dstfile, strerror (errno));
        goto done;
    }
    
    memcpy(dst, src, stat_buf.st_size);
    
    if (munmap (src, stat_buf.st_size) == -1) {
        fmterr("Unable to unmap source file '%s' after "
               "copying (%s)", srcfile, strerror (errno));
        goto done;
    }
    if (munmap (dst, stat_buf.st_size) == -1) {
        fmterr("Unable to unmap destination file '%s' after "
               "copying (%s)", dstfile, strerror (errno));
        goto done;
    }
    
    ret = TRUE;

 done:

    if (src_fd != -1) close(src_fd);
    if (dst_fd != -1) close(dst_fd);
    
    return ret;
    
} /* copy_file() */


/*
 * directory_exists() - 
 */

int directory_exists(const char *dir)
{
    struct stat stat_buf;

    if ((stat (dir, &stat_buf) == -1) || (!S_ISDIR(stat_buf.st_mode))) {
        return FALSE;
    } else {
        return TRUE;
    }
} /* directory_exists() */



/*
 * xconfigPrint() - this is the one entry point that a user of the
 * XF86Config-Parser library must provide.
 */

void xconfigPrint(MsgType t, const char *msg)
{
    typedef struct {
        MsgType msg_type;
        char *prefix;
        FILE *stream;
        int newline;
    } MessageTypeAttributes;

    char *prefix = NULL;
    int i, newline = FALSE;
    FILE *stream = stdout;
    
    const MessageTypeAttributes msg_types[] = {
        { ParseErrorMsg,      "PARSE ERROR: ",      stderr, TRUE  },
        { ParseWarningMsg,    "PARSE WARNING: ",    stderr, TRUE  },
        { ValidationErrorMsg, "VALIDATION ERROR: ", stderr, TRUE  },
        { InternalErrorMsg,   "INTERNAL ERROR: ",   stderr, TRUE  },
        { WriteErrorMsg,      "ERROR: ",            stderr, TRUE  },
        { WarnMsg,            "WARNING: ",          stderr, TRUE  },
        { ErrorMsg,           "ERROR: ",            stderr, TRUE  },
        { DebugMsg,           "DEBUG: ",            stdout, FALSE },
        { UnknownMsg,          NULL,                stdout, FALSE },
    };
    
    for (i = 0; msg_types[i].msg_type != UnknownMsg; i++) {
        if (msg_types[i].msg_type == t) {
            prefix  = msg_types[i].prefix;
            newline = msg_types[i].newline;
            stream  = msg_types[i].stream;
            break;
        }
    }
    
    if (newline) vformat(stream, NULL, "");
    vformat(stream, prefix, msg);
    if (newline) vformat(stream, NULL, "");
    
} /* xconfigPrint */



static unsigned short __terminal_width = 0;

#define DEFAULT_WIDTH 75

/*
 * reset_current_terminal_width() - if new_val is zero, then use the
 * TIOCGWINSZ ioctl to get the current width of the terminal, and
 * assign it the value to __terminal_width.  If the ioctl fails, use a
 * hardcoded constant.  If new_val is non-zero, then use new_val.
 */

void reset_current_terminal_width(unsigned short new_val)
{
    struct winsize ws;
    
    if (new_val) {
        __terminal_width = new_val;
        return;
    }

    if (ioctl(STDERR_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        __terminal_width = DEFAULT_WIDTH;
    } else {
        __terminal_width = ws.ws_col - 1;
    }
} /* get_current_terminal_width() */



/*
 * vformat() - we use nv_format_text_rows() to format the string so
 * that not more than __terminal_width characters are printed across.
 *
 * The resulting formatted output is written to the specified stream.
 * The output may also include an optional prefix (to be prepended on
 * the first line, and filled with spaces on subsequent lines.
 */

static void vformat(FILE *stream, const char *prefix, const char *msg)
{
    int i;
    TextRows *t;
    
    if (!__terminal_width) reset_current_terminal_width(0);

    t = nv_format_text_rows(prefix, msg, __terminal_width, TRUE);

    for (i = 0; i < t->n; i++) fprintf(stream, "%s\n", t->t[i]);
    
    nv_free_text_rows(t);
    
} /* vformat() */



/*
 * nv_format_text_rows() - this function breaks the given string str
 * into some number of rows, where each row is not longer than the
 * specified width.
 *
 * If prefix is non-NULL, the first line is prepended with the prefix,
 * and subsequent lines are indented to line up with the prefix.
 *
 * If word_boundary is TRUE, then attempt to only break lines on
 * boundaries between words.
 */

TextRows *nv_format_text_rows(const char *prefix,
                              const char *str,
                              int width, int word_boundary)
{
    int len, prefix_len, z, w, i;
    char *line, *buf, *local_prefix, *a, *b, *c;
    TextRows *t;
    
    /* initialize the TextRows structure */

    t = (TextRows *) nvalloc(sizeof(TextRows));
    t->t = NULL;
    t->n = 0;
    t->m = 0;

    if (!str) return t;

    buf = strdup(str);

    z = strlen(buf); /* length of entire string */
    a = buf;         /* pointer to the start of the string */

    /* initialize the prefix fields */

    if (prefix) {
        prefix_len = strlen(prefix);
        local_prefix = strdup(prefix);
    } else {
        prefix_len = 0;
        local_prefix = NULL;
    }

    /* adjust the max width for any prefix */

    w = width - prefix_len;

    do {
        /*
         * if the string will fit on one line, point b to the end of the
         * string
         */
        
        if (z < w) b = a + z;

        /* 
         * if the string won't fit on one line, move b to where the
         * end of the line should be, and then move b back until we
         * find a space; if we don't find a space before we back b all
         * the way up to a, just assign b to where the line should end.
         */
        
        else {
            b = a + w;
            
            if (word_boundary) {
                while ((b >= a) && (!isspace(*b))) b--;
                if (b <= a) b = a + w;
            }
        }

        /* look for any newline inbetween a and b, and move b to it */
        
        for (c = a; c < b; c++) if (*c == '\n') { b = c; break; }
        
        /*
         * copy the string that starts at a and ends at b, prepending
         * with a prefix, if present
         */

        len = b-a;
        len += prefix_len;
        line = (char *) malloc(len+1);
        if (local_prefix) strncpy(line, local_prefix, prefix_len);
        strncpy(line + prefix_len, a, len - prefix_len);
        line[len] = '\0';
        
        /* append the new line to the array of text rows */

        t->t = (char **) realloc(t->t, sizeof(char *) * (t->n + 1));
        t->t[t->n] = line;
        t->n++;
        
        if (t->m < len) t->m = len;

        /*
         * adjust the length of the string and move the pointer to the
         * beginning of the new line
         */
        
        z -= (b - a + 1);
        a = b + 1;

        /* move to the first non whitespace character (excluding newlines) */
        
        if (word_boundary && isspace(*b)) {
            while ((z) && (isspace(*a)) && (*a != '\n')) a++, z--;
        } else {
            if (!isspace(*b)) z++, a--;
        }
        
        if (local_prefix) {
            for (i = 0; i < prefix_len; i++) local_prefix[i] = ' ';
        }
        
    } while (z > 0);

    if (local_prefix) free(local_prefix);
    free(buf);
    
    return t;

} /* nv_format_text_rows() */



void nv_text_rows_append(TextRows *t, const char *msg)
{
    int len;

    t->t = (char **) nvrealloc(t->t, sizeof(char *) * (t->n + 1));

    if (msg) {
        t->t[t->n] = strdup(msg);
        len = strlen(msg);
        if (t->m < len) t->m = len;
    } else {
        t->t[t->n] = NULL;
    }
    
    t->n++;
}

/*
 * nv_concat_text_rows() - concatenate two text rows
 */

#define NV_MAX(x,y) ((x) > (y) ? (x) : (y))

void nv_concat_text_rows(TextRows *t0, TextRows *t1)
{
    int n, i;
    
    n = t0->n + t1->n;
    
    t0->t = (char **) nvrealloc(t0->t, sizeof(char *) * n);
    
    for (i = 0; i < t1->n; i++) {
        t0->t[i + t0->n] = nvstrdup(t1->t[i]);
    }

    t0->m = NV_MAX(t0->m, t1->m);
    t0->n = n;

} /* nv_concat_text_rows() */




/*
 * nv_free_text_rows() - free the TextRows data structure allocated by
 * nv_format_text_rows()
 */

void nv_free_text_rows(TextRows *t)
{
    int i;
    
    if (!t) return;
    for (i = 0; i < t->n; i++) free(t->t[i]);
    if (t->t) free(t->t);
    free(t);

} /* nv_free_text_rows() */


/*
 * fget_next_line() - read from the given FILE stream until a newline,
 * EOF, or null terminator is encountered, writing data into a
 * growable buffer.  The eof parameter is set to TRUE when EOF is
 * encountered.  In all cases, the returned string is null-terminated.
 *
 * XXX this function will be rather slow because it uses fgetc() to
 * pull each character off the stream one at a time; this is done so
 * that each character can be examined as it's read so that we can
 * appropriately deal with EOFs and newlines.  A better implementation
 * would use fgets(), but that would still require us to parse each
 * read line, checking for newlines or guessing if we hit an EOF.
 */

#define NV_LINE_LEN 32

char *fget_next_line(FILE *fp, int *eof)
{
    char *buf = NULL, *tmpbuf;
    char *c = NULL;
    int len = 0, buflen = 0;
    
    if (eof) *eof = FALSE;
    
    while (1) {
        if (buflen == len) { /* buffer isn't big enough -- grow it */
            buflen += NV_LINE_LEN;
            tmpbuf = (char *) nvalloc (buflen);
            if (buf) {
                memcpy (tmpbuf, buf, len);
                free (buf);
            }
            buf = tmpbuf;
            c = buf + len;
        }

        *c = fgetc(fp);
        
        if ((*c == EOF) && (eof)) *eof = TRUE;
        if ((*c == EOF) || (*c == '\n') || (*c == '\0')) {
            *c = '\0';
            return buf;
        }

        len++;
        c++;

    } /* while (1) */
    
    return NULL; /* should never get here */
   
} /* fget_next_line() */


#define NV_MSG_LEVEL_LINE    0
#define NV_MSG_LEVEL_MESSAGE 1
#define NV_MSG_LEVEL_WARNING 2
#define NV_MSG_LEVEL_ERROR   3

static void __print_message(const int level, const char *caller_prefix,
                            const char *msg)
{
    typedef struct {
        char *prefix;
        FILE *stream;
        int newline;
    } MessageLevelAttributes;

    const char *prefix = NULL;
    
    const MessageLevelAttributes msg_attrs[] = {
        { NULL,        stdout, FALSE }, /* NV_MSG_LEVEL_LOG */
        { NULL,        stdout, FALSE }, /* NV_MSG_LEVEL_MESSAGE */
        { "WARNING: ", stderr, TRUE  }, /* NV_MSG_LEVEL_WARNING */
        { "ERROR: ",   stderr, TRUE  }  /* NV_MSG_LEVEL_ERROR */
    };
    
    if (caller_prefix) {
        prefix = caller_prefix;
    } else {
        prefix = msg_attrs[level].prefix;
    }
    
    if (msg_attrs[level].newline) vformat(msg_attrs[level].stream, NULL, "");
    vformat(msg_attrs[level].stream, prefix, msg);
    if (msg_attrs[level].newline) vformat(msg_attrs[level].stream, NULL, "");
}

void fmtout(const char *fmt, ...)
{
    char *msg;
    
    NV_VSNPRINTF(msg, fmt);
    if (!__op || !__op->silent)
        __print_message(NV_MSG_LEVEL_LINE, NULL, msg);
    free(msg);
}

void fmtoutp(const char *prefix, const char *fmt, ...)
{
    char *msg;
    
    NV_VSNPRINTF(msg, fmt);
    if (!__op || !__op->silent)
        __print_message(NV_MSG_LEVEL_LINE, prefix, msg);
    free(msg);
}

void fmtmsg(const char *fmt, ...)
{
    char *msg;
    
    NV_VSNPRINTF(msg, fmt);
    if (!__op || !__op->silent)
        __print_message(NV_MSG_LEVEL_MESSAGE, NULL, msg);
    free(msg);
}

void fmterr(const char *fmt, ...)
{
    char *msg;
    
    NV_VSNPRINTF(msg, fmt);
    __print_message(NV_MSG_LEVEL_ERROR, NULL, msg);
    free(msg);
}

void fmtwarn(const char *fmt, ...)
{
    char *msg;

    NV_VSNPRINTF(msg, fmt);
    __print_message(NV_MSG_LEVEL_WARNING, NULL, msg);
    free(msg);
}

