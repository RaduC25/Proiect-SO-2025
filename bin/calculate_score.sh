#!/bin/bash

if [ $# -ne 1 ]; then
    echo "Usage: $0 <hunt_name>"
    exit 1
fi

if [[ ! "$1" =~ ^[a-zA-Z0-9_]+$ ]]; then
    echo "Invalid argument or hunt name"
    exit 1
fi

file="treasure_hunts/hunt:$1/treasure.txt";
hunt_name="hunt:$1";

if [ -f "$file" ]; then
        echo "User scores for $hunt_name are:"

        awk '
        /^user name:/ { 
            split($0, split_username, ":"); 
            user=split_username[2];
        }
        /^value:/ {
            split($0, split_value, ":");
            value=split_value[2];
            scores[user] += value
        }
        END {
            for (u in scores) {
                print "- " u ": " scores[u]
            }
        }
        ' hunt="$hunt_name" "$file"
        echo ""
else 
        echo "File open error, the file $file does not exist."
        exit 1
fi