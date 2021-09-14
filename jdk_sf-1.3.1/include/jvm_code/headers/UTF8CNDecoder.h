/**
utf8中文解码器
例：汉字'中'转换为三个数字 -28,-72,-83进行存储
如何把-28,-72,-83转换成'中'?
这三个数字先+256，变成228，184，173
对应的CN[228][184][174]内容即为汉字'中'
**/
#include "..\headers\UtilTypes.h"

void printfUTF8(b1* value);//输出utf8字符串
b1* decodeUTF8(b1* value);//解码utf8字符串
