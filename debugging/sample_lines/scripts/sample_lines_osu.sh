BIN_FILE=${ARA_UTIL_INSTALL_DIR}/bin/makeSampleLineHistos
RUN_FILE=/unix/ara/data/osu2012/root/run$1/event$1.root
OUT_FILE=/unix/ara/data/osu2012/calibration_output/sample_lines/output/run$1_osu.root
IS_MATRI=0

$BIN_FILE $RUN_FILE $OUT_FILE $IS_MATRI
#$BIN_FILE