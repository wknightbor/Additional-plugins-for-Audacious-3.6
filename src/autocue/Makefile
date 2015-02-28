PLUGIN = autocue${PLUGIN_SUFFIX}

SRCS = autocue.cc

include ../../buildsys.mk
include ../../extra.mk

plugindir := ${plugindir}/${CONTAINER_PLUGIN_DIR}

LD = ${CXX}

CPPFLAGS += -I../.. ${PLUGIN_CPPFLAGS} ${AUTOCUE_CFLAGS}
CFLAGS += ${PLUGIN_CFLAGS}
LIBS += ${AUTOCUE_LIBS}
