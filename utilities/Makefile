#############################################################################
## Makefile -- New Version of my Makefile that works on both linux
##              and mac os x
## Ryan Nichol <rjn@hep.ucl.ac.uk>
##############################################################################
include ../StandardDefinitions.mk

LD_ARA_UTIL=-L$(ARA_UTIL_LIB_DIR) -L../AraEvent -lAraEvent -lz
INC_ARA_UTIL=-I$(ARA_UTIL_INC_DIR) -I../AraEvent


#Generic and Site Specific Flags
CXXFLAGS     += $(ROOTCFLAGS) $(FFTFLAG) $(SYSINCLUDES) $(INC_ARA_UTIL)
LDFLAGS      += -g $(ROOTLDFLAGS) 

LIBS          = $(ROOTLIBS)  -lMinuit $(SYSLIBS) $(LD_ARA_UTIL) $(FFTLIBS) 
GLIBS         = $(ROOTGLIBS) $(SYSLIBS)


TREE_MAKER = makeAraEventTree makeAraHkTree makeAraCalibratedEventTree
SCRIPTS = runAraEventFileMaker.sh runAraTimeFileMaker.sh

all : $(TREE_MAKER)



% :  %.$(SRCSUF)
	@echo "<**Linking**> "  
	$(LD)  $(CXXFLAGS) $(LDFLAGS) $<  $(LIBS) -o $@

install: $(TREE_MAKER)
	install -d $(ARA_UTIL_BIN_DIR)
	install -c -m 755 $(TREE_MAKER) $(SCRIPTS) $(ARA_UTIL_BIN_DIR)


clean:
	@rm -f *Dict*
	@rm -f *.${OBJSUF}
	@rm -f $(TREE_MAKER)

