/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.in by autoheader.  */

/* Define if building universal (internal helper macro) */
/* #undef AC_APPLE_UNIVERSAL_BUILD */


#define WITH_LWIP	1
#include <lwip/opt.h>
#include <lwip/debug.h>
#include <lwip/def.h> /* provide ntohs, htons */

#define assert(x) LWIP_ASSERT("CoAP assert failed", x)

#define COAP_RESOURCES_NOHASH

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME "libcoap"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "libcoap 4.1.1"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "libcoap"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "4.1.1"

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Define WORDS_BIGENDIAN to 1 if your processor stores words with the most
   significant byte first (like Motorola and SPARC, unlike Intel). */
#if defined AC_APPLE_UNIVERSAL_BUILD
# if defined __BIG_ENDIAN__
#  define WORDS_BIGENDIAN 1
# endif
#else
# ifndef WORDS_BIGENDIAN
/* #  undef WORDS_BIGENDIAN */
# endif
#endif

/* Define to rpl_malloc if the replacement function should be used. */
/* #undef malloc */

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef ssize_t */
