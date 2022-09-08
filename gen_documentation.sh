#!/usr/bin/bash

cd ..; doxygen &>/dev/null
cd docs/latex || exit
pdflatex refman.tex &>/dev/null && makeindex refman.tex &>/dev/null && pdflatex refman.tex&>/dev/null
cp refman.pdf ../documentation.pdf
exit