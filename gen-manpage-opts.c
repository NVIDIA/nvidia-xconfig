/*
 * Prints the option help in a form that is suitable to include in the manpage.
 */
#include <stdio.h>
#include <ctype.h>

#include "XF86Config-parser/xf86Parser.h"
#include "nvidia-xconfig.h"
#include "nvgetopt.h"
#include "option_table.h"

static void print_option(const NVGetoptOption *o)
{
    printf(".TP\n.BI ");
    /* Print the name of the option */
    /* XXX We should backslashify the '-' characters in o->name. */
    if (o->flags & NVGETOPT_IS_BOOLEAN) {
        /* "\-\-name, \-\-no\-name */
        printf("\"\\-\\-%s, \\-\\-no\\-%s", o->name, o->name);
    } else if (isalpha(o->val)) {
        /* "\-c, \-\-name */
        printf("\"\\-%c, \\-\\-%s", o->val, o->name);
    } else {
        /* "\-\-name */
        printf("\"\\-\\-%s", o->name);
    }

    if (o->flags & NVGETOPT_HAS_ARGUMENT) {
        printf("=\" \"%s", o->name);
    }

    printf("\"\n");

    /* Print the option description */
    /* XXX Each sentence should be on its own line! */
    /* XXX We need to backslashify the '-' characters here. */
    printf("%s\n", o->description);
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

        if (!(o->flags & OPTION_HELP_ALWAYS))
            continue;

        print_option(o);
    }

    /* Print the advanced options. */
    printf(".SH \"ADVANCED OPTIONS\"\n");
    for (i = 0; __options[i].name; i++) {
        o = &__options[i];

        if (o->flags & OPTION_HELP_ALWAYS)
            continue;

        print_option(o);
    }

    return 0;
}
