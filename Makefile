#############################################################################
## Makefile -- New Version of my Makefile that works on both linux
##              and mac os x
## Ryan Nichol <rjn@hep.ucl.ac.uk>
##############################################################################
include Makefile.arch

#Site Specific  Flags
SYSINCLUDES	=
SYSLIBS         = 
DLLSUF = ${DllSuf}
OBJSUF = ${ObjSuf}
SRCSUF = ${SrcSuf}

ifdef ARA_UTIL_INSTALL_DIR
ARA_UTIL_LIB_DIR=${ARA_UTIL_INSTALL_DIR}/lib
ARA_UTIL_INC_DIR=${ARA_UTIL_INSTALL_DIR}/include
else
ARA_UTIL_LIB_DIR=/usr/local/lib
ARA_UTIL_INC_DIR=/usr/local/include
endif
LD_ARA_UTIL=-L$(ARA_UTIL_LIB_DIR)
INC_ARA_UTIL=-I$(ARA_UTIL_INC_DIR)

#Generic and Site Specific Flags
CXXFLAGS     += $(ROOTCFLAGS) $(FFTFLAG) $(SYSINCLUDES) $(INC_ARA_UTIL)
LDFLAGS      += -g $(ROOTLDFLAGS) 

LIBS          = $(ROOTLIBS)  -lMinuit $(SYSLIBS) $(LD_ARA_UTIL) $(FFTLIBS) -lAraEvent
GLIBS         = $(ROOTGLIBS) $(SYSLIBS)



all : 
	@cd AraEvent; make all
	@cd AraDisplay; make all
	@cd AraCorrelator; make all
	@cd utilities; make all
	@cd AraWebPlotter; make all



AraEvent/libAraEvent.so:
	cd AraEvent ; make 

AraDisplay/libAraDisplay.so:
	cd AraDisplay ; make 

AraWebPlotter/AraWebRootFileMaker:
	cd AraWebPlotter ; make


% :  %.$(SRCSUF)
	@echo "<**Linking**> "  
	$(LD)  $(CXXFLAGS) $(LDFLAGS) $<  $(LIBS) -o $@

progs:
	cd utilities; make

install:
	@cd AraEvent; make install
	@cd AraDisplay; make install
	@cd AraCorrelator; make install
	@cd utilities; make install
	@cd AraWebPlotter; make install


clean:
	cd AraEvent; make clean
	cd AraDisplay; make clean
	cd AraCorrelator; make clean
	cd utilities; make clean
	cd AraWebPlotter; make clean
	@rm -f *Dict*
	@rm -f *.${OBJSUF}
	@rm -f $(TREE_MAKER)

