TEMPLATE = lib
CONFIG += qt buildlib-qchartzoom staticlib qwt


QT  += core gui
greaterThan(QT_MAJOR_VERSION, 4) : QT += widgets

CONFIG( qwt ) : include( ../find_qwt.pri )
include( ../qchartzoom.pri )


TARGET = $$QCHARTZOOM_LIBNAME
DESTDIR = $$QCHARTZOOM_LIBDIR

target.path = $$DESTDIR
INSTALLS += target
