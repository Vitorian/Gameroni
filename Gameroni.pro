#-----------------------------------------------------------------------------
#    This file is part of Gameroni
#    http://www.vitorian.com/x1/archives/419
#    https://github.com/hftrader/gameroni
#
#    Gameroni is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    Gameroni is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
#
#-----------------------------------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Gameroni
TEMPLATE = app

SOURCES += main.cpp\
        mainwindow.cpp \
    windowcapture.cpp \
    windowcapturewidget.cpp \
    imageprocessor.cpp \
    win32utils.cpp \
    imageprocessorlaplacian.cpp \
    imageprocessormotion.cpp \
    snapshotcapture.cpp

HEADERS  += mainwindow.h \
    windowcapture.h \
    windowcapturewidget.h \
    imageprocessor.h \
    win32utils.h \
    imageprocessorlaplacian.h \
    imageprocessormotion.h \
    snapshotcapture.h

FORMS    += mainwindow.ui


win32 { 
  LIBS += -L$$PWD/3rdParty/v140-x64/lib/
  LIBS += -luser32 -lgdi32
  LIBS += -lopencv_core310 -lopencv_calib3d310 -lopencv_features2d310 -lopencv_highgui310 -lopencv_imgcodecs310 -lopencv_imgproc310 -lopencv_ml310 -lopencv_objdetect310 -lopencv_photo310 -lopencv_shape310 -lopencv_stitching310 -lopencv_superres310 -lopencv_video310 -lopencv_videoio310 -lopencv_videostab310 -lopencv_shape310
  INCLUDEPATH += $$PWD/3rdParty/v140-x64/include
  DEPENDPATH += $$PWD/3rdParty/v140-x64/include
  QMAKE_CXXFLAGS +=  /fp:fast /Zp8 /GS- /Gy
}

