#!/bin/bash

function my_func()
{
	echo "[${FUNCNAME}] $@"

	for (( i = 1; i <= $#; i++ )); do
		echo "[${FUNCNAME}] arg $i = ${!i}"
	done

}

function traverse()
{
	local dir=$1
	local func=$2
	shift 2
	for file in $(ls ${dir})
	do
		current=${dir}/${file}
		if [[ -d ${current} ]]; then
			${FUNCNAME} ${current} ${func} $@
		else
			${func} ${current} $@
		fi
	done
}

if [[ $# -lt 1 ]]; then
	echo -e "\nUsage: $0 <dir> [arg1] [arg2] ...\n"
	exit 1
fi

dir=$1
shift 1

traverse ${dir} my_func $@
