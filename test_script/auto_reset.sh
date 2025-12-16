#!/bin/bash

echo -e "*********************************"
echo -e "*          Auto Reset Test      *"
echo -e "*********************************"

HOST="192.168.100.101"
USR="root"
PWD="123"
SLEEP="60"
SSH_CMD="reboot"

LOOP=${1:-9999999999}
TIMEOUT=3
RETRY=3

echo -e "HOST=${HOST}"
echo -e "SSH_CMD=${SSH_CMD}"
echo -e "LOOP=${LOOP}\n"

for (( i = 0; i < ${LOOP}; i++ )); do
	echo -e "========== FOR LOOP $i =========="

	let retry=1
	CMD="timeout ${TIMEOUT} sshpass -p ${PWD} ssh -l ${USR} ${HOST} ${SSH_CMD}"
	echo -e "${CMD}"
	${CMD}
	ret=$?
	while [[ ${ret} -eq 124 ]] && [[ ${retry} -le ${RETRY} ]];
	do
		sleep 1
		echo -e "[INFO] ssh timeout, sleep 1s and retry=${retry}/${RETRY} ..."
		let retry=${retry}+1
		${CMD}
		ret=$?
	done
	echo "ret=${ret}"
	# ret=124 --- ssh timeout
	if [[ ${ret} -eq 124 ]]; then
		echo -e "[ERROR] ssh timeout, exit, i=${i}\n"
		exit 1
	# ret=0   --- command narmally exit
	# ret=255 --- close by remote
	elif [[ ${ret} -ne 0 ]] && [[ ${ret} -ne 255 ]]; then
		echo -e "[ERROR] ssh command exec error, exit, i=${i}\n"
		exit 1
	fi
	echo -e "Waiting ${SLEEP} seconds ... \n"
	sleep ${SLEEP}
done
