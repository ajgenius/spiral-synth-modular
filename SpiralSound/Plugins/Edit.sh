#! /bin/sh

for file in `cat PluginList.txt`
do
echo $file
cd $file/
chmod +w $file.C
python ../sr.py $file.C ../sr.csv
cd ..
done
