#!/bin/bash

cd ../web-server
INPUT_FILE="public/style.css"
OUTPUT_FILE="public/compiled.css"

npx tailwindcss -i ${INPUT_FILE} -o ${OUTPUT_FILE}
