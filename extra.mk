# overrides setting in buildsys.mk
plugindir = @plugindir@

CONTAINER_PLUGIN_DIR ?= Container
CONTAINER_PLUGINS ?=  autocue
EFFECT_PLUGINS ?= 
EFFECT_PLUGIN_DIR ?= Effect
GENERAL_PLUGINS ?= 
GENERAL_PLUGIN_DIR ?= General
INPUT_PLUGINS ?=  libape
INPUT_PLUGIN_DIR ?= Input
OUTPUT_PLUGINS ?= 
OUTPUT_PLUGIN_DIR ?= Output
TRANSPORT_PLUGIN_DIR ?= Transport
TRANSPORT_PLUGINS ?= 
VISUALIZATION_PLUGINS ?= 
VISUALIZATION_PLUGIN_DIR ?= Visualization

USE_GTK ?= yes
USE_QT ?= no

ALSA_CFLAGS ?= @ALSA_CFLAGS@
ALSA_LIBS ?= @ALSA_LIBS@
BINIO_CFLAGS ?= @BINIO_CFLAGS@
BINIO_LIBS ?= @BINIO_LIBS@
BS2B_CFLAGS ?= @BS2B_CFLAGS@
BS2B_LIBS ?= @BS2B_LIBS@
CDIO_LIBS ?= @CDIO_LIBS@
CDIO_CFLAGS ?= @CDIO_CFLAGS@
CDDB_LIBS ?= @CDDB_LIBS@
CDDB_CFLAGS ?= @CDDB_CFLAGS@
CUE_CFLAGS ?= @CUE_CFLAGS@
CUE_LIBS ?= @CUE_LIBS@
CURL_CFLAGS ?= @CURL_CFLAGS@
CURL_LIBS ?= @CURL_LIBS@
DBUS_CFLAGS ?= @DBUS_CFLAGS@
DBUS_LIBS ?= @DBUS_LIBS@
FAAD_CFLAGS ?= @FAAD_CFLAGS@
FAAD_LIBS ?= @FAAD_LIBS@
FFMPEG_CFLAGS ?= @FFMPEG_CFLAGS@
FFMPEG_LIBS ?= @FFMPEG_LIBS@
FILEWRITER_CFLAGS ?= @FILEWRITER_CFLAGS@
FILEWRITER_LIBS ?= @FILEWRITER_LIBS@
FLUIDSYNTH_CFLAGS ?= @FLUIDSYNTH_CFLAGS@
FLUIDSYNTH_LIBS ?= @FLUIDSYNTH_LIBS@
GDKX11_CFLAGS ?= @GDKX11_CFLAGS@
GDKX11_LIBS ?= @GDKX11_LIBS@
GIO_CFLAGS ?= -pthread -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/gio-unix-2.0/  
GIO_LIBS ?= -lgio-2.0 -lgobject-2.0 -lglib-2.0  
GL_LIBS ?= @GL_LIBS@
GLIB_CFLAGS ?= -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include  
GLIB_LIBS ?= -lglib-2.0  
GMODULE_CFLAGS ?= -pthread -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include  
GMODULE_LIBS ?= -Wl,--export-dynamic -pthread -lgmodule-2.0 -lglib-2.0  
GTK_CFLAGS ?= -pthread -I/usr/include/gtk-2.0 -I/usr/lib/x86_64-linux-gnu/gtk-2.0/include -I/usr/include/atk-1.0 -I/usr/include/cairo -I/usr/include/gdk-pixbuf-2.0 -I/usr/include/pango-1.0 -I/usr/include/gio-unix-2.0/ -I/usr/include/freetype2 -I/usr/include/glib-2.0 -I/usr/lib/x86_64-linux-gnu/glib-2.0/include -I/usr/include/pixman-1 -I/usr/include/libpng12 -I/usr/include/harfbuzz  
GTK_LIBS ?= -lgtk-x11-2.0 -lgdk-x11-2.0 -latk-1.0 -lgio-2.0 -lpangoft2-1.0 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lcairo -lpango-1.0 -lfontconfig -lgobject-2.0 -lglib-2.0 -lfreetype  
JACK_CFLAGS ?= @JACK_CFLAGS@
JACK_LIBS ?= @JACK_LIBS@
LIBFLAC_LIBS ?= @LIBFLAC_LIBS@
LIBFLAC_CFLAGS ?= @LIBFLAC_CFLAGS@
MMS_CFLAGS ?= @MMS_CFLAGS@
MMS_LIBS ?= @MMS_LIBS@
MODPLUG_CFLAGS ?= @MODPLUG_CFLAGS@
MODPLUG_LIBS ?= @MODPLUG_LIBS@
MPG123_CFLAGS ?= @MPG123_CFLAGS@
MPG123_LIBS ?= @MPG123_LIBS@
NEON_CFLAGS ?= @NEON_CFLAGS@
NEON_LIBS ?= @NEON_LIBS@
NOTIFY_CFLAGS ?= @NOTIFY_CFLAGS@
NOTIFY_LIBS ?= @NOTIFY_LIBS@
OSS_CFLAGS ?= @OSS_CFLAGS@
SAMPLERATE_CFLAGS ?= @SAMPLERATE_CFLAGS@
SAMPLERATE_LIBS ?= @SAMPLERATE_LIBS@
SDL_CFLAGS ?= @SDL_CFLAGS@
SDL_LIBS ?= @SDL_LIBS@
SIDPLAYFP_CFLAGS ?= @SIDPLAYFP_CFLAGS@
SIDPLAYFP_LIBS ?= @SIDPLAYFP_LIBS@
SNDFILE_CFLAGS ?= @SNDFILE_CFLAGS@
SNDFILE_LIBS ?= @SNDFILE_LIBS@
SNDIO_LIBS ?= @SNDIO_LIBS@
QT_CFLAGS ?= 
QT_LIBS ?= 
QTMULTIMEDIA_CFLAGS ?= @QTMULTIMEDIA_CFLAGS@
QTMULTIMEDIA_LIBS ?= @QTMULTIMEDIA_LIBS@
QTOPENGL_CFLAGS ?= @QTOPENGL_CFLAGS@
QTOPENGL_LIBS ?= @QTOPENGL_LIBS@
VORBIS_CFLAGS ?= @VORBIS_CFLAGS@
VORBIS_LIBS ?= @VORBIS_LIBS@
WAVPACK_CFLAGS ?= @WAVPACK_CFLAGS@
WAVPACK_LIBS ?= @WAVPACK_LIBS@
XCOMPOSITE_CFLAGS ?= @XCOMPOSITE_CFLAGS@
XCOMPOSITE_LIBS ?= @XCOMPOSITE_LIBS@
XML_CFLAGS ?= -I/usr/include/libxml2  
XML_LIBS ?= -lxml2  
XRENDER_CFLAGS ?= @XRENDER_CFLAGS@
XRENDER_LIBS ?= @XRENDER_LIBS@

HAVE_LINUX ?= yes
HAVE_MSWINDOWS ?= no
HAVE_DARWIN ?= no
