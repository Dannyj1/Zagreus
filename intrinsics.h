//
// Created by Danny on 4-1-2023.
//

#pragma once
#if defined(_MSC_VER) || defined(__MINGW32__) || defined(__MINGW64__)
#include <intrin.h>
    #ifdef _WIN64
        #pragma intrinsic(_BitScanForward64)
        #pragma intrinsic(_BitScanReverse64)
        #define USING_INTRINSICS
    #endif
#elif (defined(__GNUC__) && defined(__LP64__)) || defined(__clang__) || defined(__llvm__)
static INLINE unsigned char _BitScanForward64(unsigned long* Index, U64 Mask)
    {
        U64 Ret;
        __asm__
        (
            "bsfq %[Mask], %[Ret]"
            :[Ret] "=r" (Ret)
            :[Mask] "mr" (Mask)
        );
        *Index = (unsigned long)Ret;
        return Mask?1:0;
    }
    static INLINE unsigned char _BitScanReverse64(unsigned long* Index, U64 Mask)
    {
        U64 Ret;
        __asm__
        (
            "bsrq %[Mask], %[Ret]"
            :[Ret] "=r" (Ret)
            :[Mask] "mr" (Mask)
        );
        *Index = (unsigned long)Ret;
        return Mask?1:0;
    }
    #define USING_INTRINSICS
#endif