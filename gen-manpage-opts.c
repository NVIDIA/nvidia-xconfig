/*
 * Prints the option help in a form that is suitable to include in the manpage.
 */
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include "XF86Config-parser/xf86Parser.h"
#include "nvidia-xconfig.h"
#include "nvgetopt.h"
#include "option_table.h"

static void print_option(const NVGetoptOption *o)
{
    char scratch[64], *s;
    int j, len;

    int omitWhiteSpace;
    
    /* if we are going to need the argument, process it now */
    if (o->flags & NVGETOPT_HAS_ARGUMENT) {
        if (o->arg_name) {
            strcpy(scratch, o->arg_name);
        } else {
            len = strlen(o->name);
            for (j = 0; j < len; j++) scratch[j] = toupper(o->name[j]);
            scratch[len] = '\0';
        }
    }
    
    printf(".TP\n.BI \"");
    /* Print the name of the option */
    /* XXX We should backslashify the '-' characters in o->name. */
    
    if (isalpha(o->val)) {
        /* '\-c' */
        printf("\\-%c", o->val);

        if (o->flags & NVGETOPT_HAS_ARGUMENT) {
            /* ' " "ARG" "' */
            printf(" \" \"%s\" \"", scratch);
        }
        /* ', ' */
        printf(", ");
    }
    
    /* '\-\-name' */
    printf("\\-\\-%s", o->name);
    
    /* '=" "ARG' */
    if (o->flags & NVGETOPT_HAS_ARGUMENT) {
        printf("=\" \"%s", scratch);
        
        /* '" "' */
        if ((o->flags & NVGETOPT_IS_BOOLEAN) ||
            (o->flags & NVGETOPT_ALLOW_DISABLE)) {
            printf("\" \"");
        }
    }
    
    /* ', \-\-no\-name' */
    if (((o->flags & NVGETOPT_IS_BOOLEAN) &&
         !(o->flags & NVGETOPT_HAS_ARGUMENT)) ||
        (o->flags & NVGETOPT_ALLOW_DISABLE)) {
        printf(", \\-\\-no\\-%s", o->name);
    }
    
    printf("\"\n");
    
    /* Print the option description */
    /* XXX Each sentence should be on its own line! */
    
    /*
     * Print the option description:  write each character one at a
     * time (ugh) so that we can special-case a few characters:
     *
     * "[" --> "\n.I "
     * "]" --> "\n"
     * "-" --> "\-"
     *
     * Brackets are used to mark the text inbetween as italics.
     * '-' is special cased so that we can backslashify it.
     *
     * XXX Each sentence should be on its own line!
     */
    
    omitWhiteSpace = 0;
    
    for (s = o->description; s && *s; s++) {
        
        switch (*s) {
          case '[':
              printf("\n.I ");
              omitWhiteSpace = 0;
              break;
          case ']':
              printf("\n");
              omitWhiteSpace = 1;
              break;
          case '-':
              printf("\\-");
              omitWhiteSpace = 0;
              break;
          case ' ':
              if (!omitWhiteSpace) {
                  printf("%c", *s);
              }
              break;
          default:
              printf("%c", *s);
              omitWhiteSpace = 0;
              break;
        }
    }
    
    printf("\n");
}

int main(int argc, char* argv[])
{
    int i;
    const NVGetoptOption *o;

    /* Print the "simple" options, i.e. the ones you get by running
     * nvidia-xconfig --help.
     */
    printf(".SH OPTIONS\n");
    for (i = 0; __options[i].name; i++) {
        o = &__options[i];

        if (!(o->flags & NVGETOPT_HELP_ALWAYS))
            continue;

        print_option(o);
    }

    /* Print the advanced options. */
    printf(".SH \"ADVANCED OPTIONS\"\n");
    for (i = 0; __options[i].name; i++) {
        o = &__options[i];

        if (o->flags & NVGETOPT_HELP_ALWAYS)
            continue;

        print_option(o);
    }

    return 0;
}
