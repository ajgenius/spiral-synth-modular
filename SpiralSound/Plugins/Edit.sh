#! /bin/sh

for file in `cat PluginList.txt`
do
echo $file
cd $file/
nedit $file.h
nedit $file.C
cd ..
done
