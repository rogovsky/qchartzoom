exists(config.pri) : infile(config.pri, BUILD_LIBRARY, yes) : CONFIG += uselib-qchartzoom

TEMPLATE += fakelib
    QCHARTZOOM_LIBNAME = qchartzoom
    QCHARTZOOM_LIBDIR  = $${PWD}/lib
TEMPLATE -= fakelib

unix : uselib-qchartzoom : !buildlib-qchartzoom : QMAKE_RPATHDIR += $$QCHARTZOOM_LIBDIR

#CONFIG *= c++11 c++14

# ==============================================================================
