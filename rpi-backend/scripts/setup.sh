#!/bin/bash

cd rpi-backend

# CMake

CMAKE_DIR="cmake-build-debug"
if [ -d "$CMAKE_DIR" ]; then
  echo "CMake directory '$CMAKE_DIR' exists. Skipping creation of directory."
else
  echo "CMake directory '$CMAKE_DIR' does not exist. Creating directory."
  mkdir "$CMAKE_DIR"
fi

cd "$CMAKE_DIR"
cmake .. -DCMAKE_BUILD_TYPE=Debug -DENABLE_WEB_SERVER_SCRIPTS=ON
make

# Python
cd ../py
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt

# Systemd Service
cd ../scripts

SERVICE_FILE="startup.service"
sudo cp "$SERVICE_FILE" /etc/systemd/system/"$SERVICE_FILE"
sudo systemctl daemon-reload
sudo systemctl enable "$SERVICE_FILE"

# Serial
FW_CONFIG_PATH="/boot/firmware/config.txt"
if ! grep -q "dtparam=uart0=on" "$FW_CONFIG_PATH"; then
    echo "Enabling UART in $FW_CONFIG_PATH."
    sudo sh -c "echo 'dtparam=uart0=on' >> $FW_CONFIG_PATH"
else
    echo "UART is already enabled in $FW_CONFIG_PATH."
fi
~
