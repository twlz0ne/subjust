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
            printf("file '%s' not found.\n", name);
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

