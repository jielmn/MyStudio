#!/bin/bash

remote_host=
user_name=
password=

if [ $# -le 0 ]; then
	echo "Usage: ./mount.sh path [remote host] [user name] [password]"
	exit
fi

if ! [ -d $1 ]; then
	mkdir $1;
fi

if [ $# -ge 2 ]; then
	remote_host=$2
fi

if [ $# -ge 3 ]; then
	user_name=$3
fi

if [ $# -ge 4 ]; then
	password=$4
fi

mount -t cifs -o username=${user_name},password=${password} //${remote_host}/$1 $1

