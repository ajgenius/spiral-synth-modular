#! /bin/sh

for file in `cat PluginList.txt`
do
echo $file
cd $file/
chmod +w $file.C
nedit $file.C
cd ..
done
