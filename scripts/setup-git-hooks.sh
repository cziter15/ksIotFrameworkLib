#!/usr/bin/env bash
set -euo pipefail

# Configure repo-local hooks path
git config core.hooksPath .githooks

# Ensure the commit-msg hook is executable
if [ -f .githooks/commit-msg ]; then
  chmod +x .githooks/commit-msg || true
  echo "Installed .githooks to core.hooksPath and ensured commit-msg is executable."
else
  echo "Warning: .githooks/commit-msg not found."
fi

cat <<'EOF'
Next steps for contributors:
  1) Run: ./scripts/setup-git-hooks.sh
  2) Create commits as usual in VS Code — the commit-msg hook will validate the message.
EOF
