#!/bin/bash

# 첫 번째 인자를 커밋 메시지로 사용, 없으면 기본 메시지
MSG=${1:-"update: sync all project files"}

echo "📦 Adding files..."
git add "$1"  # main.c 만 추가

echo "📝 Committing..."
git commit -m "$MSG"

echo "🚀 Pushing to GitHub..."
git push origin main
