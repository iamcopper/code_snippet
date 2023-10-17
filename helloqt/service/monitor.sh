#!/bin/bash

#
# An App runtime monitor script
#

APP1="/home/rk/helloqt/helloqt"
APP2="/usr/bin/unclutter -idle 3 -root"

# QT Application runtime environment variable
export XDG_SESSION_TYPE=x11
export XDG_DATA_DIRS=/usr/share/unity:/usr/local/share/:/usr/share/:/var/lib/snapd/desktop
export XDG_SESSION_DESKTOP=unity
export XDG_CURRENT_DESKTOP=Unity:Unity7:ubuntu
export XDG_RUNTIME_DIR=/run/user/1000
export XDG_CONFIG_DIRS=/etc/xdg/xdg-unity:/etc/xdg

while true; do
	APP=${APP1}
	is_app_running=$(ps -ef | grep "${APP}" | grep -v grep | awk '{print $8}' 2>/dev/null)
	if [[ ${is_app_running} == "" ]]; then
		echo "App (${APP}) stoped, restart it now ..."
		DISPLAY=:0 ${APP} &
	fi

	APP=${APP2}
	is_app_running=$(ps -ef | grep "${APP}" | grep -v grep | awk '{print $8}' 2>/dev/null)
	if [[ ${is_app_running} == "" ]]; then
		echo "App (${APP}) stoped, restart it now ..."
		DISPLAY=:0 ${APP} &
	fi

	sleep 1
done
