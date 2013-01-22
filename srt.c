// 
//  srt.c
//  subjust
//  
//  Created by gongqijian on 2012-12-25.
//  Copyright 2012 gongqijian. All rights reserved.
//

#include "srt.h"
#include "debug.h"
#include "stream.h"

typedef enum {
    bs_number,
    bs_time,
    bs_text
} block_status_e;

#define MAX_NUMBER_SIZE 8
#define MAX_TIME_SIZE 32
#define MAX_TEXT_SIZE 512
#define MAX_BLANK_SIZE 4

typedef struct {
    char number[MAX_NUMBER_SIZE];
    int number_len;
    char time[MAX_TIME_SIZE];
    int time_len;
    char text[MAX_TEXT_SIZE];
    int text_len;
    char blank[MAX_BLANK_SIZE];
    int blank_len;
} block_t;

int __str2time(const char* str, float* t1, float* t2)
{
    int h1 = -1, m1 = -1, s1 = -1, f1 = -1;
    int h2 = -1, m2 = -1, s2 = -1, f2 = -1;
    
    sscanf(str, "%d:%d:%d%*c%d --> %d:%d:%d%*c%d",
           &h1, &m1, &s1, &f1, 
           &h2, &m2, &s2, &f2);
    
    if ( h1 < 0 || m1 < 0 || s1 < 0 || f1 < 0
         || h2 < 0 || m2 < 0 || s2 < 0 || f2 < 0)
        return -1;
    
    *t1 = (float)f1/1000 + s1 + 60*m1 + 3600*h1;
    *t2 = (float)f2/1000 + s2 + 60*m2 + 3600*h2;
    
    return 0;
}

int __time2str(char* buf, float t1, float t2)
{ 
    int h1 = -1, m1 = -1, s1 = -1, f1 = -1;
    int h2 = -1, m2 = -1, s2 = -1, f2 = -1;

    h1 = (int)t1/3600;
    m1 = ((int)t1-h1*3600)/60;
    s1 = (int)t1-h1*3600-m1*60;
    f1 = (t1-(float)h1*3600-(float)m1*60-(float)s1)*1000;

    h2 = (int)t2/3600;
    m2 = ((int)t2-h2*3600)/60;
    s2 = (int)t2-h2*3600-m2*60;
    f2 = (t2-(float)h2*3600-(float)m2*60-(float)s2)*1000;
    
    sprintf(buf, "%02d:%02d:%02d,%03d --> %02d:%02d:%02d,%03d\n", 
            h1, m1, s1, f1, 
            h2, m2, s2, f2);

    return 0;
}

BOOL __is_number(const char* str)
{
    int len = strlen(str);
    int i;

    for(i = 0; i < len; i++)
    {
        if ('\r' == str[i] || '\n' == str[i] || ' ' == str[i])
            break;

        if ('0' > str[i] || '9' < str[i])
            return FALSE;
    }

    return (i >= 1) ? TRUE : FALSE;
}

BOOL __is_time(const char* str)
{
    float t1 = -1.0f, t2 = -1.0f;
    return (NOERR == __str2time(str, &t1, &t2)) ? TRUE : FALSE;
}

BOOL __is_blank(const char* str)
{
    int len = strlen(str);
    int i;

    for(i = 0; i < len; i++)
    {
        if ('\r' == str[i] || '\n' == str[i])
            break;

        if (' ' != str[i])
            return FALSE;
    }

    return TRUE;
}

// output: bytes count
// return: -1 or NOERR
int __read_block(readln_func_t readln, FILE* fp, block_t* blockp)
{
    block_status_e bs_state = bs_number;
    char* ptext = blockp->text;
    int ret;

    while(1)
    {
        switch(bs_state)
        {
        case bs_number:
            // 读取序号时遇到文件结尾，正常，返回 NOERR
            blockp->number_len = readln(fp, blockp->number, MAX_NUMBER_SIZE);
            if (blockp->number_len <= 0)
                return NOERR;

            if (FALSE == __is_number(blockp->number))
                return -1;

            bs_state++;
            break;

        case bs_time:
            // 读取时间时遇到文件结尾，块错误，返回 -1
            blockp->time_len = readln(fp, blockp->time, MAX_TIME_SIZE);
            if (blockp->time_len <= 0)
                return -1;

            if (FALSE == __is_time(blockp->time))
                return -1;

            bs_state++;
            break;

        case bs_text:
            ret = readln(fp, ptext, MAX_TEXT_SIZE - (ptext - blockp->text));
            // 如果文件结束时(即 ret==0)，text 内容仍为空，则认为块错误，返回 -1
            if (ret <= 0)
                return (ptext == blockp->text) ? -1 : NOERR;

            if (TRUE == __is_blank(ptext))
            {
                // blockp->blank 空间有限，如果有多余的空格，必须剔除
                sscanf(ptext, "%[\r\n]", blockp->blank);
                *ptext = '\0';
                blockp->blank_len = strlen(blockp->blank);
                return NOERR;
            }

             blockp->text_len += ret;
             ptext += ret;
             break;
         }
     }
 }

 // return : -1 or NOERR
 int __write_block(writeln_func_t writeln, FILE* fp, block_t* blockp)
 {
     int ret = 0;

     //trace("number: %s", blockp->number);
     ret = writeln(fp, blockp->number, blockp->number_len);
     return_val_if_fail(ret >= 0, -1);

     //trace("time: %s", blockp->time);
     ret = writeln(fp, blockp->time, blockp->time_len);
     return_val_if_fail(ret >= 0, -1);

     //trace("text: %s", blockp->text);
     ret = writeln(fp, blockp->text, blockp->text_len);
     return_val_if_fail(ret >= 0, -1);

     //trace("blank: %s", blockp->blank);
     ret = writeln(fp, blockp->blank, blockp->blank_len);
     return_val_if_fail(ret >= 0, -1);

     return NOERR;
 }

 int srt_delay(void* opt)
 {
     delay_option_t* op = NULL;
     stream_t si, so;
     float t1, t2;
     block_t block;
     int err = NOERR;

     return_val_if_fail(opt, -1);
     op = (delay_option_t*)opt;

     if (NOERR == (err = stream_init(&si, op->input_type, op->finput))
         && NOERR == (err = stream_init(&so, op->output_type, op->foutput)))
     {
         while(1)
         {
             memset(&block, 0x00, sizeof(block_t));

             // read block

             if (NOERR != (err = __read_block(si.readln, si.file, &block)))
             {
                 trace("%s", "read block err");
                 break;
             }

             if (block.number_len <= 0)
                 break; // file end

             if (NOERR != (err = __str2time(block.time, &t1, &t2)))
             {
                 trace("%s", "parse time err");
                 break;
             }

             // set time

             t1 += op->delay_time;
             t2 += op->delay_time;
             __time2str(block.time, t1, t2);
             block.time_len = strlen(block.time);

             // write block

             if (NOERR != (err = __write_block(so.writeln, so.file, &block)))
             {
                 trace("%s", "write block err");
                 break;
             }
         }
     }

     stream_free(&si);
     stream_free(&so);

     return err;
 }

 int srt_split(void* opt)
 {
     split_option_t* opp = NULL;
     stream_t si, so1, so2;
     float t1, t2;
     int line_number = 1;
     block_t block;
     int err = NOERR;

     return_val_if_fail(opt, -1);
     opp = (split_option_t*)opt;

     if (NOERR == (err = stream_init(&si, opp->input_type, opp->finput))
         && NOERR == (err =  stream_init(&so1, opp->output1_type, opp->foutput1))
         && NOERR == (err = stream_init(&so2, opp->output2_type, opp->foutput2)))
     {
         while(1)
         {
             memset(&block, 0x00, sizeof(block_t));

             // read block

             if (NOERR != (err =__read_block(si.readln, si.file, &block)))
             {
                 trace("%s", "read block err");
                 break;
             }

             if (block.number_len <= 0)
                 break; // file end

             if (NOERR != (err = __str2time(block.time, &t1, &t2)))
             {
                 trace("%s", "parse time err");
                 break;
             }

             // write blocks

             if (t1 < opp->split_time)
             {
                 // part1
                 // no need change
                 if (NOERR != (err = __write_block(so1.writeln, so1.file, &block)))
                 {
                     trace("%s", "write block err (part1)");
                     break;
                 }   
             }
             else
             {
                 // part2
                 // set time & line number
                 __time2str(block.time, t1 - opp->split_time, t2 - opp->split_time);
                 block.time_len = strlen(block.time);

                 sprintf(block.number, "%d\n", line_number++);
                 block.number_len = strlen(block.number);

                 if (NOERR != (err = __write_block(so2.writeln, so2.file, &block)))
                 {
                     trace("%s", "write block err (part2)");
                     break;
                 }       
             }
         }
     }

     stream_free(&si);
     stream_free(&so1);
     stream_free(&so2);

     return err;
 }

 int srt_cat(void* opt)
 {
     cat_option_t* opp = NULL;
     stream_t si1, si2, so;
     block_t block;
     float t1 = -1.0f, t2 = -1.0f, time_delay;
     int line_number = 1;
     int err = NOERR;

     return_val_if_fail(opt, -1);
     opp = (cat_option_t*)opt;

     if (NOERR == (err = stream_init(&si1, opp->input1_type, opp->finput1))
         && NOERR == (err = stream_init(&si2, opp->input2_type, opp->finput2))
         && NOERR == (err = stream_init(&so, opp->output_type, opp->foutput)))
     {
         while(1)
         {
             memset(&block, 0x00, sizeof(block_t));

             // read block

             if (NOERR != (err = __read_block(si1.readln, si1.file, &block)))
            {
                trace("%s", "readblock err [file 1]");
                break;
            }

            if (block.number_len <= 0)
                break; // file end

            // get time & line number

            if (NOERR != (err = __str2time(block.time, &t1, &t2)))
            {
                trace("%s", "parse time err [file 1]");
                break;
            }

            line_number++;

            // write block

            if (NOERR != (err = __write_block(so.writeln, so.file, &block)))
            {
                trace("%s", "write block err [file 1]");
                break;
            }
        }

        time_delay =  (opp->delay_time > 0.0f) ? opp->delay_time : t1;

        while(1)
        {
            memset(&block, 0x00, sizeof(block_t));

            // read block

            if (NOERR != (err = __read_block(si2.readln, si2.file, &block)))
            {
                trace("%s", "readblock err [file 2]");
                break;
            }

            if (block.number_len <= 0)
                break; // file end

            if (NOERR != (err = __str2time(block.time, &t1, &t2)))
            {
                trace("%s", "parse time err [file 2]");
                break;
            }

            // set time & line number

            __time2str(block.time, t1 + time_delay, t2 + time_delay);
            block.time_len = strlen(block.time);

            sprintf(block.number, "%d\n", line_number++);
            block.number_len = strlen(block.number);

           // write block

            if (NOERR != (err = __write_block(so.writeln, so.file, &block)))
            {
                trace("%s", "write block err [file 2]");
                break;
            }
        }
    }

    stream_free(&si1);
    stream_free(&si2);
    stream_free(&so);

    return err;
}

int srt_mix(void* opt)
{
    mix_option_t* op = NULL;
    stream_t si1, si2, so;
    block_t block1, block2;
    int number1, number2, err = NOERR;

    return_val_if_fail(opt, -1);
    op = (mix_option_t*)opt;

    if (NOERR == (err = stream_init(&si1, op->input1_type, op->finput1))
        && NOERR == (err = stream_init(&si2, op->input2_type, op->finput2))
        && NOERR == (err = stream_init(&so, op->output_type, op->foutput)))
    {
        while(1)
        {
            memset(&block1, 0x00, sizeof(block_t));
            memset(&block2, 0x00, sizeof(block_t));
            
            // read blocks

            if (NOERR != (err = __read_block(si1.readln, si1.file, &block1)))
            {
                trace("%s", "readblock err [file 1]");
                break;
            }

            if (block1.number_len <= 0)
                break; // file end

            if (NOERR != (err = __read_block(si2.readln, si2.file, &block2)))
            {
                trace("%s", "readblock err [file 2]");
                break;
            }

            if (block2.number_len <= 0)
                break; // file end

            // mix blocks

            number1 = -1;
            number2 = -1;
            sscanf(block1.number, "%d", &number1);
            sscanf(block2.number, "%d", &number2);
            if (number1 != -1 && number1 != number2)
            {
                err = -1;
                trace("number err, file1:%d, file2:%d", number1, number2);
                break;
            }
            strcat(block1.text, block2.text);
            block1.text_len = strlen(block1.text);

            // write block

            if (NOERR != (err = __write_block(so.writeln, so.file, &block1)))
            {
                trace("%s", "write block err");
                break;
            }
        }
    }

    stream_free(&si1);
    stream_free(&si2);
    stream_free(&so);

    return err;
}

