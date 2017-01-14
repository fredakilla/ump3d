#--------------------------------------------------------------------
# 3rdparty libraries path
#--------------------------------------------------------------------

unix:!macx: URHO3D = /home/fred/Documents/Urho3D/BUILD
win32: URHO3D = C:\dev\Urho3D\build

win32: MAGIC_PARTICLES = ..\lib\win

#--------------------------------------------------------------------
# target
#--------------------------------------------------------------------

TEMPLATE = app
CONFIG -= console
CONFIG += windows
QT += widgets xml
CONFIG += c++11
CONFIG -= debug_and_release
CONFIG -= debug_and_release_target

#--------------------------------------------------------------------
# output directory
#--------------------------------------------------------------------

CONFIG(debug,debug|release) {
    DESTDIR = $$PWD/bin
} else {
    DESTDIR = $$PWD/bin
}

QMAKE_CLEAN += $$DESTDIR/$$TARGET

#--------------------------------------------------------------------
# compilation flags
#--------------------------------------------------------------------

unix:!macx: QMAKE_CXXFLAGS += -Wall
unix:!macx: QMAKE_CXXFLAGS += -Wno-comment
unix:!macx: QMAKE_CXXFLAGS += -Wno-ignored-qualifiers
unix:!macx: QMAKE_CXXFLAGS += -Wno-unused-parameter
unix:!macx: QMAKE_CXXFLAGS += -Wno-reorder
unix:!macx: QMAKE_CXXFLAGS += -std=c++11

CONFIG(debug,debug|release) {
#message( debug )
    QMAKE_CXXFLAGS_DEBUG -= -O0
} else {
#message( release )
unix:!macx: QMAKE_CXXFLAGS += -Wno-strict-aliasing
win32:QMAKE_CXXFLAGS_RELEASE -= -Zc:strictStrings

# remove possible other optimization flags
QMAKE_CXXFLAGS_RELEASE -= -O
QMAKE_CXXFLAGS_RELEASE -= -O1
QMAKE_CXXFLAGS_RELEASE -= -O2
# add the desired -O3 if not present
QMAKE_CXXFLAGS_RELEASE *= -O3
}

#--------------------------------------------------------------------
# defines
#--------------------------------------------------------------------

CONFIG(debug,debug|release) {
    #message( debug )
    DEFINES += \
        MAGIC_3D \
        SHADER_ALPHATEST_WRAP \
        ASSERT_DEBUG \
        _WINDOWS
} else {
    DEFINES += \
        MAGIC_3D \
        SHADER_ALPHATEST_WRAP \
        _WINDOWS
}


#--------------------------------------------------------------------
# libraries includes
#--------------------------------------------------------------------

INCLUDEPATH += $${URHO3D}/include
INCLUDEPATH += $${URHO3D}/include/Urho3D/ThirdParty
INCLUDEPATH += $${MAGIC_PARTICLES}

#--------------------------------------------------------------------
# linker
#--------------------------------------------------------------------

LIBS += -L$${URHO3D}/lib
LIBS += -L$${DESTDIR}
win32: LIBS += -L$${MAGIC_PARTICLES}

unix:!macx:: LIBS += -lmagic3d
unix:!macx:: LIBS += -lUrho3D

win32: LIBS += -lmagic3d_x64
win32: CONFIG(debug,debug|release) {
    LIBS += -lUrho3D_d
} else {
    LIBS += -lUrho3D
}

unix:!macx: LIBS +=  -lXi -ldl -lpthread -lGL -lGLU -lX11

win32:LIBS += kernel32.lib user32.lib gdi32.lib comdlg32.lib advapi32.lib shell32.lib \
                ole32.lib oleaut32.lib uuid.lib imm32.lib winmm.lib wsock32.lib opengl32.lib glu32.lib version.lib

#--------------------------------------------------------------------
# project files
#--------------------------------------------------------------------

HEADERS += \
    MagicParticleEffect.h \
    MagicParticleEmitter.h \
    Magic.h


SOURCES += \
    MagicParticleEffect.cpp \
    MagicParticleEmitter.cpp \
    main.cpp
