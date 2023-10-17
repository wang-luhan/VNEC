#pragma once
#define SpB_TOKEN_PASTING2(x, y) x##y
#define SpB_TOKEN_PASTING3(x, y, z) x##y##z
#define SpB_TOKEN_PASTING4(x, y, z, a) x##y##z##a
#define SpB(x) SpB_SPLICE2(SpB_, x)
