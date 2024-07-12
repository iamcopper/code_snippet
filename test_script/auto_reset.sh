#!/bin/bash

echo -e "*********************************"
echo -e "*          Auto Reset Test      *"
echo -e "*********************************"

HOST="192.168.2.53"
USR="root"
PWD="123"
SLEEP="60"
SSH_CMD="reboot"

LOOP=$1
TIMEOUT=3
RETRY=5

echo -e "HOST=${HOST}"
echo -e "CMD=${CMD}"
echo -e "LOOP=${LOOP}\n"

for (( i = 0; i < ${LOOP}; i++ )); do
	echo -e "========== FOR LOOP $i =========="

	let retry=1
	CMD="timeout ${TIMEOUT} sshpass -p ${PWD} ssh -l ${USR} ${HOST} ${SSH_CMD}"
	echo -e "${CMD}"
	${CMD}
	while [[ $? == 124 ]] && [[ ${retry} -le ${RETRY} ]];
	do
		sleep 3
		echo -e "[INFO] ssh timeout, sleep 3s and retry=${retry}/${RETRY} ...\n"
		let retry=${retry}+1
		${CMD}
	done
	if [[ $? == 124 ]]; then
		echo -e "[ERROR] ssh timeout, exit, i=${i}\n"
		exit 1
	fi
	echo -e "Waiting ${SLEEP} seconds ... \n"
	sleep ${SLEEP}
done
