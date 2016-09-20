#!/bin/bash
if [ -n "$1" ]; then
tar -zcvf ./miracast.tar.gz $1

ftp 172.17.186.2 << EOF
root
cd /home/root
put miracast.tar.gz
close
EOF
rm ./miracast.tar.gz
else
echo "please input dir"
fi
