#pragma once

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#ifdef __EFI__
// grr i hate efi
typedef unsigned long long size_t;
#else
typedef unsigned long size_t;
#endif

typedef _Bool bool;
#define true 1
#define false 0

#ifndef NULL
#define NULL ((void *)0)
#endif
