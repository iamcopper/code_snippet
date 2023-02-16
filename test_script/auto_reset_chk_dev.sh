#!/bin/bash

echo -e "*********************************************"
echo -e "*             Auto Reset Test"
echo -e "*"
echo -e "* - To test and summery video dev node detected"
echo -e "*   during every boot."
echo -e "*********************************************"

HOST="192.168.1.184"
USR="root"
PWD="123"
SLEEP="80"
declare -A SSH_CMD_ARRAY
SSH_CMD_ARRAY[0]="ls /dev/video* | wc -w 2>/dev/null"
SSH_CMD_ARRAY[1]="ls /dev/video* 2>/dev/null"
SSH_CMD_ARRAY[2]="reboot"

declare -A VIDEO_ARRAY
DEV_NUM_MAX=9
for (( DEV_NUM = 0; DEV_NUM < DEV_NUM_MAX; DEV_NUM++ )); do
	DEV_NUM_TIMES_ARRAY[${DEV_NUM}]=0
done

LOOP=$1
TIMEOUT=5

echo -e "HOST=${HOST}"
echo -e "LOOP=${LOOP}\n"

for (( i = 1; i <= ${LOOP}; i++ )); do
	echo -e "========== FOR LOOP $i/${LOOP} =========="

	let j=0
	for SSH_CMD in "${SSH_CMD_ARRAY[@]}"; do
		if [[ ${j} -eq 0 ]]; then
			CUR_DEV_NUM=$(timeout ${TIMEOUT} sshpass -p ${PWD} ssh -l ${USR} ${HOST} ${SSH_CMD})
		else
			CMD="timeout ${TIMEOUT} sshpass -p ${PWD} ssh -l ${USR} ${HOST} ${SSH_CMD}"
			ret1=$?
			echo -e "${CMD}"
			${CMD}
			ret2=$?
		fi
		let j=${j}+1
	done

	if [[ ${ret1} == 124 || ${ret2} == 124 ]]; then
		echo -e "[ERROR] ssh timeout, exit, i=${i}\n"
		exit 1
	fi

	if [[ ${CUR_DEV_NUM} -ge 0 ]] && [[ ${CUR_DEV_NUM} -le 8 ]]; then
		let DEV_NUM_TIMES_ARRAY[${CUR_DEV_NUM}]=${DEV_NUM_TIMES_ARRAY[${CUR_DEV_NUM}]}+1
	else
		echo -e "[ERROR] Get CUR_DEV_NUM error, exit\n"
		exit 1
	fi

	for (( DEV_NUM = 0; DEV_NUM < DEV_NUM_MAX; DEV_NUM++ )); do
		echo -e "DEV_NUM_TIMES_ARRAY[${DEV_NUM}] = ${DEV_NUM_TIMES_ARRAY[${DEV_NUM}]}"
	done

	if [[ i -lt ${LOOP} ]]; then
		echo -e "Waiting ${SLEEP} seconds ... \n"
		sleep ${SLEEP}
	fi
done

echo -e "\n========== Test Result =========="
echo -e "Totally test loops: ${LOOP}"
for (( DEV_NUM = 0; DEV_NUM < DEV_NUM_MAX; DEV_NUM++ )); do
	echo -e "DEV_NUM_TIMES_ARRAY[${DEV_NUM}] = ${DEV_NUM_TIMES_ARRAY[${DEV_NUM}]}"
done
