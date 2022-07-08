#!/bin/bash

function my_func()
{
	for (( i = 1; i <= $#; i++ )); do
		echo "[${FUNCNAME}] --- ${!i}"
	done
}

function traverse()
{
	local dir=$1
	local func=$2
	for file in $(ls ${dir})
	do
		current=${dir}/${file}
		if [[ -d ${current} ]]; then
			${FUNCNAME} ${current} ${func}
		else
			${func} ${current}
		fi
	done
}

if [[ $# -lt 1 ]]; then
	traverse . my_func
else
	traverse $1 my_func
fi
