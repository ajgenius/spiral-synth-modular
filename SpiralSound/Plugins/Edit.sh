#! /bin/sh

for file in `cat PluginList.txt`
do
echo $file
cd $file/
nedit Makefile.in
cd ..
done
