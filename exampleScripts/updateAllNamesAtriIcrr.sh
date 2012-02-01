#!/bin/bash

function doFindAndReplace() {
    if [[ $3 = "" ]]; then
	echo "`basename $0` <old> <new> <file> "
	exit 1
    fi
    
    OLD=$1
    NEW=$2
    FILE=$3
    #cp $3 $3.old

    sed "s/${1}/${2}/g" $3 > $3.new
    mv $3.new $3
}


for file in *.h *.cxx Makefile; do
    echo $file
#    doFindAndReplace AraOne Atri $file
#    doFindAndReplace AraTestBed Icrr $file
#    doFindAndReplace ARAONE ATRI $file
#    doFindAndReplace ARATESTBED ICRR $file
#    doFindAndReplace TestBed Icrr $file
#    doFindAndReplace TESTBED ICRR $file
#    doFindAndReplace Testbed Icrr $file
    doFindAndReplace IcrrIcrr Icrr $file

done
