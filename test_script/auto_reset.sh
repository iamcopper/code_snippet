#!/bin/bash

echo -e "*************************************"
echo -e "*          Auto Reset Test          *"
echo -e "*************************************"

HOST="192.168.1.195"
USR="root"
PWD="123"
SLEEP="60"
SSH_CMD="reboot"

LOOP=$1
TIMEOUT=3

echo -e "HOST=${HOST}"
echo -e "CMD=${CMD}"
echo -e "LOOP=${LOOP}\n"

for (( i = 0; i < ${LOOP}; i++ )); do
	echo -e "========== FOR LOOP $i =========="

	CMD="timeout ${TIMEOUT} ssh -l ${USR} ${HOST} ${SSH_CMD}"
	echo -e "${CMD}"
	${CMD}
	if [[ $? == 124 ]]; then
		echo -e "[ERROR] ssh timeout, exit, i=${i}\n"
		exit 1
	fi
	echo -e "Waiting ${SLEEP} seconds ... \n"
	sleep ${SLEEP}
done
