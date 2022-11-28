#!/bin/bash

rm -fr odevzdani odevzdani-test

cp -r src odevzdani
cp Makefile odevzdani/
sed -i 's/TARGETS = $(addprefix $(SOURCE_DIR), $(SOURCES))/TARGETS = $(SOURCES)/' ./odevzdani/Makefile
cp doc/dokumentace.pdf odevzdani

echo "xbielg00:25" >> odevzdani/rozdeleni
echo "xgabry01:25" >> odevzdani/rozdeleni
echo "xmikys03:25" >> odevzdani/rozdeleni
echo "xtobol06:25" >> odevzdani/rozdeleni

cd odevzdani
zip xbielg00.zip *
mv xbielg00.zip ../

cd ..

mkdir odevzdani-test
yes | ./is_it_ok.sh xbielg00.zip odevzdani-test

rm -fr odevzdani odevzdani-test
