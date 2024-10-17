#!/bin/bash

# Argument parsing
if [ "$#" -ne 2 ]; then
    echo "Usage: $0 <source-directory> <output-file>"
    exit 1
fi

SOURCES_DIR="$1"
OUTPUT_FILE="$2"

generate_content_constexpr() {
    local path=$1
    local lc_name=$2

    local content=$(tr -d '[:space:]' < "$path")
    local content_name=${lc_name}_content

    echo "constexpr char $content_name[] = R\"RAW($content)RAW\";"
}

generate_iter_macro() {
    local lc_name=$1
    local uc_name=$(echo "$lc_name" | tr 'a-z' 'A-Z')

    local content_name=${lc_name}_content

    echo "_F($uc_name, $content_name, __VA_ARGS__)\\"
}

# Clear the output file
> "$OUTPUT_FILE"

{
    echo "#pragma once"
    echo ""
    echo "namespace web {"

    for file_path in "$SOURCES_DIR"/*; do
        if [ -f "$file_path" ]; then
            file_name=$(basename "$file_path")
            # Replace non-alphanumeric characters with underscores to form a valid variable name
            formatted_name=$(echo "$file_name" | sed -e 's/[^a-zA-Z0-9]/_/g')

            generate_content_constexpr "$file_path" "$formatted_name"
        fi
    done

    echo "#define SOURCE_FILES_ITER(_F, ...)\\"

    for file_path in "$SOURCES_DIR"/*; do
        if [ -f "$file_path" ]; then
            file_name=$(basename "$file_path")
            formatted_name=$(echo "$file_name" | sed -e 's/[^a-zA-Z0-9]/_/g')
            generate_iter_macro "$formatted_name"
        fi
    done

    echo ""
    echo "} // namespace web"
} >> "$OUTPUT_FILE"

echo "Web sources generated into $OUTPUT_FILE"
