#!/bin/bash

echo -e "*********************************"
echo -e "*          Auto Reset Test      *"
echo -e "*********************************"

HOST="192.168.100.101"
USR="root"
PWD="123"
SLEEP="90"
SSH_CMD="reboot"

LOOP=${1:-9999999999}
RETRY=3

echo -e "HOST=${HOST}"
echo -e "SSH_CMD=${SSH_CMD}"
echo -e "LOOP=${LOOP}\n"

for (( i = 0; i < ${LOOP}; i++ )); do
	echo -e "========== FOR LOOP $i =========="

	let retry=1
	CMD="ping -c 3 ${HOST}"
	echo -e "\n>>> ${CMD}"
	${CMD}
	ret=$?
	while [[ ${ret} -ne 0 ]] && [[ ${retry} -le ${RETRY} ]];
	do
		sleep 3
		echo -e "[ERROR] ping to host failed, retry=${retry}/${RETRY}"
		let retry=${retry}+1
		${CMD}
		ret=$?
	done
	if [[ ${ret} -ne 0 ]]; then
		echo -e "[ERROR] ping to host failed, exit.\n"
		exit 1
	fi

	CMD="timeout 20 sshpass -p ${PWD} ssh -l ${USR} ${HOST} ${SSH_CMD}"
	echo -e "\n>>> ${CMD}"
	${CMD}
	ret=$?
	echo -e "ret=${ret}"
	# ssh ret=0   --- command narmally exit
	# ssh ret=124 --- ssh timeout
	# ssh ret=255 --- close by remote, or network error
	if [[ ${ret} -ne 0 ]] && [[ ${ret} -ne 255 ]]; then
		if [[ ${ret} -eq 124 ]]; then
			echo -e "[ERROR] ssh command timeout, exit.\n"
		else
			echo -e "[ERROR] ssh command error, exit.\n"
		fi
		exit 1
	fi
	echo -e "\n>>> sleep ${SLEEP} ...\n"
	sleep ${SLEEP}
done
