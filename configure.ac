dnl ***
dnl *** Process this file with autoconf to produce a configure script.
dnl ***

dnl Initialize
dnl ==========
AC_PREREQ([2.59])
AC_INIT([audacious-plugins], [3.6-beta1])
AC_COPYRIGHT([Copyright (C) 2001-2015 Audacious developers and others])

AC_DEFINE_UNQUOTED([PACKAGE], "$PACKAGE_NAME", [Name of package])
AC_DEFINE_UNQUOTED([VERSION], "$PACKAGE_VERSION", [Version number of package])

AC_CONFIG_AUX_DIR([build-aux])
AC_CONFIG_HEADERS([config.h])
AC_CONFIG_MACRO_DIR([m4])

AUD_COMMON_PROGS

BUILDSYS_SHARED_LIB

dnl Headers and functions
dnl =====================
AC_CHECK_FUNCS([mkdtemp])

dnl gettext
dnl =======
AM_GNU_GETTEXT([external])
if test "$MSGFMT" = ":" ; then
    AC_MSG_ERROR([msgfmt was not found; have you installed gettext?])
fi

LIBS="$LIBS $LIBINTL"

dnl Check for Audacious
dnl ===================

PKG_CHECK_MODULES(AUDACIOUS, [audacious >= 3.6],
    [],
    [AC_MSG_ERROR([Cannot find Audacious 3.6; have you installed Audacious yet?])]
)

CPPFLAGS="$CPPFLAGS $AUDACIOUS_CFLAGS"
LIBS="$LIBS $AUDACIOUS_LIBS"

dnl Check for libxml2 (required to load XSPF playlists from previous versions)
dnl ==========================================================================

if test $HAVE_DARWIN = yes ; then
    AC_ARG_WITH([system-libxml2],
     [AS_HELP_STRING([--with-system-libxml2=yes,no], [Use system version of libxml2 (default=yes)])],
     [with_system_libxml2=$withval],
     [with_system_libxml2=yes])

    if test $with_system_libxml2 = yes ; then
        XML_LIBS="-lxml2"
        XML_CFLAGS="-I/usr/include/libxml2"
        AC_SUBST(XML_LIBS)
        AC_SUBST(XML_CFLAGS)
    else
        PKG_CHECK_MODULES([XML], [libxml-2.0])
    fi
else
    PKG_CHECK_MODULES([XML], [libxml-2.0])
fi

dnl Default Set of Plugins
dnl ======================

INPUT_PLUGINS=""
OUTPUT_PLUGINS=""
EFFECT_PLUGINS=""
GENERAL_PLUGINS=""
VISUALIZATION_PLUGINS=""
CONTAINER_PLUGINS=""
TRANSPORT_PLUGINS=""

dnl AUTOCUE
dnl ---

AC_ARG_ENABLE(autocue,
[AS_HELP_STRING([--disable-autocue], [disable autocue support. (default=enabled)])],
[enable_autocue=$enableval],
[enable_autocue=auto])

CONTAINER_PLUGINS="$CONTAINER_PLUGINS autocue"
have_autocue=yes

dnl XXX

dnl LIBAPE
dnl ---

AC_ARG_ENABLE(autocue,
[AS_HELP_STRING([--disable-libape], [disable libape support. (default=enabled)])],
[enable_libape=$enableval],
[enable_libape=auto])

have_libape=no

if test "x$enable_libape" != "xno"; then
    PKG_CHECK_MODULES(MACLIB, [maclib >= 4.11.0],
     [have_libape=yes
      INPUT_PLUGINS="$INPUT_PLUGINS libape"],
     [if test "x$enable_libape" = "xyes"; then
          AC_MSG_ERROR([Cannot find maclib development files (ver >= 4.11.0), but compilation of LIBAPE plugin has been explicitly requested; please install maclib dev files and run configure again])
      fi]
    )
else
    AC_MSG_RESULT([*** maclib not found; LIBAPE plugin disabled])
fi

dnl XXX

INPUT_PLUGIN_DIR=Input
OUTPUT_PLUGIN_DIR=Output
EFFECT_PLUGIN_DIR=Effect
GENERAL_PLUGIN_DIR=General
VISUALIZATION_PLUGIN_DIR=Visualization
CONTAINER_PLUGIN_DIR=Container
TRANSPORT_PLUGIN_DIR=Transport

AC_SUBST(INPUT_PLUGIN_DIR)
AC_SUBST(OUTPUT_PLUGIN_DIR)
AC_SUBST(EFFECT_PLUGIN_DIR)
AC_SUBST(GENERAL_PLUGIN_DIR)
AC_SUBST(VISUALIZATION_PLUGIN_DIR)
AC_SUBST(CONTAINER_PLUGIN_DIR)
AC_SUBST(TRANSPORT_PLUGIN_DIR)

localedir="$datarootdir/locale"
AC_SUBST(localedir)

AC_SUBST(EFFECT_PLUGINS)
AC_SUBST(GENERAL_PLUGINS)
AC_SUBST(INPUT_PLUGINS)
AC_SUBST(OUTPUT_PLUGINS)
AC_SUBST(VISUALIZATION_PLUGINS)
AC_SUBST(CONTAINER_PLUGINS)
AC_SUBST(TRANSPORT_PLUGINS)


dnl Reliably #include "config.h" (for large file support)
dnl =====================================================
CPPFLAGS="$CPPFLAGS -include config.h"


dnl Generate config files
dnl =====================

AC_CONFIG_FILES([
    buildsys.mk
    extra.mk
])

AC_OUTPUT


dnl Print results
dnl =============

echo
echo "Configuration:"
echo
echo "  Install path:                           $plugindir"
echo
echo "  Input Plugins"
echo "  -------------"
echo "  Monkey's audio libape:                  $have_libape"
echo
echo "  Container"
echo "  ---------"
echo "  Auto CUE playlist format (cue):         $have_autocue"

