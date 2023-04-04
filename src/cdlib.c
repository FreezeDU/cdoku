/* Copyright (C) 2023  Aleksey Bagin aka freeze (email freeze@2du.ru) */

#include "cdlib.h"

void
cdinitmap(cdmap *map)
{
    for (cdval i = 0; i < CDCELS; i++)
    {
        cdval ir  = i  / CDVALS,
              ic  = i  % CDVALS,
              ibr = ir / CDBLKS,
              ibc = ic / CDBLKS;

        for (cdval j = 0, e = 0; j < CDCELS; j++)
        {
            if (i == j)
                continue;

            cdval jr = j / CDVALS,
                  jc = j % CDVALS;

            if (ir == jr || ic == jc ||
                  (ibr == (jr / CDBLKS) && ibc == (jc / CDBLKS)))

                map->rls[i][e++] = j;
        }
    }
}

void
cdinitfld(cdfld *fld, cdmap *map)
{
    memset(fld, CDMPTY, sizeof(cdfld));

    fld->map = map;
    fld->mpt = CDCELS;

    for (cdval adr = 0; adr < CDCELS; adr++)
    {
        cdcel *cel = &fld->cls[adr];

        cel->fld = fld;
        cel->adr = adr;

        for (cdval val = 1; val < CDOPTS; val++)
            cel->ops.vls[val] = true;

        cel->ops.cnt = CDVALS;
    }

    fld->ops[CDVALS] = CDCELS;
}

bool
cdrmopt(cdcel *cel, cdval val)
{
    if (!cel || cel->val)
        return false;

    if (val <= CDMPTY || val > CDVALS)
        return false;

    if (!cel->ops.vls[val])
        return false;

    if (cel->ops.cnt == CDSNGL)
        return false;

    cdfld *fld = cel->fld;

    fld->ops[cel->ops.cnt]--;
    fld->ops[--cel->ops.cnt]++;

    if (!fld->cnd ||
            fld->cnd->ops.cnt > cel->ops.cnt)
        fld->cnd = cel;

    cel->ops.vls[val] = false;
    return true;
}

bool
cdwrite(cdcel *cel, cdval val)
{
    if (!cel || cel->val)
        return false;

    if (val <= CDMPTY || val > CDVALS)
        return false;

    if (!cel->ops.vls[val])
        return false;

    cdfld *fld = cel->fld,
           src;

    memcpy(&src, fld, sizeof(cdfld));

    fld->mpt--;
    fld->ops[cel->ops.cnt]--;

    if (fld->cnd == cel)
        fld->cnd = NULL;

    cel->val = val;

    for (cdval off = 0; off < CDRELS; off++)
    {
        cdcel *rel = &fld->cls[
            fld->map->rls[cel->adr][off]
        ];

        if (rel->val || !rel->ops.vls[val])
            continue;

        if (!cdrmopt(rel, val))
        {
            memcpy(fld, &src, sizeof(cdfld));
            return false;
        }
    }

    return true;
}

bool
cdsolve(cdfld *fld)
{
    cdval ops;

    for (ops = 1; ops < CDOPTS; ops++)
    {
        if (fld->ops[ops])
            break;
    }

    cdcel *cel = NULL;

    if (fld->cnd && fld->cnd->ops.cnt == ops)
        cel = fld->cnd;
    else
    {
        for (cdval adr = 0; adr < CDCELS; adr++)
        {
            cdcel *cur = &fld->cls[adr];

            if (cur->val || cur->ops.cnt != ops)
                continue;

            cel = cur;
            break;
        }
    }

    if (!cel)
        return false;

    cdfld src;
    memcpy(&src, fld, sizeof(cdfld));

    struct { cdval cnt; cdval ops[CDOPTS]; } lst;
    lst.cnt = CDMPTY;

    for (cdval opt = 1; opt < CDOPTS; opt++)
    {
        if (!cel->ops.vls[opt])
            continue;

        if (cdwrite(cel, opt) && (!fld->mpt || cdsolve(fld)))
            return true;

        if (++lst.cnt == cel->ops.cnt)
            break;

        memcpy(fld, &src, sizeof(cdfld));
        lst.ops[lst.cnt] = opt;

        for (cdval off = 0; off < lst.cnt; off++)
            cdrmopt(cel, lst.ops[off]);
    }

    memcpy(fld, &src, sizeof(cdfld));
    return false;
}
