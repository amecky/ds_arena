#include "sprites.h"
#include <stdint.h>
#include "..\..\..\diesel\diesel.h"

typedef uint8_t BYTE;

const BYTE Sprite_GS_Main[] = {
	68,  88,  66,  67,  37,  55,
	92, 120, 113, 229,  23,   1,
	199, 206, 202,  12,  64, 233,
	99, 179,   1,   0,   0,   0,
	4,   8,   0,   0,   3,   0,
	0,   0,  44,   0,   0,   0,
	184,   0,   0,   0,  44,   1,
	0,   0,  73,  83,  71,  78,
	132,   0,   0,   0,   4,   0,
	0,   0,   8,   0,   0,   0,
	104,   0,   0,   0,   0,   0,
	0,   0,   1,   0,   0,   0,
	3,   0,   0,   0,   0,   0,
	0,   0,  15,   3,   0,   0,
	116,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,
	3,   0,   0,   0,   1,   0,
	0,   0,  15,  15,   0,   0,
	122,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,
	3,   0,   0,   0,   2,   0,
	0,   0,   7,   7,   0,   0,
	116,   0,   0,   0,   1,   0,
	0,   0,   0,   0,   0,   0,
	3,   0,   0,   0,   3,   0,
	0,   0,  15,  15,   0,   0,
	83,  86,  95,  80,  79,  83,
	73,  84,  73,  79,  78,   0,
	67,  79,  76,  79,  82,   0,
	78,  79,  82,  77,  65,  76,
	0, 171, 171, 171,  79,  83,
	71,  78, 108,   0,   0,   0,
	3,   0,   0,   0,   8,   0,
	0,   0,  80,   0,   0,   0,
	0,   0,   0,   0,   1,   0,
	0,   0,   3,   0,   0,   0,
	0,   0,   0,   0,  15,   0,
	0,   0,  92,   0,   0,   0,
	0,   0,   0,   0,   0,   0,
	0,   0,   3,   0,   0,   0,
	1,   0,   0,   0,   3,  12,
	0,   0, 101,   0,   0,   0,
	0,   0,   0,   0,   0,   0,
	0,   0,   3,   0,   0,   0,
	2,   0,   0,   0,  15,   0,
	0,   0,  83,  86,  95,  80,
	79,  83,  73,  84,  73,  79,
	78,   0,  84,  69,  88,  67,
	79,  79,  82,  68,   0,  67,
	79,  76,  79,  82,   0, 171,
	83,  72,  68,  82, 208,   6,
	0,   0,  64,   0,   2,   0,
	180,   1,   0,   0,  89,   0,
	0,   4,  70, 142,  32,   0,
	0,   0,   0,   0,   5,   0,
	0,   0,  97,   0,   0,   5,
	242,  16,  32,   0,   1,   0,
	0,   0,   0,   0,   0,   0,
	1,   0,   0,   0,  95,   0,
	0,   4, 242,  16,  32,   0,
	1,   0,   0,   0,   1,   0,
	0,   0,  95,   0,   0,   4,
	114,  16,  32,   0,   1,   0,
	0,   0,   2,   0,   0,   0,
	95,   0,   0,   4, 242,  16,
	32,   0,   1,   0,   0,   0,
	3,   0,   0,   0, 104,   0,
	0,   2,   5,   0,   0,   0,
	93,   8,   0,   1,  92,  40,
	0,   1, 103,   0,   0,   4,
	242,  32,  16,   0,   0,   0,
	0,   0,   1,   0,   0,   0,
	101,   0,   0,   3,  50,  32,
	16,   0,   1,   0,   0,   0,
	101,   0,   0,   3, 242,  32,
	16,   0,   2,   0,   0,   0,
	94,   0,   0,   2,   4,   0,
	0,   0,   9,   0,   0,   1,
	56,   0,   0,   9, 242,   0,
	16,   0,   0,   0,   0,   0,
	182,  30,  32,   0,   0,   0,
	0,   0,   1,   0,   0,   0,
	22,  20,  32,   0,   0,   0,
	0,   0,   2,   0,   0,   0,
	56,   0,   0,  10, 242,   0,
	16,   0,   0,   0,   0,   0,
	70,  14,  16,   0,   0,   0,
	0,   0,   2,  64,   0,   0,
	0,   0,   0,  63,   0,   0,
	0, 191,   0,   0,   0,  63,
	0,   0,   0, 191,  77,   0,
	0,   8,  18,   0,  16,   0,
	1,   0,   0,   0,  18,   0,
	16,   0,   2,   0,   0,   0,
	42,  16,  32,   0,   0,   0,
	0,   0,   2,   0,   0,   0,
	56,   0,   0,   7, 242,   0,
	16,   0,   1,   0,   0,   0,
	70,  14,  16,   0,   0,   0,
	0,   0,   6,   0,  16,   0,
	1,   0,   0,   0,  50,   0,
	0,  10, 242,   0,  16,   0,
	3,   0,   0,   0,   6,   0,
	16,   0,   2,   0,   0,   0,
	150,   9,  16,   0,   0,   0,
	0,   0,   6,  15,  16, 128,
	65,   0,   0,   0,   1,   0,
	0,   0,  50,   0,   0,   9,
	242,   0,  16,   0,   0,   0,
	0,   0,   6,   0,  16,   0,
	2,   0,   0,   0,   6,  15,
	16,   0,   0,   0,   0,   0,
	150,   9,  16,   0,   1,   0,
	0,   0,  50,   0,   0,  15,
	50,   0,  16,   0,   1,   0,
	0,   0,  70, 128,  32, 128,
	65,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,
	2,  64,   0,   0,   0,   0,
	0,  63,   0,   0,   0,  63,
	0,   0,   0,   0,   0,   0,
	0,   0,  70,  16,  32,   0,
	0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   7,
	242,   0,  16,   0,   2,   0,
	0,   0,   6,   0,  16,   0,
	1,   0,   0,   0,  38,  13,
	16,   0,   3,   0,   0,   0,
	0,   0,   0,   7, 242,   0,
	16,   0,   0,   0,   0,   0,
	198,   9,  16,   0,   0,   0,
	0,   0,  86,   5,  16,   0,
	1,   0,   0,   0,  54,   0,
	0,   5,  18,   0,  16,   0,
	1,   0,   0,   0,  26,   0,
	16,   0,   2,   0,   0,   0,
	54,   0,   0,   5,  34,   0,
	16,   0,   1,   0,   0,   0,
	10,   0,  16,   0,   0,   0,
	0,   0,  54,   0,   0,   8,
	194,   0,  16,   0,   1,   0,
	0,   0,   2,  64,   0,   0,
	0,   0,   0,   0,   0,   0,
	0,   0,   0,   0, 128,  63,
	0,   0, 128,  63,  17,   0,
	0,   8,  18,   0,  16,   0,
	3,   0,   0,   0,  70,  15,
	16,   0,   1,   0,   0,   0,
	70, 142,  32,   0,   0,   0,
	0,   0,   1,   0,   0,   0,
	54,   0,   0,   5,  18,  32,
	16,   0,   0,   0,   0,   0,
	10,   0,  16,   0,   3,   0,
	0,   0,  17,   0,   0,   8,
	18,   0,  16,   0,   3,   0,
	0,   0,  70,  15,  16,   0,
	1,   0,   0,   0,  70, 142,
	32,   0,   0,   0,   0,   0,
	2,   0,   0,   0,  17,   0,
	0,   8,  18,   0,  16,   0,
	1,   0,   0,   0,  70,  14,
	16,   0,   1,   0,   0,   0,
	70, 142,  32,   0,   0,   0,
	0,   0,   4,   0,   0,   0,
	54,   0,   0,   5,  34,  32,
	16,   0,   0,   0,   0,   0,
	10,   0,  16,   0,   3,   0,
	0,   0,  54,   0,   0,   5,
	66,  32,  16,   0,   0,   0,
	0,   0,   1,  64,   0,   0,
	0,   0, 128,  63,  54,   0,
	0,   5, 130,  32,  16,   0,
	0,   0,   0,   0,  10,   0,
	16,   0,   1,   0,   0,   0,
	14,   0,   0,   9, 242,   0,
	16,   0,   1,   0,   0,   0,
	70,  30,  32,   0,   0,   0,
	0,   0,   1,   0,   0,   0,
	230, 142,  32,   0,   0,   0,
	0,   0,   0,   0,   0,   0,
	54,   0,   0,   5,  50,  32,
	16,   0,   1,   0,   0,   0,
	70,   0,  16,   0,   1,   0,
	0,   0,  54,   0,   0,   6,
	242,  32,  16,   0,   2,   0,
	0,   0,  70,  30,  32,   0,
	0,   0,   0,   0,   3,   0,
	0,   0,  19,   0,   0,   1,
	54,   0,   0,   5,  18,   0,
	16,   0,   3,   0,   0,   0,
	42,   0,  16,   0,   2,   0,
	0,   0,  54,   0,   0,   5,
	34,   0,  16,   0,   3,   0,
	0,   0,  42,   0,  16,   0,
	0,   0,   0,   0,  54,   0,
	0,   8, 194,   0,  16,   0,
	3,   0,   0,   0,   2,  64,
	0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,
	128,  63,   0,   0, 128,  63,
	17,   0,   0,   8,  18,   0,
	16,   0,   4,   0,   0,   0,
	70,  15,  16,   0,   3,   0,
	0,   0,  70, 142,  32,   0,
	0,   0,   0,   0,   1,   0,
	0,   0,  54,   0,   0,   5,
	18,  32,  16,   0,   0,   0,
	0,   0,  10,   0,  16,   0,
	4,   0,   0,   0,  17,   0,
	0,   8,  18,   0,  16,   0,
	4,   0,   0,   0,  70,  15,
	16,   0,   3,   0,   0,   0,
	70, 142,  32,   0,   0,   0,
	0,   0,   2,   0,   0,   0,
	17,   0,   0,   8,  18,   0,
	16,   0,   3,   0,   0,   0,
	70,  14,  16,   0,   3,   0,
	0,   0,  70, 142,  32,   0,
	0,   0,   0,   0,   4,   0,
	0,   0,  54,   0,   0,   5,
	34,  32,  16,   0,   0,   0,
	0,   0,  10,   0,  16,   0,
	4,   0,   0,   0,  54,   0,
	0,   5,  66,  32,  16,   0,
	0,   0,   0,   0,   1,  64,
	0,   0,   0,   0, 128,  63,
	54,   0,   0,   5, 130,  32,
	16,   0,   0,   0,   0,   0,
	10,   0,  16,   0,   3,   0,
	0,   0,   0,   0,   0,   7,
	194,   0,  16,   0,   1,   0,
	0,   0, 166,  14,  16,   0,
	1,   0,   0,   0,   6,   4,
	16,   0,   1,   0,   0,   0,
	54,   0,   0,   5,  18,  32,
	16,   0,   1,   0,   0,   0,
	42,   0,  16,   0,   1,   0,
	0,   0,  54,   0,   0,   5,
	34,  32,  16,   0,   1,   0,
	0,   0,  26,   0,  16,   0,
	1,   0,   0,   0,  54,   0,
	0,   6, 242,  32,  16,   0,
	2,   0,   0,   0,  70,  30,
	32,   0,   0,   0,   0,   0,
	3,   0,   0,   0,  19,   0,
	0,   1,  54,   0,   0,   5,
	18,   0,  16,   0,   0,   0,
	0,   0,  58,   0,  16,   0,
	2,   0,   0,   0,  54,   0,
	0,   5,  34,   0,  16,   0,
	2,   0,   0,   0,  58,   0,
	16,   0,   0,   0,   0,   0,
	54,   0,   0,   8, 194,   0,
	16,   0,   2,   0,   0,   0,
	2,  64,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,
	0,   0, 128,  63,   0,   0,
	128,  63,  17,   0,   0,   8,
	34,   0,  16,   0,   1,   0,
	0,   0,  70,  15,  16,   0,
	2,   0,   0,   0,  70, 142,
	32,   0,   0,   0,   0,   0,
	1,   0,   0,   0,  54,   0,
	0,   5,  18,  32,  16,   0,
	0,   0,   0,   0,  26,   0,
	16,   0,   1,   0,   0,   0,
	17,   0,   0,   8,  34,   0,
	16,   0,   1,   0,   0,   0,
	70,  15,  16,   0,   2,   0,
	0,   0,  70, 142,  32,   0,
	0,   0,   0,   0,   2,   0,
	0,   0,  17,   0,   0,   8,
	18,   0,  16,   0,   2,   0,
	0,   0,  70,  14,  16,   0,
	2,   0,   0,   0,  70, 142,
	32,   0,   0,   0,   0,   0,
	4,   0,   0,   0,  54,   0,
	0,   5,  34,  32,  16,   0,
	0,   0,   0,   0,  26,   0,
	16,   0,   1,   0,   0,   0,
	54,   0,   0,   5,  66,  32,
	16,   0,   0,   0,   0,   0,
	1,  64,   0,   0,   0,   0,
	128,  63,  54,   0,   0,   5,
	130,  32,  16,   0,   0,   0,
	0,   0,  10,   0,  16,   0,
	2,   0,   0,   0,  54,   0,
	0,   5,  18,  32,  16,   0,
	1,   0,   0,   0,  10,   0,
	16,   0,   1,   0,   0,   0,
	54,   0,   0,   5,  34,  32,
	16,   0,   1,   0,   0,   0,
	58,   0,  16,   0,   1,   0,
	0,   0,  54,   0,   0,   6,
	242,  32,  16,   0,   2,   0,
	0,   0,  70,  30,  32,   0,
	0,   0,   0,   0,   3,   0,
	0,   0,  19,   0,   0,   1,
	54,   0,   0,   8, 194,   0,
	16,   0,   0,   0,   0,   0,
	2,  64,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,
	0,   0, 128,  63,   0,   0,
	128,  63,  17,   0,   0,   8,
	18,   0,  16,   0,   1,   0,
	0,   0,  70,  15,  16,   0,
	0,   0,   0,   0,  70, 142,
	32,   0,   0,   0,   0,   0,
	1,   0,   0,   0,  54,   0,
	0,   5,  18,  32,  16,   0,
	0,   0,   0,   0,  10,   0,
	16,   0,   1,   0,   0,   0,
	17,   0,   0,   8,  18,   0,
	16,   0,   1,   0,   0,   0,
	70,  15,  16,   0,   0,   0,
	0,   0,  70, 142,  32,   0,
	0,   0,   0,   0,   2,   0,
	0,   0,  17,   0,   0,   8,
	18,   0,  16,   0,   0,   0,
	0,   0,  70,  14,  16,   0,
	0,   0,   0,   0,  70, 142,
	32,   0,   0,   0,   0,   0,
	4,   0,   0,   0,  54,   0,
	0,   5,  34,  32,  16,   0,
	0,   0,   0,   0,  10,   0,
	16,   0,   1,   0,   0,   0,
	54,   0,   0,   5,  66,  32,
	16,   0,   0,   0,   0,   0,
	1,  64,   0,   0,   0,   0,
	128,  63,  54,   0,   0,   5,
	130,  32,  16,   0,   0,   0,
	0,   0,  10,   0,  16,   0,
	0,   0,   0,   0,  54,   0,
	0,   5,  50,  32,  16,   0,
	1,   0,   0,   0, 230,  10,
	16,   0,   1,   0,   0,   0,
	54,   0,   0,   6, 242,  32,
	16,   0,   2,   0,   0,   0,
	70,  30,  32,   0,   0,   0,
	0,   0,   3,   0,   0,   0,
	19,   0,   0,   1,   9,   0,
	0,   1,  62,   0,   0,   1
};

const BYTE Sprite_PS_Main[] = {
	68,  88,  66,  67, 133, 123,
	244, 109,  36, 101, 150, 228,
	91, 135, 209, 221,  54, 143,
	33,  28,   1,   0,   0,   0,
	112,   1,   0,   0,   3,   0,
	0,   0,  44,   0,   0,   0,
	160,   0,   0,   0, 212,   0,
	0,   0,  73,  83,  71,  78,
	108,   0,   0,   0,   3,   0,
	0,   0,   8,   0,   0,   0,
	80,   0,   0,   0,   0,   0,
	0,   0,   1,   0,   0,   0,
	3,   0,   0,   0,   0,   0,
	0,   0,  15,   0,   0,   0,
	92,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,
	3,   0,   0,   0,   1,   0,
	0,   0,   3,   3,   0,   0,
	101,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,
	3,   0,   0,   0,   2,   0,
	0,   0,  15,  15,   0,   0,
	83,  86,  95,  80,  79,  83,
	73,  84,  73,  79,  78,   0,
	84,  69,  88,  67,  79,  79,
	82,  68,   0,  67,  79,  76,
	79,  82,   0, 171,  79,  83,
	71,  78,  44,   0,   0,   0,
	1,   0,   0,   0,   8,   0,
	0,   0,  32,   0,   0,   0,
	0,   0,   0,   0,   0,   0,
	0,   0,   3,   0,   0,   0,
	0,   0,   0,   0,  15,   0,
	0,   0,  83,  86,  95,  84,
	65,  82,  71,  69,  84,   0,
	171, 171,  83,  72,  68,  82,
	148,   0,   0,   0,  64,   0,
	0,   0,  37,   0,   0,   0,
	90,   0,   0,   3,   0,  96,
	16,   0,   0,   0,   0,   0,
	88,  24,   0,   4,   0, 112,
	16,   0,   0,   0,   0,   0,
	85,  85,   0,   0,  98,  16,
	0,   3,  50,  16,  16,   0,
	1,   0,   0,   0,  98,  16,
	0,   3, 242,  16,  16,   0,
	2,   0,   0,   0, 101,   0,
	0,   3, 242,  32,  16,   0,
	0,   0,   0,   0, 104,   0,
	0,   2,   1,   0,   0,   0,
	69,   0,   0,   9, 242,   0,
	16,   0,   0,   0,   0,   0,
	70,  16,  16,   0,   1,   0,
	0,   0,  70, 126,  16,   0,
	0,   0,   0,   0,   0,  96,
	16,   0,   0,   0,   0,   0,
	56,   0,   0,   7, 242,  32,
	16,   0,   0,   0,   0,   0,
	70,  14,  16,   0,   0,   0,
	0,   0,  70,  30,  16,   0,
	2,   0,   0,   0,  62,   0,
	0,   1
};

const BYTE Sprite_VS_Main[] = {
	68,  88,  66,  67, 164, 251,
	180,  24, 176, 117, 195, 165,
	138, 253, 123, 104, 232,  72,
	91, 185,   1,   0,   0,   0,
	28,   2,   0,   0,   3,   0,
	0,   0,  44,   0,   0,   0,
	180,   0,   0,   0,  64,   1,
	0,   0,  73,  83,  71,  78,
	128,   0,   0,   0,   4,   0,
	0,   0,   8,   0,   0,   0,
	104,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,
	3,   0,   0,   0,   0,   0,
	0,   0,   7,   7,   0,   0,
	113,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,
	3,   0,   0,   0,   1,   0,
	0,   0,  15,  15,   0,   0,
	119,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,
	3,   0,   0,   0,   2,   0,
	0,   0,   7,   7,   0,   0,
	113,   0,   0,   0,   1,   0,
	0,   0,   0,   0,   0,   0,
	3,   0,   0,   0,   3,   0,
	0,   0,  15,  15,   0,   0,
	80,  79,  83,  73,  84,  73,
	79,  78,   0,  67,  79,  76,
	79,  82,   0,  78,  79,  82,
	77,  65,  76,   0, 171, 171,
	79,  83,  71,  78, 132,   0,
	0,   0,   4,   0,   0,   0,
	8,   0,   0,   0, 104,   0,
	0,   0,   0,   0,   0,   0,
	1,   0,   0,   0,   3,   0,
	0,   0,   0,   0,   0,   0,
	15,   0,   0,   0, 116,   0,
	0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   3,   0,
	0,   0,   1,   0,   0,   0,
	15,   0,   0,   0, 122,   0,
	0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   3,   0,
	0,   0,   2,   0,   0,   0,
	7,   8,   0,   0, 116,   0,
	0,   0,   1,   0,   0,   0,
	0,   0,   0,   0,   3,   0,
	0,   0,   3,   0,   0,   0,
	15,   0,   0,   0,  83,  86,
	95,  80,  79,  83,  73,  84,
	73,  79,  78,   0,  67,  79,
	76,  79,  82,   0,  78,  79,
	82,  77,  65,  76,   0, 171,
	171, 171,  83,  72,  68,  82,
	212,   0,   0,   0,  64,   0,
	1,   0,  53,   0,   0,   0,
	95,   0,   0,   3, 114,  16,
	16,   0,   0,   0,   0,   0,
	95,   0,   0,   3, 242,  16,
	16,   0,   1,   0,   0,   0,
	95,   0,   0,   3, 114,  16,
	16,   0,   2,   0,   0,   0,
	95,   0,   0,   3, 242,  16,
	16,   0,   3,   0,   0,   0,
	103,   0,   0,   4, 242,  32,
	16,   0,   0,   0,   0,   0,
	1,   0,   0,   0, 101,   0,
	0,   3, 242,  32,  16,   0,
	1,   0,   0,   0, 101,   0,
	0,   3, 114,  32,  16,   0,
	2,   0,   0,   0, 101,   0,
	0,   3, 242,  32,  16,   0,
	3,   0,   0,   0,  54,   0,
	0,   5, 114,  32,  16,   0,
	0,   0,   0,   0,  70,  18,
	16,   0,   0,   0,   0,   0,
	54,   0,   0,   5, 130,  32,
	16,   0,   0,   0,   0,   0,
	1,  64,   0,   0,   0,   0,
	128,  63,  54,   0,   0,   5,
	242,  32,  16,   0,   1,   0,
	0,   0,  70,  30,  16,   0,
	1,   0,   0,   0,  54,   0,
	0,   5, 114,  32,  16,   0,
	2,   0,   0,   0,  70,  18,
	16,   0,   2,   0,   0,   0,
	54,   0,   0,   5, 242,  32,
	16,   0,   3,   0,   0,   0,
	70,  30,  16,   0,   3,   0,
	0,   0,  62,   0,   0,   1
};
namespace sprites {

	struct SpriteBufferVertex {
		ds::vec3 position;
		ds::vec4 texture;
		ds::vec3 size;
		ds::Color color;
	};

	struct SpriteBufferConstantBuffer {
		ds::vec4 screenDimension;
		ds::matrix wvp;
	};

	struct SpritesContext {
		int max;
		int current;
		SpriteBufferConstantBuffer constantBuffer;
		RID vertexBufferID;
		RID item;
		RID textureID;
		RID orthoPass;
		ds::Camera orthoCamera;
		SpriteBufferVertex* vertices;
	};

	static SpritesContext* _spritesCtx = 0;

	void init(int maxSprites, RID textureID) {
		_spritesCtx = new SpritesContext;
		_spritesCtx->max = maxSprites;
		_spritesCtx->textureID = textureID;
		_spritesCtx->vertices = new SpriteBufferVertex[maxSprites];

		RID vertexShader = ds::createVertexShader(Sprite_VS_Main, sizeof(Sprite_VS_Main), "SpriteVS");
		RID pixelShader = ds::createPixelShader(Sprite_PS_Main, sizeof(Sprite_PS_Main), "SpritePS");
		RID geoShader = ds::createGeometryShader(Sprite_GS_Main, sizeof(Sprite_GS_Main), "SpriteGS");

		// very special buffer layout 
		ds::InputLayoutDefinition decl[] = {
			{ ds::BufferAttribute::POSITION,ds::BufferAttributeType::FLOAT,3 },
			{ ds::BufferAttribute::COLOR,ds::BufferAttributeType::FLOAT,4 },
			{ ds::BufferAttribute::NORMAL,ds::BufferAttributeType::FLOAT,3 },
			{ ds::BufferAttribute::COLOR,ds::BufferAttributeType::FLOAT,4 }
		};
		ds::InputLayoutInfo layoutInfo = { decl, 4, vertexShader };
		RID vertexDeclId = ds::createInputLayout(layoutInfo, "PCNC_Layout");

		RID cbid = ds::createConstantBuffer(sizeof(SpriteBufferConstantBuffer), &_spritesCtx->constantBuffer, "SpriteBufferConstantBuffer");
		ds::VertexBufferInfo vbInfo = { ds::BufferType::DYNAMIC, maxSprites, sizeof(SpriteBufferVertex) };
		_spritesCtx->vertexBufferID = ds::createVertexBuffer(vbInfo, "SpriteBufferVertex");

		ds::SamplerStateInfo samplerInfo = { ds::TextureAddressModes::CLAMP, ds::TextureFilters::LINEAR };
		RID ssid = ds::createSamplerState(samplerInfo);

		RID spriteStateGroup = ds::StateGroupBuilder()
			.inputLayout(vertexDeclId)
			.vertexBuffer(_spritesCtx->vertexBufferID)
			.indexBuffer(NO_RID)
			.vertexShader(vertexShader)
			.geometryShader(geoShader)
			.pixelShader(pixelShader)
			.constantBuffer(cbid, geoShader, 0)
			.texture(textureID, pixelShader, 0)
			.samplerState(ssid, pixelShader)
			.build();

		ds::vec2 textureSize = ds::getTextureSize(textureID);
		_spritesCtx->constantBuffer.screenDimension = ds::vec4(ds::getScreenWidth(), ds::getScreenHeight(), textureSize.x, textureSize.y);

		ds::DrawCommand drawCmd = { 100, ds::DrawType::DT_VERTICES, ds::PrimitiveTypes::POINT_LIST, 0 };
		_spritesCtx->item = ds::compile(drawCmd, spriteStateGroup, "SpriteBuffer");

		ds::matrix orthoView = ds::matIdentity();
		ds::matrix orthoProjection = ds::matOrthoLH(ds::getScreenWidth(), ds::getScreenHeight(), 0.1f, 1.0f);
		_spritesCtx->orthoCamera = {
			orthoView,
			orthoProjection,
			orthoView * orthoProjection,
			ds::vec3(0,0,0),
			ds::vec3(0,0,0),
			ds::vec3(0,1,0),
			ds::vec3(1,0,0),
			0.0f,
			0.0f,
			0.0f
		};
		ds::RenderPassInfo rpInfo = { &_spritesCtx->orthoCamera, ds::DepthBufferState::DISABLED, 0, 0 };
		_spritesCtx->orthoPass = ds::createRenderPass(rpInfo);
		_spritesCtx->constantBuffer.wvp = ds::matTranspose(_spritesCtx->orthoCamera.viewProjectionMatrix);

		_spritesCtx->current = 0;
	}

	void begin() {
		_spritesCtx->current = 0;
	}

	void add(const ds::vec2& position, const ds::vec4& rect, const ds::vec2& scale, float rotation, const ds::Color& clr) {
		if ((_spritesCtx->current + 1) >= _spritesCtx->max) {
			flush();
		}
		_spritesCtx->vertices[_spritesCtx->current++] = { position, rect, ds::vec3(scale.x, scale.y, rotation), clr };
	}

	void flush() {
		if (_spritesCtx->current > 0) {
			ds::mapBufferData(_spritesCtx->vertexBufferID, _spritesCtx->vertices, _spritesCtx->current * sizeof(SpriteBufferVertex));
			ds::submit(_spritesCtx->orthoPass, _spritesCtx->item, _spritesCtx->current);
			_spritesCtx->current = 0;
		}
	}


	void shutdown() {
		if (_spritesCtx != 0) {
			delete _spritesCtx;
		}
	}
}