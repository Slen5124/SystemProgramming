#!/bin/bash

# 커밋 메시지를 입력받거나 기본 메시지 사용
MSG=${1:-"update: sync all project files"}

echo "📦 Adding files..."
git add .

echo "📝 Committing..."
git commit -m "$MSG"

echo "🚀 Pushing to GitHub..."
git push origin main
