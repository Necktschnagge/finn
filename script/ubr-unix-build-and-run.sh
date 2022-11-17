#!/bin/bash

if [ ! "${PWD##*/}" = script ]; then exit 1; fi
file="summary.json"

if [ -f "$file" ]; then rm -v "$file"; else echo "File "$file" did not exist."; fi

echo -e "\nCreating Project ..."
cmake -S .. -B ../build/ > ucbp-unix-create-build-project.log 2>&1
echo "Building Project ..."
cmake --build ../build >> ucbp-unix-create-build-project.log 2>&1

echo -e "\nRunning Project ...\n"
script -c ./ur-unix-run.sh ur-unix-run-typescript.log > /dev/null 2>&1
cat ur-unix-run-typescript.log | perl -pe 's/\e([^\[\]]|\[.*?[a-zA-Z]|\].*?\a)//g' | col -b > ur-unix-run.log
rm ur-unix-run-typescript.log

[ -f "$file" ] && echo "New "$file" was created."

if [ ! -f "$file" ]; then echo "Error: "$file" was not created."; fi

