#include "ff.h"

#if _USE_LFN != 0

#if   _CODE_PAGE == 932	/* Japanese Shift_JIS */
#include "option/cc932.c"
#elif _CODE_PAGE == 936	/* Simplified Chinese GBK */
#include "option/cc936.c"
#elif _CODE_PAGE == 949	/* Korean */
#include "option/cc949.c"
#elif _CODE_PAGE == 950	/* Traditional Chinese Big5 */
#include "option/cc950.c"
#else			/* Single Byte Character-Set */
#include "option/ccsbcs.c"
#endif

#endif
