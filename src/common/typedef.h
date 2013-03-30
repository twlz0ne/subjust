// 
//  typedef.h
//  subjust
//  
//  Created by gongqijian on 2012-12-25.
//  Copyright 2012 gongqijian. All rights reserved.
// 

#ifndef _TYPEDEF_H_
#define _TYPEDEF_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define OUT 

#ifndef NULL
#define NULL 0
#endif

#ifndef CR
#define CR '\r'
#endif

#ifndef LF
#define LF '\n'
#endif

#define NOERR 0
#define BOOL int
#define TRUE 1
#define FALSE 0

typedef int (*readln_func_t) (FILE* fp, char* buf, int size);
typedef int (*writeln_func_t) (FILE* fp, char* data, int len);

typedef struct {
    FILE* file;
    readln_func_t readln;
    writeln_func_t writeln;
}stream_t;

typedef enum {
    e_input_stdin,
    e_input_file,	
    e_output_stdout,
    e_output_file
} io_type_e;

typedef enum {
    e_format_unknow,
    e_format_srt,
    e_format_idx,
    e_format_ssa
} sub_format_e;

typedef struct {
    float delay_time;
    io_type_e input_type;
    char* finput;
    io_type_e output_type;
    char* foutput;
} delay_option_t;

typedef struct {
    char* finput;
    io_type_e input_type;
    char* foutput1;
    io_type_e output1_type;
    char* foutput2;
    io_type_e output2_type;
    float split_time;
} split_option_t;

typedef struct {
    char* finput1;
    io_type_e input1_type;
    char* finput2;
    io_type_e input2_type;
    char* foutput;
    io_type_e output_type;
    float delay_time;
} cat_option_t;

typedef struct {
    char* finput1;
    io_type_e input1_type;
    char* finput2;
    io_type_e input2_type;
    char* foutput;
    io_type_e output_type;
} mix_option_t;

typedef int (*ofsset_func_t) (void* opt);
typedef int (*split_func_t) (void* opt);
typedef int (*cat_func_t) (void* opt);
typedef int (*mix_func_t) (void* opt);

typedef struct {
    const char* name;
    ofsset_func_t delay;
    split_func_t split;
    cat_func_t cat;
    mix_func_t mix;
} processor_t;

typedef enum {
    e_action_unknow,
    e_action_delay,
    e_action_split,
    e_action_cat,
    e_action_mix
} action_e;

#endif /* end of include guard: _TYPEDEF_H_ */
