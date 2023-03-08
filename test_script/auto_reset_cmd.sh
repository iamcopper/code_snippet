#!/bin/bash

set -u

echo -e "********************************************************"
echo -e "*             Auto Reset Test"
echo -e "*"
echo -e "* - To execute a specific cmd via ssh in multiple times,"
echo -e "*   and summary the times for each result.              "
echo -e "********************************************************"

HOST="192.168.1.159"
USR="root"
PWD="123"
SLEEP="80"

#CMD="ls /dev/video*"
#WC_CMD="wc -w"

CMD="lsblk | grep nvme"
WC_CMD="wc -l"

########################################################

declare -A WC_NUM_ARRAY
WC_NUM_MAX=8
for (( j = 0; j <= ${WC_NUM_MAX}; j++ )); do
	WC_NUM_ARRAY[${j}]=0
done

if [ $# -ge 1 ]; then
	LOOP=$1
else
	LOOP=1
fi
TIMEOUT=5

echo -e "HOST=${HOST}"
echo -e "LOOP=${LOOP}\n"

for (( i = 1; i <= ${LOOP}; i++ )); do
	echo -e "============= FOR LOOP $i/${LOOP} ============="

	# Execute CMD
	SSH_CMD="timeout ${TIMEOUT} sshpass -p ${PWD} ssh -l ${USR} ${HOST} ${CMD}"
	echo -e ">>> SSH_CMD\n${SSH_CMD}"

	SSH_CMD_OUTPUT=$(${SSH_CMD})
	ssh_ret=$?
	echo -e ">>> SSH_CMD_OUTPUT\n${SSH_CMD_OUTPUT}"
	if [[ ${ssh_ret} == 124 ]]; then
		echo -e "[ERROR] ssh cmd timeout, exit, i=${i}\n"
		exit 1
	fi

	WC_NUM=$(echo -e "${SSH_CMD_OUTPUT}" | ${WC_CMD})
	echo -e ">>> WC_NUM=${WC_NUM}"
	if [[ ${WC_NUM} -ge 0 ]] && [[ ${WC_NUM} -le ${WC_NUM_MAX} ]]; then
		let WC_NUM_ARRAY[${WC_NUM}]=${WC_NUM_ARRAY[${WC_NUM}]}+1
	else
		echo -e "[ERROR] Get WC_NUM (${WC_NUM}) error, exit\n"
		exit 1
	fi
	echo -e ">>> WC_NUM_ARRAY"
	for (( j = 0; j < ${#WC_NUM_ARRAY[@]}; j++ )); do
		echo -e "WC_NUM_ARRAY[${j}] = ${WC_NUM_ARRAY[${j}]}"
	done

	# Reboot
	timeout ${TIMEOUT} sshpass -p ${PWD} ssh -l ${USR} ${HOST} reboot
	ssh_ret=$?
	if [[ ${ssh_ret} == 124 ]]; then
		echo -e "[ERROR] ssh reboot timeout, exit, i=${i}\n"
		exit 1
	fi

	if [[ i -lt ${LOOP} ]]; then
		echo -e "Waiting ${SLEEP} seconds ... \n"
		sleep ${SLEEP}
	fi

done

echo -e "\n============= Test Result ============="
echo -e "Totally test loops: ${LOOP}"
for (( j = 0; j < ${#WC_NUM_ARRAY[@]}; j++ )); do
	echo -e "WC_NUM_ARRAY[${j}] = ${WC_NUM_ARRAY[${j}]}"
done
