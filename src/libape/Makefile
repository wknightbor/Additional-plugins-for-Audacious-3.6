PLUGIN = libape${PLUGIN_SUFFIX}

SRCS = mac.cc

include ../../buildsys.mk
include ../../extra.mk

plugindir := ${plugindir}/${INPUT_PLUGIN_DIR}

LD = ${CXX}

CPPFLAGS += -I../.. ${PLUGIN_CPPFLAGS} ${MACLIB_CFLAGS}
CFLAGS += ${PLUGIN_CFLAGS}
LIBS += ${MACLIB_LIBS} -laudtag -lmaclib


