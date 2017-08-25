TEMPLATE = subdirs
CONFIG  += ordered

exists(3rdparty) : SUBDIRS += 3rdparty

include(common.pri)
uselib-qchartzoom : SUBDIRS += buildlib
SUBDIRS += examples
