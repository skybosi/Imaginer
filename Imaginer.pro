TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    src/DPC/dataPcer.cpp \
    src/DPC/dpcComm.cpp \
    src/DPC/dpcCons.cpp \
    src/DPC/dpcCore.cpp \
    src/DPer/BmpPer.cpp \
    src/DPer/iConsper.cpp \
    src/MetaData/iColor.cpp \
    src/MetaData/iPixels.cpp \
    src/MetaData/iPoint.cpp \
    src/Utils/getOpt.cpp \
    src/Utils/iCsTransf.cpp \
    src/Utils/iFonts.cpp \
    src/Utils/iGenfuner.cpp \
    src/Utils/iMatrix.cpp \
    src/Utils/iRpn.cpp \
    src/Utils/iTransfMatrix.cpp \
    src/Utils/iUtils.cpp \
    src/Utils/iVector.cpp

HEADERS += \
    include/BaseData/ibmp.h \
    include/Commons/iCommons.h \
    include/DPC/dataPcer.h \
    include/DPC/dpcComm.h \
    include/DPC/dpcCons.h \
    include/DPC/dpcCore.h \
    include/DPer/BmpPer.h \
    include/DPer/DperMum.h \
    include/DPer/iConsper.h \
    include/MetaData/iColor.h \
    include/MetaData/imetadata.h \
    include/MetaData/iPixels.h \
    include/MetaData/iPoint.h \
    include/Utils/getOpt.h \
    include/Utils/iCsTransf.h \
    include/Utils/iFonts.h \
    include/Utils/iGenfuner.h \
    include/Utils/iMatrix.h \
    include/Utils/iRpn.h \
    include/Utils/iStack.h \
    include/Utils/iTransfMatrix.h \
    include/Utils/iUtils.h \
    include/Utils/iVector.h \
    include/linker.h

INCLUDEPATH += include \
    include/BaseData  \
    include/Commons              \
    include/DPC              \
    include/DPer              \
    include/MetaData              \
    include/Utils
