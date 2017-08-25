include( common.pri )

INCLUDEPATH += $$PWD
DEPENDPATH  += $$PWD
VPATH       += $$PWD

DEFINES += QCHARTZOOM

# ==============================================================================

PUBLIC_HEADERS += \
    qwtchartzoom.h  \
    qaxiszoomsvc.h  \
    qwheelzoomsvc.h \

PUBLIC_SOURCES += \
    qwtchartzoom.cpp  \
    qaxiszoomsvc.cpp  \
    qwheelzoomsvc.cpp \

# ==============================================================================

PRIVATE_HEADERS += \

PRIVATE_SOURCES += \

# ==============================================================================

uselib : !buildlib {
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
