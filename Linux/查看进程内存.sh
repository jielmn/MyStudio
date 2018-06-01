#!/bin/sh
until [ $# -eq 0 ]
do
	if [ -e /proc/$1 ]; then
		echo -e "$1\t`cut -d' ' -f2 /proc/$1/statm`"
	fi
	shift
done
