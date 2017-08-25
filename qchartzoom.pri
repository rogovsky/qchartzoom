include( common.pri )

INCLUDEPATH += $$PWD/sources
DEPENDPATH  += $$PWD/sources
VPATH       += $$PWD

DEFINES += QCHARTZOOM

# ==============================================================================

PUBLIC_HEADERS += \
    sources/qwtchartzoom.h  \
    sources/qaxiszoomsvc.h  \
    sources/qwheelzoomsvc.h \

PUBLIC_SOURCES += \
    sources/qwtchartzoom.cpp  \
    sources/qaxiszoomsvc.cpp  \
    sources/qwheelzoomsvc.cpp \

# ==============================================================================

PRIVATE_HEADERS += \

PRIVATE_SOURCES += \

# ==============================================================================

uselib-qchartzoom : !buildlib-qchartzoom {
    LIBS += -L$$QCHARTZOOM_LIBDIR -l$$QCHARTZOOM_LIBNAME
} else {

    HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS \

    SOURCES += $$PUBLIC_SOURCES $$PRIVATE_SOURCES \
}

# ==============================================================================

OBJECTS_DIR = dir.obj
MOC_DIR     = dir.moc
RCC_DIR     = dir.rcc
UI_DUR      = dir.ui

# ==============================================================================
