
#ifndef _COMMON_TYPES_H_
#define _COMMON_TYPES_H_

#define U8 unsigned char
#define U16 unsigned short
#define U32 unsigned long
#define I8 signed char
#define I16 signed short
#define I32 signed long

#ifndef BOOL
#define BOOL unsigned long
#endif

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#define SAFE_DELETE(p) if(p != NULL) { delete (p); (p) = NULL; }

#endif /* _COMMON_TYPES_H_ */
