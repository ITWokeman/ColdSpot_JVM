#include "..\headers\UtilTypes.h"
#include <iostream>
using namespace std;

#define	ACC_PUBLIC        0x0001 // class field method
#define	ACC_PRIVATE       0x0002 //       field method
#define	ACC_PROTECTED     0x0004 //       field method
#define	ACC_STATIC        0x0008 //       field method
#define	ACC_FINAL         0x0010 // class field method
#define	ACC_SUPER         0x0020 // class
#define ACC_SYNCHRONIZED  0x0020 //             method
#define	ACC_VOLATILE      0x0040 //       field
#define	ACC_BRIDGE        0x0040 //             method
#define	ACC_TRANSIENT     0x0080 //       field
#define	ACC_VARARGS       0x0080 //             method
#define	ACC_NATIVE        0x0100 //             method
#define	ACC_INTERFACE     0x0200 // class
#define	ACC_ABSTRACT      0x0400 // class       method
#define ACC_STRICT        0x0800 //             method
#define	ACC_SYNTHETIC     0x1000 // class field method
#define	ACC_ANNOTATION    0x2000 // class
#define	ACC_ENUM          0x4000 // class field

int IsPublic(u2 accessFlag){
	if((accessFlag & ACC_PUBLIC) == ACC_PUBLIC){
		return 1;
	}
	return 0;
}

int IsPrivate(u2 accessFlag){
	if((accessFlag & ACC_PRIVATE) == ACC_PRIVATE){
		return 1;
	}
	return 0;
}

int IsProtected(u2 accessFlag){
	if((accessFlag & ACC_PROTECTED) == ACC_PROTECTED){
		return 1;
	}
	return 0;
}

int IsStatic(u2 accessFlag){
	if((accessFlag & ACC_STATIC) == ACC_STATIC){
		return 1;
	}
	return 0;
}

int IsFinal(u2 accessFlag){
	if((accessFlag & ACC_FINAL) == ACC_FINAL){
		return 1;
	}
	return 0;
}

int IsSuper(u2 accessFlag){
	if((accessFlag & ACC_SUPER) == ACC_SUPER){
		return 1;
	}
	return 0;
}

int IsSynchronized(u2 accessFlag){
	if((accessFlag & ACC_SYNCHRONIZED) == ACC_SYNCHRONIZED){
		return 1;
	}
	return 0;
}

int IsVolatile(u2 accessFlag){
	if((accessFlag & ACC_VOLATILE) == ACC_VOLATILE){
		return 1;
	}
	return 0;
}

int IsBridge(u2 accessFlag){
	if((accessFlag & ACC_BRIDGE) == ACC_BRIDGE){
		return 1;
	}
	return 0;
}

int IsTransient(u2 accessFlag){
	if((accessFlag & ACC_TRANSIENT) == ACC_TRANSIENT){
		return 1;
	}
	return 0;
}

int IsVarages(u2 accessFlag){
	if((accessFlag & ACC_VARARGS) == ACC_VARARGS){
		return 1;
	}
	return 0;
}

int IsNative(u2 accessFlag){
	if((accessFlag & ACC_NATIVE) == ACC_NATIVE){
		return 1;
	}
	return 0;
}

int IsInterface(u2 accessFlag){
	if((accessFlag & ACC_INTERFACE) == ACC_INTERFACE){
		return 1;
	}
	return 0;
}

int IsAbstract(u2 accessFlag){
	if((accessFlag & ACC_ABSTRACT) == ACC_ABSTRACT){
		return 1;
	}
	return 0;
}

int IsStrict(u2 accessFlag){
	if((accessFlag & ACC_STRICT) == ACC_STRICT){
		return 1;
	}
	return 0;
}

int IsSynthetic(u2 accessFlag){
	if((accessFlag & ACC_SYNTHETIC) == ACC_SYNTHETIC){
		return 1;
	}
	return 0;
}

int IsAnnotation(u2 accessFlag){
	if((accessFlag & ACC_ANNOTATION) == ACC_ANNOTATION){
		return 1;
	}
	return 0;
}

int IsEnum(u2 accessFlag){
	if((accessFlag & ACC_ENUM) == ACC_ENUM){
		return 1;
	}
	return 0;
}
