
m4_include([configure.m4])

AC_INIT(spamserver, [m4_defn([CONFIGURE_VERSION])], m4_defn([CONFIGURE_EMAIL]))

AM_INIT_AUTOMAKE(foreign -Wall)

AC_CONFIG_SRCDIR([server.cc])

AC_PREREQ(2.50)

AC_PROG_CC
AC_PROG_CXX
AC_PROG_LIBTOOL

### define test-for-header macro
# $1 = header file name (i.e. "mdbg.h")
# function fids out the header or terminates configure script
AC_DEFUN([TEST_HEADER],[
FOUND="no"
AC_CHECK_HEADER($1,FOUND="yes")
if test "$FOUND" = "no"; then
  # try search dirs
  for SRCHDIR in \
      `pwd` \
      $HOME/libs/include \
      $HOME/cvs/libs/include \
      $HOME/CVS/libs/include \
      /usr/local/include \
      /usr/include/mysql \
      ;
  do
    AC_MSG_CHECKING(for $1 ($SRCHDIR))
    AC_TRY_CPP([#include "$SRCHDIR/$1"],FOUND="yes")
    AC_MSG_RESULT($FOUND)
    if test "$FOUND" = "yes"; then
      # add path to env
      CPPFLAGS="$CPPFLAGS -I$SRCHDIR"
      break
    fi
  done
fi
# check for result
if test "$FOUND" = "no"; then
  AC_MSG_ERROR(*** Cannot find $1 header!!! ***)
fi
])
### end macro


### define test-for-lib macro
# $1 = library file name (i.e. "mdbg")
# $2 = test function to link (i.e. "MLOG_LOG")
# $3 = other libs needed to link against lib in $1
# function fids out the library or terminates configure script
AC_DEFUN([TEST_LIB],[
FOUND="no"
AC_CHECK_LIB($1,$2,FOUND="yes"; LIBS="-l$1 $LIBS",,$3)
if test "$FOUND" = "no"; then
  # try search dirs
  SAVED_LIBS=$LIBS
  for SRCHDIR in \
      `pwd` \
      $HOME/libs/lib \
      $HOME/cvs/libs/lib \
      $HOME/CVS/libs/lib \
      /usr/local/lib \
          ;
  do
    AC_MSG_CHECKING(for $2 in -l$1 ($SRCHDIR))
    LIBS="-l$1 $SAVED_LIBS -L$SRCHDIR $3"
    AC_TRY_LINK_FUNC($2, FOUND="yes")
    AC_MSG_RESULT($FOUND)
    if test "$FOUND" = "yes"; then
      # add path to env
      LIBS="-l$1 $SAVED_LIBS -L$SRCHDIR"
      break
    fi
  done
fi
# check for result
if test "$FOUND" = "no"; then
  AC_MSG_ERROR(*** Cannot find $1 library!!! ***)
fi
])
### end macro

AC_LANG_CPLUSPLUS
TEST_HEADER(dbglog.h)
TEST_LIB(dbglog, _ZN8DbgLog_t3logEjP8_IO_FILEPKcS3_mPKvPvPFiPciS5_S6_S6_ES6_)

AC_LANG_CPLUSPLUS
TEST_HEADER(pthread.h)
TEST_LIB(pthread, pthread_create)

AC_LANG_CPLUSPLUS
TEST_HEADER(dlfcn.h)
TEST_LIB(dl, dlopen)

AC_LANG_CPLUSPLUS
TEST_HEADER(boost/thread.hpp)
TEST_LIB(boost_thread-mt, _ZN5boost11this_thread18interruption_pointEv)

AC_LANG_CPLUSPLUS
TEST_HEADER(boost/program_options.hpp)
TEST_LIB(boost_program_options-mt, _ZN5boost15program_options18option_descriptionC1Ev)

AC_LANG_CPLUSPLUS
TEST_HEADER(boost/system/config.hpp)
TEST_LIB(boost_system-mt, _ZN5boost6system19get_system_categoryEv)

AC_LANG_CPLUSPLUS
TEST_HEADER(frpc.h)
TEST_LIB(fastrpc, _ZN4FRPC8Server_tD0Ev)

AC_LANG_CPLUSPLUS
TEST_HEADER(threadserver/threadserver.h)
TEST_LIB(threadserver, _ZN12ThreadServer15Configuration_tC1EiPPc)

AC_LANG_CPLUSPLUS
TEST_HEADER(threadserver/handlers/cppfrpchandler/cppfrpchandler.h)
TEST_LIB(:threadserver/handlers/cppfrpchandler.a, _ZN12ThreadServer16CppFrpcHandler_tD0Ev)

AC_LANG_CPLUSPLUS
TEST_HEADER(mysqlwrapper/mysqlwrapper.h)
TEST_LIB(mysqlwrapper, _ZN12MySQLWrapper14MySQLWrapper_tD1Ev)

AC_LANG_CPLUSPLUS
TEST_HEADER(vmime/vmime.hpp)
TEST_LIB(vmime, _ZN5vmime6objectC1ERKS0_)

AC_LANG_CPLUSPLUS
TEST_HEADER(magic.h)
TEST_LIB(magic, magic_open)

AC_LANG_CPLUSPLUS
TEST_HEADER(teng.h)
TEST_LIB(teng, _ZN4Teng6Teng_tD1Ev)

AC_SUBST(CXXEXTRAFLAGS)

AC_OUTPUT( \
    Makefile
)

