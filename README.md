subjust
=======

a simple command line tool for subtitle adjustment.

一个命令行小工具，用来对字幕进行简单的延时(-d delay)、切割(-s split)、合并(-c concat)和混合(-m mix)操作，目前仅支持 .srt 格式，其它格式的支持以后再说 :P

## 命令格式

    $ ./subjust
    Useage: ./a.out [-t <time>] [-f <format>] [-<action>] file...
    Time:
        -t <time>       hh:mm:ss,fff or ss.fff
    Format:
        -f <format>     srt, idx, ssa...
    Action:
        -d 	            delay
        -s 	            split
        -c 	            cat
        -m 	            mix
    Example:
        ./subjust -t 12.345 -f srt -d input.srt output.srt
	
## 字幕延时

    $ ./subjust -t 12.345 -f srt -d input.srt output.srt
    
如果输入文件名带有后缀，则选项 -f 可省略。
    
## 切割字幕

    $ ./subjust -t 12.345 -f srt -s input.srt output1.srt ouput2.srt
 
 选项 -t 用于指定从何时开始切割，也用于在切割过程中对 output2.srt 进行自动延时。虽然对 output2.srt 进行了延时操作，但恐怕还需对它进行一次 -d 操作，才能完全与视频匹配。
 
## 合并字幕

    $ ./subjust -t 12.345 -f srt -c input1.srt input2.srt output.srt

选项 -t 用于校准合并后由 input2.srt 输入的字幕的时间，如不指定，则取 input1.srt 最后的时间作为缺省值。最好先把视频合并了，再来确定 -t 的时间。

## 混合字幕

    $ ./subjust -f srt -m input.chs.srt input.eng.srt ouput.mix.srt

用来混合不同语言的字幕，输入的两个文件行号必须一致，时间以第一个文件为准。
