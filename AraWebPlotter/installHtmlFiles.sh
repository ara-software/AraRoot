#/bin/bash



MONITOR_DIR=`getDirFromConfig --plotDir`
HTML_REL_DIR=/uhen/ara/monitor 
# because http://www.hep.ucl.ac.uk/uhen/ara/monitor/ 

#First up copy the html files across
rsync -av --exclude '.svn' html/araweb/ $MONITOR_DIR

cd $MONITOR_DIR
for file in *.shtml */*.shtml */*/*.shtml */*.php */*/*.php */*/*/*.php;
do
  mv $file $file.old
  sed "s#/monitor#${HTML_REL_DIR}#g" $file.old > $file       
  rm $file.old
done
