#!/bin/bash

echo "[INFO] Updating test scripts to the latest version..."

# 일반적으로 전체 서브모듈 초기화 & 업데이트
git submodule init
git submodule update --remote --merge

if [ $? -ne 0 ]; then
  echo "[ERROR] Failed to update test scripts. Exiting."
  exit 1
fi

export LAB_HOME=$(pwd)
bash test/submit.sh "$1"

