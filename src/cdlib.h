/* Copyright (C) 2023  Aleksey Bagin aka freeze (email freeze@2du.ru) */

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define CDMPTY 0
#define CDSNGL 1
#define CDBLKS 3
#define CDVALS 9
#define CDOPTS 10
#define CDRELS 20
#define CDCELS 81

typedef int8_t cdval;

typedef struct cdmap cdmap;
typedef struct cdcel cdcel;
typedef struct cdfld cdfld;

struct cdmap
{
    cdval rls[CDCELS][CDRELS];
};

struct cdcel
{
    cdfld *fld;
    cdval  adr;
    cdval  val;

    struct cdops
    {
        cdval cnt;
        cdval vls[CDOPTS];
    }
    ops;
};

struct cdfld
{
    cdmap *map;
    cdval  mpt;
    cdcel  cls[CDCELS];
    cdcel *cnd;
    cdval  ops[CDOPTS];
};

void cdinitmap(cdmap *map);
void cdinitfld(cdfld *fld, cdmap *map);

bool cdrmopt(cdcel *cel, cdval val);
bool cdwrite(cdcel *cel, cdval val);
bool cdsolve(cdfld *fld);
