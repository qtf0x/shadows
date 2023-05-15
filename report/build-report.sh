#!/bin/sh

echo "Building..."

rm -rf ./build/* 2> /dev/null
mkdir build 2> /dev/null
pdflatex -output-directory=build final-report.tex  > /dev/null 2>&1
cp ./sources.bib ./build/
cd ./build/
bibtex final-report > /dev/null 2>&1
cd ..
pdflatex -output-directory=build final-report.tex  > /dev/null 2>&1
pdflatex -output-directory=build final-report.tex  > /dev/null 2>&1
cp ./build/final-report.pdf ./

echo "Done!"
