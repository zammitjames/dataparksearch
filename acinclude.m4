sinclude(docbook.m4)
sinclude(net.m4)
sinclude(threads.m4)
dnl builtin([include], [docbook.m4])
dnl builtin([include], [net.m4])
dnl builtin([include], [threads.m4])
dnl dnl builtin([include], [libtool15.m4])
dnl dnl builtin([include], [largefiles.m4])

dnl Macro to check for a function prototype in a given header.
dnl
AC_DEFUN([AC_CHECK_FUNC_PROTO],
        [AC_CACHE_CHECK(for $1 function prototype in $2, ac_cv_have_$1_proto,
                AC_EGREP_HEADER($1, $2,
                        ac_cv_have_$1_proto=yes,
                        ac_cv_have_$1_proto=no))
        if test $ac_cv_have_$1_proto = yes ; then
                ac_tr_func=HAVE_`echo $1 | tr 'abcdefghijklmnopqrstuvwxyz' 'ABCDEFGHIJKLMNOPQRSTUVWXYZ'`_PROTO
                AC_DEFINE_UNQUOTED($ac_tr_func)
        fi
])

# PGAC_UNION_SEMUN
# ----------------
# Check if `union semun' exists. Define HAVE_UNION_SEMUN if so.
# If it doesn't then one could define it as
# union semun { int val; struct semid_ds *buf; unsigned short *array; }
AC_DEFUN([PGAC_UNION_SEMUN],
[AC_CHECK_TYPES([union semun], [], [],
[#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>])])
# PGAC_UNION_SEMUN

