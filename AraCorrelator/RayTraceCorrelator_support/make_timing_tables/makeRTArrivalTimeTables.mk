LDFLAGS= -L${shell root-config --libdir}  -L${ARA_DEPS_INSTALL_DIR}/lib -L${ARA_UTIL_INSTALL_DIR}/lib -L${ARA_UTIL_INSTALL_DIR}/../../source/AraSim 
CXXFLAGS= -I${shell root-config --cflags} -I${ARA_DEPS_INSTALL_DIR}/include -I${ARA_UTIL_INSTALL_DIR}/include -I${ARA_UTIL_INSTALL_DIR}/../../source/AraSim
LDLIBS += ${shell root-config --libs} -lAraEvent -lRootFftwWrapper -lAra -lAraCorrelator