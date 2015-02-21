# DEBUG can be set to YES to include debugging info, or NO otherwise
DEBUG          := YES
#****************************************************************************
CC     := $(CROSS_COMPILE)gcc
CXX    := $(CROSS_COMPILE)g++
LD     := $(CROSS_COMPILE)ld
AR     := $(CROSS_COMPILE)ar
RANLIB := $(CROSS_COMPILE)ranlib

DEBUG_CFLAGS     := -Wall -Wno-format -g -DDEBUG -Wcpp -DBOOST_SIGNALS_NO_DEPRECATION_WARNING -std=c++11 -DTIXML_USE_STL -DDEV_MODE
RELEASE_CFLAGS   := -Wall -Wno-unknown-pragmas -Wno-format -O3 -Wcpp -DBOOST_SIGNALS_NO_DEPRECATION_WARNING -std=c++11 -DTIXML_USE_STL

LIBS		     := -lstdc++ -lwt -lwthttp -lboost_signals -lwtfcgi -lutils -lxmlparser
LIB_PATH	     := -L ../build

DEBUG_CXXFLAGS   := ${DEBUG_CFLAGS} 
RELEASE_CXXFLAGS := ${RELEASE_CFLAGS}

DEBUG_LDFLAGS    := -g
RELEASE_LDFLAGS  :=

ifeq (YES, ${DEBUG})
   CFLAGS       := ${DEBUG_CFLAGS}
   CXXFLAGS     := ${DEBUG_CXXFLAGS}
   LDFLAGS      := ${DEBUG_LDFLAGS}
else
   CFLAGS       := ${RELEASE_CFLAGS}
   CXXFLAGS     := ${RELEASE_CXXFLAGS}
   LDFLAGS      := ${RELEASE_LDFLAGS}
endif

#****************************************************************************
# Preprocessor directives
#****************************************************************************
DEFS :=

#****************************************************************************
# Include paths
#****************************************************************************
INCS :=-I inc -I utils/inc

#****************************************************************************
# Include paths
#****************************************************************************
BUILD_DIR := ../build


#****************************************************************************
# Makefile code common to all platforms
#****************************************************************************

CFLAGS   := ${CFLAGS}   ${DEFS}
CXXFLAGS := ${CXXFLAGS} ${DEFS}

#****************************************************************************
# Targets of the build
#****************************************************************************
OUTPUT := ${BUILD_DIR}/addypin.wt 

all: ${OUTPUT}


#****************************************************************************
# Source files
#****************************************************************************
SRCS := main.cpp AddyPin.cpp AddyPinAllocator.cpp AddyPinApp.cpp AddyAccountMngmnt.cpp AddyMasterInfo.cpp AddyUserInfo.cpp AddyInfoDialog.cpp AddyDB.cpp

# Add on the sources for libraries
SRCS := ${SRCS}

OBJS := $(addsuffix .o,$(basename ${SRCS}))
OBJS_ABS := $(addprefix ${BUILD_DIR}/,$(OBJS))

#****************************************************************************
# Output
#****************************************************************************

${OUTPUT}: ${OBJS_ABS}
	${CXX} ${LDFLAGS} ${OBJS_ABS} ${LIB_PATH} ${LIBS} ${EXTRA_LIBS} -o $@

# Rules for compiling source files to object files
${BUILD_DIR}/%.o : %.cpp
	${CXX} -c ${CXXFLAGS} ${INCS} $< -o ${BUILD_DIR}/$@

dist:
	bash makedistlinux

clean:
	-rm -f core ${OBJS_ABS} ${BUILD_DIR}/${OUTPUT}

depend:
	makedepend -Y -p${BUILD_DIR}/ --${CXXFLAGS} ${SRCS} ${INCS}

# DO NOT DELETE

../build/main.o: inc/AddyPinApp.h inc/AddyInfoDialog.h inc/AddyDB.h
../build/main.o: inc/AddyPin.h inc/AddyUserInfo.h inc/AddyMasterInfo.h
../build/main.o: inc/AddyPinAllocator.h inc/AddyPin.h inc/AddyDB.h
../build/main.o: inc/AddyUserInfo.h inc/AddyInfoDialog.h
../build/AddyPin.o: inc/AddyPin.h
../build/AddyPinAllocator.o: inc/AddyPinAllocator.h
../build/AddyPinApp.o: inc/AddyPinApp.h inc/AddyInfoDialog.h inc/AddyDB.h
../build/AddyPinApp.o: inc/AddyPin.h inc/AddyUserInfo.h inc/AddyMasterInfo.h
../build/AddyPinApp.o: inc/AddyPinAllocator.h inc/AddyAccountMngmnt.h
../build/AddyPinApp.o: inc/AddyText.h
../build/AddyAccountMngmnt.o: inc/AddyAccountMngmnt.h inc/AddyInfoDialog.h
../build/AddyAccountMngmnt.o: inc/AddyDB.h inc/AddyPin.h inc/AddyUserInfo.h
../build/AddyAccountMngmnt.o: inc/AddyMasterInfo.h inc/AddyPinAllocator.h
../build/AddyAccountMngmnt.o: inc/AddyText.h
../build/AddyMasterInfo.o: inc/AddyMasterInfo.h inc/AddyUserInfo.h
../build/AddyMasterInfo.o: inc/AddyPin.h utils/inc/strtk.hpp
../build/AddyMasterInfo.o: tinyxml/tinyxml.h
../build/AddyUserInfo.o: inc/AddyUserInfo.h utils/inc/strtk.hpp
../build/AddyUserInfo.o: utils/inc/CException.hpp utils/inc/types.h
../build/AddyUserInfo.o: tinyxml/tinyxml.h
../build/AddyInfoDialog.o: inc/AddyInfoDialog.h
../build/AddyDB.o: inc/AddyDB.h utils/inc/CException.hpp utils/inc/types.h
../build/AddyDB.o: tinyxml/tinyxml.h
