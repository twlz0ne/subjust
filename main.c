// 
//  main.c
//  subjust
//  
//  Created by gongqijian on 2012-12-25.
//  Copyright 2012 gongqijian. All rights reserved.
// 

#include <getopt.h>
#include "typedef.h"
#include "stream.h"
#include "srt.h"

#define PROCESSOR(a) { #a, a ## _delay, a ## _split, a ## _cat, a ## _mix},
processor_t s_processors[] = {
	#include "processor_table.h"
};
#undef PROCESSOR

processor_t* get_processor(char* name)
{
    int i, n = sizeof(s_processors)/sizeof(processor_t);

    if (NULL != name)
    {
        for (i = 0; i < n; i++)
        {
            if (strcmp(name, s_processors[i].name) == 0)
                return &s_processors[i];
        }
    }

    return NULL;
}

int str2time(const char* str, float* val)
{
    int hh = -1, mm = -1, ss = -1,ff = -1;
    char* pstr = (char*)str;
     
    return_val_if_fail(str, -1.0f);

    if ('-' == *pstr)
        pstr++;

    sscanf(pstr, "%d%*c%d%*c%d%*c%d", &hh, &mm, &ss, &ff);

    if (hh >= 0 && mm >= 0 && ss >= 0 && ff >= 0)
        *val = (float)ff/1000 + ss + mm*60 + hh*3600;
    else if (hh >= 0 && mm >= 0 && ss < 0 && ff < 0)
        *val = atof(pstr);
    else if (hh >= 0 && mm < 0 && ss < 0 && ff < 0)
        *val = (float)atoi(pstr);
    else
        return -1;

    if ('-' == *str)
        *val = 0 - *val;
    
    return 0;
}

char* get_format(const char* filename)
{
    return (strrchr(filename, '.') + 1);
}

void useage(char* appname)
{
    printf("\nUseage: %s [-t <time>] [-f <format>] [-<action>] file...\n\
Time:\n\
    -t <time>       hh:mm:ss,fff or ss.fff\n\
Format:\n\
    -f <format>     srt, idx, ssa...\n\
Action:\n\
    -d 	            delay\n\
    -s 	            split\n\
    -c 	            cat\n\
    -m 	            mix\n\
Example:\n\
    %s -t 12.345 -f srt -d input.srt output.srt\n", appname, appname);
}

int main(int argc, char** argv)
{
    int err = NOERR;
    char optchr;
    processor_t* proc = NULL;
    void* subopt = NULL;
    char* fmt = NULL;
    float time = 0.0f;
    char* optstr = "t:f:d:s:c:m:";
    action_e act = e_action_unknow;

    if (argc <= 1)
    {
        useage(argv[0]);
        return -1;
    }

    while(-1 != (optchr = getopt(argc, argv, optstr)))
    {
        switch(optchr) 
        {
        case 't': // time
            if (0 != str2time(optarg, &time))
            {
                trace("time convert erro : %0.3f", time);
                return __LINE__;
            }
            break;

        case 'f': // format
            fmt = optarg;
            break;

        case 'd':
        {
            delay_option_t op = {0};
            op.delay_time = time;

            if (NULL != optarg)
            {
                op.finput = optarg;
                op.input_type = e_input_file;
                if (NULL == fmt)
                    fmt = get_format(op.finput);
            }
            else
                op.input_type = e_input_stdin;

            if (NULL != argv[optind])
            {
                op.foutput = argv[optind++];
                op.output_type = e_output_file;
            }
            else
                op.output_type = e_output_stdout;

            act = e_action_delay;
            subopt = (void*)&op;
        }
        break;

        case 's':
        {
            split_option_t op = {0};
            op.split_time = time;

            if (NULL != optarg)
            {
                op.finput = optarg;
                op.input_type = e_input_file;
                if (NULL == fmt)
                    fmt = get_format(op.finput);
            }
            else
                op.input_type = e_input_stdin;

            if (NULL != argv[optind])
            {
                op.foutput1 = argv[optind++];
                op.output1_type = e_output_file;
            }
            else
                op.output1_type = e_output_stdout;

            if (NULL != argv[optind])
            {
                op.foutput2 = argv[optind++];
                op.output2_type = e_output_file;
            }
            else
                op.output2_type = e_output_stdout;

            act = e_action_split;
            subopt = (void*)&op;
        }
        break;

        case 'c':
        {
            cat_option_t op = {0};
            op.delay_time = time;

            if (NULL != optarg)
            {
                op.finput1 = optarg;
                op.input1_type = e_input_file;
                if (NULL == fmt)
                    fmt = get_format(op.finput1);
            }
            else
            {
                printf("requires first input file\n");
                return __LINE__;
            }

            if (NULL != argv[optind])
            {
                op.finput2 = argv[optind++];
                op.input2_type = e_input_file;
            }
            else
                op.input2_type = e_input_stdin;

            if (NULL != argv[optind])
            {
                op.foutput = argv[optind++];
                op.output_type = e_output_file;
            }
            else
                op.output_type = e_output_stdout;

            act = e_action_cat;
            subopt = (void*)&op;
        }
        break;

        case 'm':
        {
            mix_option_t op = {0};
            
            if (NULL != optarg)
            {
                op.finput1 = optarg;
                op.input1_type = e_input_file;
                if (NULL == fmt)
                    fmt = get_format(op.finput1);
            }
            else
            {
                printf("requires first input file\n");
                return __LINE__;
            }


            if (NULL != argv[optind])
            {
                op.finput2 = argv[optind++];
                op.input2_type = e_input_file;
            }
            else
                op.input2_type = e_input_file;

            if (NULL != argv[optind])
            {
                op.foutput = argv[optind++];
                op.output_type = e_output_file;
            }
            else
                op.output_type = e_output_stdout;

            act = e_action_mix;
            subopt = (void*)&op;
        }
        break;

        default:
            useage(argv[0]);
            return __LINE__;
        }
    }

    proc = get_processor(fmt);

	if (NULL == proc)
    {
        printf("unknow format:'%s'\n", fmt);
        useage(argv[0]);
        return __LINE__;
    }
    
    if (NULL == subopt)
    {
        printf("requires -<action> option\n");
        useage(argv[0]);
        return __LINE__;
    }
    
    switch(act)
    {
    case e_action_delay:
        err = proc->delay(subopt);
        break;
    case e_action_split:
        err = proc->split(subopt);
        break;
    case e_action_cat:
        err = proc->cat(subopt);
        break;
    case e_action_mix:
        err = proc->mix(subopt);
        break;
    default:
        trace("unknow action:'%d'", act);
    }

    if (err != NOERR)
        printf("err:%d, fmt:%s, act:%d\n", err, fmt, act);

	return err;
}
