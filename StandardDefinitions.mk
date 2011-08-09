#############################################################################
## Makefile -- New Version of my Makefile that works on both linux
##              and mac os x
## Ryan Nichol <rjn@hep.ucl.ac.uk>
##############################################################################
#Site Specific  Flags
include ../Makefile.arch  #Silly thing to do but there we are
#If you have things in non standard paths (eg. libRootFftwWrapper) append the appropriate -I or -L lines below
SYSINCLUDES	=
SYSLIBS         = 

#USE_GOOGLE_PROFILER=1 #Must comment this out

ifdef ARA_UTIL_INSTALL_DIR
ARA_UTIL_LIB_DIR=${ARA_UTIL_INSTALL_DIR}/lib
ARA_UTIL_BIN_DIR=${ARA_UTIL_INSTALL_DIR}/bin
ARA_UTIL_INC_DIR=${ARA_UTIL_INSTALL_DIR}/include
ARA_UTIL_CALIB_DIR=$(ARA_UTIL_INSTALL_DIR)/share/araCalib
ARA_UTIL_CONFIG_DIR=$(ARA_UTIL_INSTALL_DIR)/share/araConfig
else
ARA_UTIL_LIB_DIR=/usr/local/lib
ARA_UTIL_INC_DIR=/usr/local/include
ARA_UTIL_BIN_DIR=/usr/local/bin
ARA_UTIL_CALIB_DIR=/usr/local/share/araCalib
ARA_UTIL_CONFIG_DIR=/usr/local/share/araConfig
endif

DLLSUF = ${DllSuf}
OBJSUF = ${ObjSuf}
SRCSUF = ${SrcSuf}


#Toggles the FFT functions on and off
USE_FFT_TOOLS=1

ifdef USE_FFT_TOOLS
FFTLIBS = -lRootFftwWrapper -lfftw3 -lMathMore -lgsl
FFTFLAG = -DUSE_FFT_TOOLS
else
FFTLIBS =
FFTFLAG =
endif

ifdef USE_GOOGLE_PROFILER
SYSLIBS += -L/home/rjn/thirdParty/lib -lprofiler -ltcmalloc
endif
