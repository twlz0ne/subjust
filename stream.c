// 
//  stream.c
//  subjust
//  
//  Created by gongqijian on 2012-12-25.
//  Copyright 2012 gongqijian. All rights reserved.
//

#include <unistd.h>
#include <stdio.h>
#include "stream.h"
#include "debug.h"

typedef struct {
    char *bom;
    int len;
    bom_type_e typ;
} bom_t;

static char BOM_GB_18030 [] = { 0x84, 0x31, 0x95, 0x33 };
static char BOM_UTF_8    [] = { 0xEF, 0xBB, 0xBF       }; 
static char BOM_UTF_16_BE[] = { 0xFE, 0xFF             };
static char BOM_UTF_16_LE[] = { 0xFF, 0xFE             };
static char BOM_UTF_32_BE[] = { 0x00, 0x00, 0xFE, 0xFF };
static char BOM_UTF_32_LE[] = { 0xFF, 0xFE, 0x00, 0x00 };

int readln_stdio(FILE* fp, char* buf, int size)
{
    char *p = buf;

    while(1)
    {
        if (p - buf > size)
        {
            trace("buffer overflow. size：%d, data:%s", size, buf);
            return -1;
        }

        *p = getchar();
        if ('\n' == *p || EOF == *p)
            break;

        p++;
    }

    return p - buf;
}

int writeln_stdio(FILE* fp,char* data, int len)
{
    printf("%s", data);
    return len;
}

int readln_file(FILE* fp, char* buf, int size)
{
    // 先这样吧
    if (NULL != fgets(buf, size, fp))
        return strlen(buf);
    else
        return 0;
}

int writeln_file(FILE* fp, char* data, int len)
{
    // 先这样吧
    return fputs(data, fp);
}

bom_type_e __skip_bom(FILE* fp)
{
    char buf [5] = {0};
    bom_t boms[] = {
        {BOM_GB_18030  , sizeof(BOM_GB_18030 ), e_bom_gb_18030 },
        {BOM_UTF_8     , sizeof(BOM_UTF_8    ), e_bom_utf_8    },
        {BOM_UTF_16_BE , sizeof(BOM_UTF_16_BE), e_bom_utf_16_be},
        {BOM_UTF_16_LE , sizeof(BOM_UTF_16_LE), e_bom_utf_16_le},
        {BOM_UTF_32_BE , sizeof(BOM_UTF_32_BE), e_bom_utf_32_be},
        {BOM_UTF_32_LE , sizeof(BOM_UTF_32_LE), e_bom_utf_32_le},
    };

    if (fread(buf, sizeof(unsigned char), sizeof(buf), fp))
    {
        int i;
        for (i = 0; i < sizeof(boms)/sizeof(bom_t); i++)
        {
            if (0 == strncmp(buf, boms[i].bom, boms[i].len))
            {
                fseek(fp, boms[i].len, SEEK_SET);
                return boms[i].typ;
            }
        }
    }

    rewind(fp);
    return e_bom_none;
}

int __stdio_stream_init(stream_t* ps)
{ 
    ps->file = NULL;
    ps->readln = readln_stdio;
    ps->writeln = writeln_stdio;
    return 0;
}

int __file_stream_init(stream_t* ps, char* name, char* mod)
{ 
    ps->file = fopen(name, mod); 

    if (!ps->file)
    {
        trace("file '%s' cannot open.\n", name);
        return -1;
    }

    __skip_bom(ps->file);

    ps->readln = readln_file; 
    ps->writeln = writeln_file; 
    return 0;
}

int stream_init(stream_t* ps, io_type_e typ, char* name)
{ 
    return_val_if_fail(ps != NULL, -1);
    memset(ps, 0x00, sizeof(stream_t));

    switch(typ) 
    { 
    case e_input_stdin: 
    case e_output_stdout: 
        return __stdio_stream_init(ps); 
        break;
    case e_input_file:
        if(access(name, F_OK) != 0)
        {
            trace("file '%s' not found.\n", name);
            return -1;
        }
        return __file_stream_init(ps, name, "r"); 
    case e_output_file: 
        return __file_stream_init(ps, name, "w"); 
    default:
        trace("%s", "unknow io type");
        return -1;
    } 
}

void stream_free(stream_t* ps)
{
    if (!ps && !ps->file) 
    { 
        free(ps->file);
        ps->file = NULL; 
    }
}

