// 
//  stream.h
//  subjust
//  
//  Created by gongqijian on 2012-12-25.
//  Copyright 2012 gongqijian. All rights reserved.
//

#ifndef _STREAM_H_
#define _STREAM_H_

#include "typedef.h"

typedef enum {
     e_bom_none = 0,
     e_bom_gb_18030,
     e_bom_utf_8, 
     e_bom_utf_16_be,
     e_bom_utf_16_le,
     e_bom_utf_32_be,
     e_bom_utf_32_le
} bom_type_e;

extern int readln_stdio(FILE* fp, char* buf, int size);
extern int writeln_stdio(FILE* fp,char* data, int len);

extern int readln_file(FILE* fp, char* buf, int size);
extern int writeln_file(FILE* fp, char* data, int len);

extern int stream_init(stream_t* a, io_type_e typ, char* name);
extern void stream_free(stream_t* ps);

#endif /* _STREAM_H_ */

