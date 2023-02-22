#!/bin/bash
rm README.md
cp README.tmp README.md

tree=$(tree -L 2 -tf --noreport -I '*~' --charset ascii $1 |
       sed -e 's/| \+/  /g' -e 's/[|`]-\+/ */g' -e 's:\(* \)\(\(.*/\)\([^/]\+\)\):\1[\4](\2):g')

printf "# Project tree\n\n${tree}" | grep -v README | grep -v gen_readme >> README.md
