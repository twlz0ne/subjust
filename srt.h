// 
//  srt.h
//  subjust
//  
//  Created by gongqijian on 2012-12-25.
//  Copyright 2012 gongqijian. All rights reserved.
// 

#ifndef _SRT_H_
#define _SRT_H_

#include "typedef.h"

extern int srt_delay(void* opt);
extern int srt_split(void* opt);
extern int srt_cat(void* opt);
extern int srt_mix(void* opt);

#endif /* end of include guard: _SRT_H_ */
