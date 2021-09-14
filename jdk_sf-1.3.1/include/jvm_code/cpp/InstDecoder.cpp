/***
 JVM译码器
 注册JVM指令集
***/
#pragma once//防止重复定义

#include "..\headers\InstDecoder.h"

//获取指令名称， 将16进值操作码翻译成对应助记符
char* getInstName(u1 index){
	
//        操 作 码    |      助 记 符                     功能

//Constants指令--------------------------------------------
	if(index==0x00) {return "nop\0"; }						//无操作
	if(index==0x01) {return "aconst_null\0"; }				//null进栈（栈 操作栈，下同）
	if(index==0x02) {return "iconst_m1\0"; }				//int型常量值-1进栈
	if(index==0x03) {return "iconst_0\0"; }				//int型常量值0进栈
	if(index==0x04) {return "iconst_1\0"; }				//int型常量值1进栈
	if(index==0x05) {return "iconst_2\0"; }				//int型常量值2进栈
	if(index==0x06) {return "iconst_3\0"; }				//int型常量值3进栈
	if(index==0x07) {return "iconst_4\0"; }				//int型常量值4进栈
	if(index==0x08) {return "iconst_5\0"; }				//int型常量值5进栈

	if(index==0x09) {return "lconst_0\0"; }				//long型常量值0进栈
	if(index==0x0a) {return "lconst_1\0"; }				//long型常量值1进栈

	if(index==0x0b) {return "fconst_0\0"; }				//float型常量值0进栈
	if(index==0x0c) {return "fconst_1\0"; }				//float型常量值1进栈
	if(index==0x0d) {return "fconst_2\0"; }				//float型常量值2进栈

	if(index==0x0e) {return "dconst_0\0"; }				//double型常量值0进栈
	if(index==0x0f) {return "dconst_1\0"; }				//double型常量值1进栈

	if(index==0x10) {return "bipush\0"; }					//将byte型常量推送栈顶
	if(index==0x11) {return "sipush\0"; }					//将short型常量推送栈顶
	
	if(index==0x12) {return "ldc\0"; }						//将int，float或String型常量从常量池推送栈顶
	if(index==0x13) {return "ldc_w\0"; }					//同上（宽索引）
	if(index==0x14) {return "ldc2_w\0"; }					//同上（宽索引）
	
//Loads指令--------------------------------------------

	if(index==0x15) {return "iload\0"; }					//指定的int型局部变量进栈
	if(index==0x16) {return "lload\0"; }					//指定的long型变量进栈
	if(index==0x17) {return "fload\0"; }					//指定的float型变量进栈
	if(index==0x18) {return "dload\0"; }					//指定的double型局部变量进栈
	if(index==0x19) {return "aload\0"; }					//指令格式)aload index)将当前栈帧局部变量数组中下标为index的引用变量进栈。index是u1类型
	
	if(index==0x1a) {return "iload_0\0"; }					//将第1个int型局部变量进栈
	if(index==0x1b) {return "iload_1\0"; }					//将第2个int型局部变量进栈
	if(index==0x1c) {return "iload_2\0"; }					//将第3个int型局部变量进栈
	if(index==0x1d) {return "iload_3\0"; }					//将第4个int型局部变量进栈

	if(index==0x1e) {return "lload_0\0"; }					//将第1个long型局部变量进栈
	if(index==0x1f) {return "lload_1\0"; }					//将第2个long型局部变量进栈
	if(index==0x20) {return "lload_2\0"; }					//将第3个long型局部变量进栈
	if(index==0x21) {return "lload_3\0"; }					//将第4个long型局部变量进栈

	if(index==0x22) {return "fload_0\0"; }					//将第1个float型局部变量进栈
	if(index==0x23) {return "fload_1\0"; }					//将第2个float型局部变量进栈
	if(index==0x24) {return "fload_2\0"; }					//将第3个float型局部变量进栈
	if(index==0x25) {return "fload_3\0"; }					//将第4个float型局部变量进栈

	if(index==0x26) {return "dload_0\0"; }					//将第1个double型局部变量进栈
	if(index==0x27) {return "dload_1\0"; }					//将第2个double型局部变量进栈
	if(index==0x28) {return "dload_2\0"; }					//将第3个double型局部变量进栈
	if(index==0x29) {return "dload_3\0"; }					//将第4个double型局部变量进栈

	if(index==0x2a) {return "aload_0\0"; }					//等同于aload 0
	if(index==0x2b) {return "aload_1\0"; }					//同上
	if(index==0x2c) {return "aload_2\0"; }					//同上
	if(index==0x2d) {return "aload_3\0"; }					//同上

	if(index==0x2e) {return "iaload\0"; }					//指令格式：iaload index。将当前栈帧局部变量数组中下标为index的int型数组进栈
	if(index==0x2f) {return "laload\0"; }					//同上，long数组进栈
	if(index==0x30) {return "faload\0"; }					//同上，float数组进栈
	if(index==0x31) {return "daload\0"; }					//同上，double数组进栈
	if(index==0x32) {return "aaload\0"; }					//数组引用出栈，数组元素值进栈
	if(index==0x33) {return "baload\0"; }					//同iload。byte或boolean数组进栈
	if(index==0x34) {return "caload\0"; }					//同上，char数组进栈
	if(index==0x35) {return "saload\0"; }					//同上，short数组进栈

//Stores指令--------------------------------------------

	if(index==0x36) {return "istore\0"; }					//将栈顶int型数值存入指定局部变量
	if(index==0x37) {return "lstore\0"; }					//同上
	if(index==0x38) {return "fstore\0"; }					//同上
	if(index==0x39) {return "dstore\0"; }					//同上
	if(index==0x3a) {return "astore\0"; }					//
	
	if(index==0x3b) {return "istore_0\0"; }				//将栈顶int型存入第1个局部变量
	if(index==0x3c) {return "istore_1\0"; }				//同上
	if(index==0x3d) {return "istore_2\0"; }				//同上
	if(index==0x3e) {return "istore_3\0"; }				//同上

	if(index==0x3f) {return "lstore_0\0"; }				//将栈顶long型存入第1个局部变量
	if(index==0x40) {return "lstore_1\0"; }				//同上
	if(index==0x41) {return "lstore_2\0"; }				//同上
	if(index==0x42) {return "lstore_3\0"; }				//同上

	if(index==0x43) {return "fstore_0\0"; }				//将栈顶float型存入第1个局部变量
	if(index==0x44) {return "fstore_1\0"; }				//同上
	if(index==0x45) {return "fstore_2\0"; }				//同上
	if(index==0x46) {return "fstore_3\0"; }				//同上

	if(index==0x47) {return "dstore_0\0"; }				//将栈顶double型存入第1个局部变量
	if(index==0x48) {return "dstore_1\0"; }				//同上
	if(index==0x49) {return "dstore_2\0"; }				//同上
	if(index==0x4a) {return "dstore_3\0"; }				//同上

	if(index==0x4b) {return "astore_0\0"; }				//同astore 0
	if(index==0x4c) {return "astore_1\0"; }				//同上
	if(index==0x4d) {return "astore_2\0"; }				//同上
	if(index==0x4e) {return "astore_3\0"; }				//同上

	if(index==0x4f) {return "iastore\0"; }					//
	if(index==0x50) {return "lastore\0"; }					//
	if(index==0x51) {return "fastore\0"; }					//
	if(index==0x52) {return "dastore\0"; }					//
	if(index==0x53) {return "aastore\0"; }
	if(index==0x54) {return "bastore\0"; }
	if(index==0x55) {return "castore\0"; }
	if(index==0x56) {return "sastore\0"; }					//

//Stack指令---------------------------------------------
//栈指令

	if(index==0x57) {return "pop\0"; }
	if(index==0x58) {return "pop2\0"; }
	if(index==0x59) {return "dup\0"; }
	if(index==0x5a) {return "dup_x1\0"; }
	if(index==0x5b) {return "dup_x2\0"; }
	if(index==0x5c) {return "dup2\0"; }
	if(index==0x5d) {return "dup2_x1\0"; }
	if(index==0x5e) {return "dup2_x2\0"; }
	if(index==0x5f) {return "swap\0"; }

//Math指令----------------------------------------------
//即运算指令
//1、数据类型：(1)i：int (2)l：long (3)f：float (4)d：double
//2、操作指令：(1)add)加 (2)sub)减 (3)mul)乘 (4)div)除 (5)rem)求余 (6)neg)取反 (7)shl)左位移 (8)shr)右位移 (9)and)与 (10)or)或 (11)xor异或 (12)inc)自加
//iadd表示int相加。其余以此类推

	if(index==0x60) {return "iadd\0"; }
	if(index==0x61) {return "ladd\0"; }
	if(index==0x62) {return "fadd\0"; }
	if(index==0x63) {return "dadd\0"; }
	if(index==0x64) {return "isub\0"; }
	if(index==0x65) {return "lsub\0"; }
	if(index==0x66) {return "fsub\0"; }
	if(index==0x67) {return "dsub\0"; }
	if(index==0x68) {return "imul\0"; }
	if(index==0x69) {return "lmul\0"; }
	if(index==0x6a) {return "fmul\0"; }
	if(index==0x6b) {return "dmul\0"; }
	if(index==0x6c) {return "idiv\0"; }
	if(index==0x6d) {return "ldiv\0"; }
	if(index==0x6e) {return "fdiv\0"; }
	if(index==0x6f) {return "ddiv\0"; }
	if(index==0x70) {return "irem\0"; }
	if(index==0x71) {return "lrem\0"; }
	if(index==0x72) {return "frem\0"; }
	if(index==0x73) {return "drem\0"; }
	if(index==0x74) {return "ineg\0"; }
	if(index==0x75) {return "lneg\0"; }
	if(index==0x76) {return "fneg\0"; }
	if(index==0x77) {return "dneg\0"; }
	if(index==0x78) {return "ishl\0"; }
	if(index==0x79) {return "lshl\0"; }
	if(index==0x7a) {return "ishr\0"; }
	if(index==0x7b) {return "lshr\0"; }
	if(index==0x7c) {return "iushr\0"; }
	if(index==0x7d) {return "lushr\0"; }
	if(index==0x7e) {return "iand\0"; }
	if(index==0x7f) {return "land\0"; }
	if(index==0x80) {return "ior\0"; }
	if(index==0x81) {return "lor\0"; }
	if(index==0x82) {return "ixor\0"; }
	if(index==0x83) {return "lxor\0"; }
	if(index==0x84) {return "iinc\0"; }

//Conversions指令---------------------------------------
//即数据转化指令

	if(index==0x85) {return "i2l\0"; }
	if(index==0x86) {return "i2f\0"; }
	if(index==0x87) {return "i2d\0"; }
	if(index==0x88) {return "l2i\0"; }
	if(index==0x89) {return "l2f\0"; }
	if(index==0x8a) {return "l2d\0"; }
	if(index==0x8b) {return "f2i\0"; }
	if(index==0x8c) {return "f2l\0"; }
	if(index==0x8d) {return "f2d\0"; }
	if(index==0x8e) {return "d2i\0"; }
	if(index==0x8f) {return "d2l\0"; }
	if(index==0x90) {return "d2f\0"; }
	if(index==0x91) {return "i2b\0"; }
	if(index==0x92) {return "i2c\0"; }
	if(index==0x93) {return "i2s\0"; }

//Comparisons指令---------------------------------------
//即比较指令

	if(index==0x94) {return "lcmp\0"; }
	if(index==0x95) {return "fcmpl\0"; }
	if(index==0x96) {return "fcmpg\0"; }
	if(index==0x97) {return "dcmpl\0"; }
	if(index==0x98) {return "dcmpg\0"; }
	if(index==0x99) {return "ifeq\0"; }
	if(index==0x9a) {return "ifne\0"; }
	if(index==0x9b) {return "iflt\0"; }
	if(index==0x9c) {return "ifge\0"; }
	if(index==0x9d) {return "ifgt\0"; }
	if(index==0x9e) {return "ifle\0"; }
	if(index==0x9f) {return "if_icmpeq\0"; }
	if(index==0xa0) {return "if_icmpne\0"; }
	if(index==0xa1) {return "if_icmplt\0"; }
	if(index==0xa2) {return "if_icmpge\0"; }
	if(index==0xa3) {return "if_icmpgt\0"; }
	if(index==0xa4) {return "if_icmple\0"; }
	if(index==0xa5) {return "if_acmpeq\0"; }
	if(index==0xa6) {return "if_acmpne\0"; }

//Control指令-------------------------------------------
//即控制指令

	if(index==0xa7) {return "goto\0"; }
	if(index==0xa8) {return "jsr\0"; }
	if(index==0xa9) {return "ret\0"; }
	if(index==0xaa) {return "tableswitch\0"; }
	if(index==0xab) {return "lookupswitch\0"; }
	if(index==0xac) {return "ireturn\0"; }
	if(index==0xad) {return "lreturn\0"; }
	if(index==0xae) {return "freturn\0"; }
	if(index==0xaf) {return "dreturn\0"; }
	if(index==0xb0) {return "areturn\0"; }
	if(index==0xb1) {return "return\0"; }

//References指令----------------------------------------
//即引用指令,用于类和对象

	if(index==0xb2) {return "getstatic\0"; }
	if(index==0xb3) {return "putstatic\0"; }
	if(index==0xb4) {return "getfield\0"; }
	if(index==0xb5) {return "putfield\0"; }
	if(index==0xb6) {return "invokevirtual\0"; }			//其他
	if(index==0xb7) {return "invokespecial\0"; }			//调用无须动态绑定的实例方法(构造函数、私有方法和通过super调用的超类方法)
	if(index==0xb8) {return "invokestatic\0"; }				//调用静态方法
	if(index==0xb9) {return "invokeinterface\0"; }			//调用针对接口类型的引用调用方法
	if(index==0xba) {return "invokedynamic\0"; }			//暂不讨论
	if(index==0xbb) {return "new\0"; }						//新建对象
	if(index==0xbc) {return "newarray\0"; }
	if(index==0xbd) {return "anewarray\0"; }
	if(index==0xbe) {return "arraylength\0"; }
	if(index==0xbf) {return "athrow\0"; }
	if(index==0xc0) {return "checkcast\0"; }
	if(index==0xc1) {return "instanceof\0"; }
	if(index==0xc2) {return "monitorenter\0"; }
	if(index==0xc3) {return "monitorexit\0"; }

//Extended指令------------------------------------------
//延伸指令

	if(index==0xc4) {return "wide\0"; }
	if(index==0xc5) {return "multianewarray\0"; }
	if(index==0xc6) {return "ifnull\0"; }
	if(index==0xc7) {return "ifnonnull\0"; }
	if(index==0xc8) {return "goto_w\0"; }
	if(index==0xc9) {return "jsr_w\0"; }

//Reserved指令------------------------------------------
//保留指令

	if(index==0xca) {return "breakpoint\0"; }
	if(index==0xfe) {return "invokenative\0"; }
	if(index==0xff) {return "impdep2\0"; }
	
	return "unkown";
}


JInst* buildInstTable(){
	JInst* insts=(JInst*)calloc(256,sizeof(JInst));
	//为注册指令添加名字
	for(int i=0x00;i<0xff;++i){
		insts[i].name=(u1*)getInstName(i);
	}

	regConstInsts(insts);//注册Constants类指令
	regLoadInsts(insts);//注册Loads类指令
	regStoreInsts(insts);//注册Store类指令
	regStackInsts(insts);//注册Stack类指令
	regMathInsts(insts);//注册Math类指令
	regConversInsts(insts);//注册Conversions类指令
	regComparInsts(insts);//注册Comparisons类指令
	regControlInsts(insts);//注册Control类指令
	regExtendedInsts(insts);//注册Extended类指令
	regReferncesInsts(insts);//注册Refernces类指令
	regReservedInsts(insts);//注册Reserved类指令
	return insts;
}