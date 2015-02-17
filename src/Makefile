# DEBUG can be set to YES to include debugging info, or NO otherwise
DEBUG          := YES
#****************************************************************************
CC     := $(CROSS_COMPILE)gcc
CXX    := $(CROSS_COMPILE)g++
LD     := $(CROSS_COMPILE)ld
AR     := $(CROSS_COMPILE)ar
RANLIB := $(CROSS_COMPILE)ranlib

DEBUG_CFLAGS     := -Wall -Wno-format -g -DDEBUG -Wcpp -DBOOST_SIGNALS_NO_DEPRECATION_WARNING -std=c++11
RELEASE_CFLAGS   := -Wall -Wno-unknown-pragmas -Wno-format -O3

LIBS		     := -lstdc++ -lwt -lwthttp -lboost_signals -lwtfcgi -lutils
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
	${CXX} -lwtfcgi -c ${CXXFLAGS} ${INCS} $< -o ${BUILD_DIR}/$@

dist:
	bash makedistlinux

clean:
	-rm -f core ${OBJS_ABS} ${BUILD_DIR}/${OUTPUT}

depend:
	makedepend -Y -p${BUILD_DIR}/ --${CXXFLAGS} --${SRCS} ${INCS}

# DO NOT DELETE

../build/AddyPin.o: inc/AddyPin.h
../build/AddyPinAllocator.o: inc/AddyPinAllocator.h
../build/AddyPinApp.o: inc/AddyPinApp.h inc/AddyDB.h inc/AddyPin.h
../build/AddyPinApp.o: inc/AddyUserInfo.h inc/AddyMasterInfo.h
../build/AddyPinApp.o: inc/AddyPinAllocator.h utils/inc/CHdfManager.hpp
../build/AddyPinApp.o: utils/inc/types.h utils/inc/neo_hdf.h
../build/AddyPinApp.o: utils/inc/neo_err.h utils/inc/neo_misc.h
../build/AddyPinApp.o: utils/inc/cs_config.h utils/inc/neo_str.h
../build/AddyPinApp.o: utils/inc/ulist.h utils/inc/neo_hash.h
../build/AddyPinApp.o: utils/inc/CHdfFileAdapter.hpp
../build/AddyPinApp.o: utils/inc/IHdfStorage.hpp utils/inc/CPosixFile.hpp
../build/AddyPinApp.o: utils/inc/IFile.hpp inc/AddyAccountMngmnt.h
../build/AddyPinApp.o: inc/AddyInfoDialog.h
../build/AddyAccountMngmnt.o: inc/AddyAccountMngmnt.h inc/AddyDB.h
../build/AddyAccountMngmnt.o: inc/AddyPin.h inc/AddyUserInfo.h
../build/AddyAccountMngmnt.o: inc/AddyMasterInfo.h inc/AddyPinAllocator.h
../build/AddyAccountMngmnt.o: utils/inc/CHdfManager.hpp utils/inc/types.h
../build/AddyAccountMngmnt.o: utils/inc/neo_hdf.h utils/inc/neo_err.h
../build/AddyAccountMngmnt.o: utils/inc/neo_misc.h utils/inc/cs_config.h
../build/AddyAccountMngmnt.o: utils/inc/neo_str.h utils/inc/ulist.h
../build/AddyAccountMngmnt.o: utils/inc/neo_hash.h
../build/AddyAccountMngmnt.o: utils/inc/CHdfFileAdapter.hpp
../build/AddyAccountMngmnt.o: utils/inc/IHdfStorage.hpp
../build/AddyAccountMngmnt.o: utils/inc/CPosixFile.hpp utils/inc/IFile.hpp
../build/AddyMasterInfo.o: inc/AddyMasterInfo.h inc/AddyUserInfo.h
../build/AddyMasterInfo.o: inc/AddyPin.h utils/inc/strtk.hpp
../build/AddyUserInfo.o: inc/AddyUserInfo.h inc/AddyPin.h utils/inc/strtk.hpp
../build/AddyInfoDialog.o: inc/AddyInfoDialog.h
../build/AddyDB.o: inc/AddyDB.h inc/AddyPin.h inc/AddyUserInfo.h
../build/AddyDB.o: inc/AddyMasterInfo.h inc/AddyPinAllocator.h
../build/AddyDB.o: utils/inc/CHdfManager.hpp utils/inc/types.h
../build/AddyDB.o: utils/inc/neo_hdf.h utils/inc/neo_err.h
../build/AddyDB.o: utils/inc/neo_misc.h utils/inc/cs_config.h
../build/AddyDB.o: utils/inc/neo_str.h utils/inc/ulist.h utils/inc/neo_hash.h
../build/AddyDB.o: utils/inc/CHdfFileAdapter.hpp utils/inc/IHdfStorage.hpp
../build/AddyDB.o: utils/inc/CPosixFile.hpp utils/inc/IFile.hpp
../build/AddyDB.o: utils/inc/CException.hpp
