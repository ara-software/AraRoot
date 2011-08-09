#!/bin/bash

if [ "$2" = "" ]
then
    echo "usage `basename $0` <file base> <title>" 
    exit
fi

fileBase=$1
title=$2

for time in OneHour SixHours TwelveHours OneDay All; 
do
  echo "${fileBase}$time"
  shtmlFile=${fileBase}${time}.shtml
  shtmlFile2=content/${fileBase}${time}.shtml
  pngFile=${fileBase}${time}.png
  
  #Step one is to copy example
  cp exampleOneHour.shtml $shtmlFile
  rm -f $shtmlFile2

  
#  echo "<h3>QnDWBOM started on -- <!--#flastmod virtual=\"/monitor/timefile\"--></h3>" > $shtmlFile2
  echo "<h3>Current Time -- <!--#echo var=\"DATE_LOCAL\"--></h3>" >> $shtmlFile2
  echo "<h3>Current Time -- <!--#echo var=\"DATE_GMT\" --></h3>" >> $shtmlFile2
  echo "<h3>Last Event Packet -- <!--#flastmod virtual=\"/monitor/lastEvent\"--></h3>" >> $shtmlFile2
  
  echo "<a name=\"${fileBase}\"><h3>${title}</h3></a>" >> $shtmlFile2
  
  echo "<img style=\"border:outset\" src=\"${pngFile}\" alt=\"${fileBase}\" />" >> $shtmlFile2

done
