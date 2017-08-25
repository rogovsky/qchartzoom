TEMPLATE = lib
CONFIG += qt buildlib-qchartzoom staticlib qwt

include( ../qchartzoom.pri )

TARGET = $$QCHARTZOOM_LIBNAME
DESTDIR = $$QCHARTZOOM_LIBDIR

target.path = $$DESTDIR
INSTALLS += target
