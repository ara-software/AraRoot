year=$1
day=$2

if [ $year -eq 2012 ]; then FilePrefix=SPS-ARA; DIR=/data/exp/ARA/2012/filtered/L0/$day; fi
if [ $year -eq 2011 ]; then FilePrefix=TestBed; DIR=/data/exp/ARA/2011/minbias/L0/$day; fi

echo Doing Day $day for year $year using file name $FilePrefix in $DIR/$FilePrefix
mkdir out
mkdir out/$1
mkdir out/$1/out_$2
cd out/$1/out_$2
for file in `ls $DIR/$FilePrefix*.root`; do
  runNumber=$(echo "$file" | awk '{split($0,temp,"un"); split(temp[2],temp,"."); print temp[1]}')
  echo $file run $runNumber

  ../../../exampleLoop $file $runNumber

done

