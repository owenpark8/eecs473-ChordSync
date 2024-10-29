#!/bin/bash

if [ -z "$1" ]; then
    echo "Usage: $0 <directory>"
    exit 1
fi

echo "Going to directory \"$1\"..."
cd "$1" || { echo "Failed to change directory to $1"; exit 1; }

INPUT_FILE="public/style.css"
OUTPUT_FILE="public/compiled.css"

npx tailwindcss -i ${INPUT_FILE} -o ${OUTPUT_FILE}
