BIN_FILE=${ARA_UTIL_INSTALL_DIR}/bin/makeAdcTree
RUN_FILE=/unix/ara/data/osu2012/root3/run$1/event$1.root
OUT_FILE=/unix/ara/data/osu2012/calibration_output/data_lines/output/run$1_osu3.root
IS_MATRI=0

$BIN_FILE $RUN_FILE $OUT_FILE $IS_MATRI
#$BIN_FILE