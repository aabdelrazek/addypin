#if !defined( _TYPES_H_ )
#define _TYPES_H_

#include <stdint.h>

typedef int8_t      int8;
typedef int16_t     int16;
typedef int32_t     int32;
typedef int64_t     int64;

typedef uint8_t     uint8;
typedef uint16_t    uint16;
typedef uint32_t    uint32;
typedef uint64_t    uint64;

#ifndef NULL
#if __cplusplus
#define NULL 0
#else
#define NULL (void *)0L
#endif
#endif

#endif
