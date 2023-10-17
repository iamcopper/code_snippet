#!/bin/bash

APP=helloqt
SERVICE=app_monitor.service

# build and install QT app
echo ">>> Build and install QT app"
cd .. && \
	./build.sh \
	&& cd -

# install service
echo ">>> Install service"
sudo ln -sf $(pwd)/../${SERVICE} /lib/systemd/system/${SERVICE}
sudo systemctl daemon-reload
sudo systemctl enable ${SERVICE} 2> /dev/null

# install desktop
#sudo ln -sf $(pwd)/../${APP}.desktop /etc/xdg/autostart/${APP}.desktop
