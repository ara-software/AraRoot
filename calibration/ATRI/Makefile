#############################################################################
## Makefile -- New Version of my Makefile that works on both linux
##              and mac os x
## Ryan Nichol <rjn@hep.ucl.ac.uk>
##############################################################################
ifdef ARA_ROOT_DIR
include ${ARA_ROOT_DIR}/StandardDefinitions.mk
endif

LD_ARA_UTIL=-L$(ARA_UTIL_LIB_DIR) -L${ARA_ROOT_DIR}/AraEvent -lAraEvent -lz
INC_ARA_UTIL=-I$(ARA_UTIL_INC_DIR) -I ${ARA_ROOT_DIR}/AraEvent


#Generic and Site Specific Flags
CXXFLAGS     += $(ROOTCFLAGS) $(FFTFLAG) $(SYSINCLUDES) $(INC_ARA_UTIL)
LDFLAGS      += -g $(ROOTLDFLAGS) 

LIBS          = $(ROOTLIBS)  -lMinuit $(SYSLIBS) $(LD_ARA_UTIL) $(FFTLIBS) 
GLIBS         = $(ROOTGLIBS) $(SYSLIBS)


PROGRAM =   firstCalibTry firstCalibTryJon

all : $(PROGRAM)



% :  %.$(SRCSUF)
	@echo "<**Linking**> "  
	$(LD)  $(CXXFLAGS) $(LDFLAGS) $<  $(LIBS) -o $@



clean:
	@rm -f *Dict*
	@rm -f *.${OBJSUF}
	@rm -f $(PROGRAM)

