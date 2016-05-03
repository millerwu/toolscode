#!/bin/bash

echo "start check GCC version in delivery"

search_path=$1

if [ "$search_path" = "" ]; then
	echo "please input filepath"
	echo "sample: ./check_gcc_version.sh ./"
	exit
fi

echo "search path = $search_path"

line_num=`find $1 -name *.so | wc -l`

for k in $( seq 1 ${line_num}); do
	file_name=`find $1 -name *.so | awk "NR == $k" `
	echo "${file_name}"
	strings -a ${file_name} | grep GCC
done

echo "find ${line_num} shared lib"