#!/bin/bash

cd rpi-backend

# CMake

CMAKE_DIR="cmake-build"
if [ -d "$CMAKE_DIR" ]; then
  echo "CMake directory '$CMAKE_DIR' exists. Replacing it with an empty directory."
  rm -rf "$CMAKE_DIR"
else
  echo "CMake directory '$CMAKE_DIR' does not exist."
fi

echo "Creatking CMake directory '$CMAKE_DIR'."
mkdir "$CMAKE_DIR"

cd "$CMAKE_DIR"
cmake .. -DCMAKE_BUILD_TYPE=Release

# Python
cd ../py
python3 -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt

