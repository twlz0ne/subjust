// 
//  debug.h
//  subjust
//  
//  Created by gongqijian on 2013-01-22
//  Copyright 2012 gongqijian. All rights reserved.
//

#ifndef _DEBUG_H_
#define _DEBUG_H_

#define return_if_fail(p) if(!(p))                                      \
    {                                                                   \
        printf("%s:%d Warning: "#p" failed.\n",  __func__, __LINE__);   \
        return;                                                         \
    }

#define return_val_if_fail(p, ret) if(!(p))                             \
    {                                                                   \
        printf("%s:%d Warning: "#p" failed.\n", __func__, __LINE__);    \
        return (ret);                                                   \
    }

#define trace(format, args...)                                     \
    printf("[%s:%d] %s() "format"\n", __FILE__, __LINE__, __func__, args)

#define trace_buf(buf, len)                                     \
    {                                                           \
        int __i;                                                \
        unsigned char* __p = (buf);                             \
        printf("[%s: %d] %s() buf=%s, size=%d:",                \
               __FILE__, __LINE__, __func__, #buf, (int)len);   \
        for (__i = 0; __i < len; __i++, __p++)                  \
        {                                                       \
            if (__i % 16 == 0)                                  \
                printf("\n%p:", __p);                           \
            printf(" %02x", *__p);                              \
        }                                                       \
        printf("\n");                                           \
    }

#endif /* _DEBUG_H_*/
