# Contributing

This repository enforces a commit message format for all commits, including commits made from VS Code.

Required header pattern (single-line commit message header):

**Format 1 (repository style):**
    
    (type) <short description>

**Format 2 (Conventional Commits style):**
    
    type: <short description>
    type(scope): <short description>

Valid types: feat, fix, docs, style, refactor, perf, test, chore

Examples: 
- `(feat) add OAuth2 token refresh`
- `feat: add OAuth2 token refresh`
- `feat(auth): add OAuth2 token refresh`

Setup (one-time, per clone):

1. Run the helper script to enable repo-managed Git hooks:

   ```bash
   ./scripts/setup-git-hooks.sh
   ```

2. Optionally install a VS Code extension to help craft commit messages (recommended):
   - Conventional Commits support / commit message linter

CI enforcement:
- A GitHub Actions workflow will validate commit messages on pull requests and block merges with invalid commits.

If you cannot run the local hook, CI will still validate commits and will fail the PR if commits are invalid. Please update your commit messages to match the required format.
