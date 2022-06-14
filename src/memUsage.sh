#!/bin/bash
z=$(ps aux)
while read -r z
do
    var=$var$(awk -e '$11 ~ /lab1aotN3246/ {print "memory_usage{process=\""$11"\", pid=\""$2"\"}", $4z}');
done <<< "$z"
if [[ $(echo $var | wc -c) -le 1 ]]
then
    var="memory_usage{process=\"\", pid=\"\"} 0.0"
fi
curl -X POST -H "Content-Type: text/plain" --data  "$var
" http://localhost:9091/metrics/job/top/instance/machine

