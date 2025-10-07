#!/bin/bash
#
# Script to install git hooks for the ksIotFrameworkLib repository.
# This script sets up the commit-msg hook to validate commit messages.
#
# Copyright (c) 2020-2026, Krzysztof Strehlau
# This file is a part of the ksIotFrameworkLib IoT library.
# https://github.com/cziter15/ksIotFrameworkLib

# Color codes for output
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
RED='\033[0;31m'
NC='\033[0m' # No Color

echo "=========================================="
echo "Installing Git Hooks"
echo "=========================================="
echo ""

# Get the git directory
GIT_DIR=$(git rev-parse --git-dir 2>/dev/null)

if [ -z "$GIT_DIR" ]; then
    echo -e "${RED}Error:${NC} Not in a git repository"
    exit 1
fi

# Get the repository root
REPO_ROOT=$(git rev-parse --show-toplevel)

# Define paths
HOOKS_SOURCE_DIR="$REPO_ROOT/.githooks"
HOOKS_TARGET_DIR="$GIT_DIR/hooks"

# Check if source hooks directory exists
if [ ! -d "$HOOKS_SOURCE_DIR" ]; then
    echo -e "${RED}Error:${NC} Hooks source directory not found: $HOOKS_SOURCE_DIR"
    exit 1
fi

# Install commit-msg hook
HOOK_NAME="commit-msg"
SOURCE_HOOK="$HOOKS_SOURCE_DIR/$HOOK_NAME"
TARGET_HOOK="$HOOKS_TARGET_DIR/$HOOK_NAME"

if [ ! -f "$SOURCE_HOOK" ]; then
    echo -e "${YELLOW}Warning:${NC} Source hook not found: $SOURCE_HOOK"
else
    # Create backup if hook already exists
    if [ -f "$TARGET_HOOK" ]; then
        echo -e "${YELLOW}Info:${NC} Backing up existing $HOOK_NAME hook to ${HOOK_NAME}.backup"
        cp "$TARGET_HOOK" "${TARGET_HOOK}.backup"
    fi
    
    # Copy and make executable
    cp "$SOURCE_HOOK" "$TARGET_HOOK"
    chmod +x "$TARGET_HOOK"
    echo -e "${GREEN}✓${NC} Installed $HOOK_NAME hook"
fi

echo ""
echo "=========================================="
echo -e "${GREEN}Git hooks installed successfully!${NC}"
echo "=========================================="
echo ""
echo "The commit-msg hook will now validate your commit messages."
echo "Ensure your commits follow the format:"
echo "  (type) Brief description of changes #issue_number"
echo ""
echo "Valid types: feat, fix, docs, chore, perf, refactor, test"
echo ""
echo "Examples:"
echo "  (fix) resolve memory leak in MQTT connector #123"
echo "  (feat) add WiFi reconnection logic"
echo "  (docs) update README with installation steps"
echo ""
