#!/bin/bash

#
# Log the boot timestamp and boot count,
# should be called during startup
#

CUR_DIR=$(cd $(dirname -- ${BASH_SOURCE[0]}) &> /dev/null && pwd)
BOOT_CNT_FILE=${CUR_DIR}/boot_cnt.txt
BOOT_LOG_FILE=${CUR_DIR}/boot_log.txt

if [[ ! -f ${BOOT_CNT_FILE} ]]; then
	bootcnt=1
else
	let bootcnt=$(cat ${BOOT_CNT_FILE})+1
fi
echo ${bootcnt} > ${BOOT_CNT_FILE}

boottime=$(date +"%Y/%m/%d %H:%M:%S")

echo -e "${boottime}\t${bootcnt}" >> ${BOOT_LOG_FILE}
