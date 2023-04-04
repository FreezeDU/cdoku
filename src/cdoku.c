/* Copyright (C) 2023  Aleksey Bagin aka freeze (email freeze@2du.ru) */

#include <stdio.h>
#include "cdlib.h"

#define CDOK   0
#define CDFAIL 1

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("No Task.");
        return CDFAIL;
    }

    char *task = argv[1];

    if (strlen(task) < CDCELS)
    {
        printf("Bad Size. Size: %zu.", strlen(task));
        return CDFAIL;
    }

    cdmap map;
    cdinitmap(&map);

    cdfld fld;
    cdinitfld(&fld, &map);

    for (cdval adr = 0; adr < CDCELS; adr++)
    {
        cdval val = task[adr] - '0';

        if (!val)
            continue;

        if (val <= CDMPTY || val > CDVALS)
        {
            printf("Bad Symbol. Offset: %u. Symbol: \"%c\".", adr, task[adr]);
            return CDFAIL;
        }

        if (!cdwrite(&fld.cls[adr], val))
        {
            printf("Bad Value. Offset: %u, Value: %u.", adr, val);
            return CDFAIL;
        }
    }

    if (!cdsolve(&fld))
    {
        printf("Can't Solve.");
        return CDFAIL;
    }

    for (cdval adr = 0; adr < CDCELS; adr++)
        printf("%u", fld.cls[adr].val);

    return CDOK;
}
