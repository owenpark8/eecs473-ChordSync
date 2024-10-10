#!/bin/bash

cd ../src/web-server/public
INPUT_FILE="style.css"
OUTPUT_FILE="compiled.css"

npx tailwindcss -i ${INPUT_FILE} -o ${OUTPUT_FILE}
