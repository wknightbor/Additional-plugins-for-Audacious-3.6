PLUGIN = filterpro${PLUGIN_SUFFIX}

SRCS = filterpro.cc

include ../../buildsys.mk
include ../../extra.mk

plugindir := ${plugindir}/${EFFECT_PLUGIN_DIR}

LD = ${CXX}

CFLAGS += ${PLUGIN_CFLAGS}
CPPFLAGS += ${PLUGIN_CPPFLAGS} -I../..
#LIBS += -llibc 
