#!/bin/bash

# 参数：命令和重复次数
COMMAND=$1
REPEAT=$2

# 检查是否提供了足够的参数
if [ -z "$COMMAND" ] || [ -z "$REPEAT" ]; then
  echo "Usage: $0 <command> <repeat_count>"
  exit 1
fi

# 重复执行命令
for ((i=1; i<=REPEAT; i++)); do
  echo "Run #$i: Starting..."
  eval $COMMAND
  echo "Run #$i: Finished."
done
