#! /bin/sh

for file in `cat PluginList.txt`
do
echo $file
cd $file/
cvs edit Makefile.in
nedit Makefile.in
cd ..
done
