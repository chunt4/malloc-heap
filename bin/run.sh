#!/bin/bash

ff(){
	env LD_PRELOAD=lib/libmalloc-ff.so cat README.md
}

bf(){
	env LD_PRELOAD=lib/libmalloc-bf.so cat README.md
}

wf(){
	env LD_PRELOAD=lib/libmalloc-wf.so cat README.md
}

while [ $# -gt 0 ]; do
	case $1 in
		-f) ff;;
		-b) bf;;
		-w) wf;;
	esac
	shift
done


