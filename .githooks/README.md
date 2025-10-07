# Git Hooks for ksIotFrameworkLib

This directory contains Git hooks to help maintain consistent commit message formatting in the repository.

## Available Hooks

### commit-msg

Validates commit messages to ensure they follow the repository's commit message format:

```
(type) Brief description of changes #issue_number
```

**Valid commit types:**
- `(feat)` - New features
- `(fix)` - Bug fixes
- `(docs)` - Documentation changes
- `(chore)` - Maintenance tasks (build, configs, dependencies)
- `(perf)` - Performance improvements
- `(refactor)` - Code refactoring
- `(test)` - Test additions or modifications

**Examples:**
```
(fix) resolve memory leak in MQTT connector #123
(feat) add WiFi reconnection logic
(docs) update README with installation steps
(chore) update dependency versions
(perf) optimize memory usage in component
(refactor) simplify authentication logic
(test) add unit tests for config provider
```

## Installation

### Automatic Installation

Run the installation script from the repository root:

```bash
./.githooks/install-hooks.sh
```

This script will:
- Copy the hooks to your `.git/hooks` directory
- Make them executable
- Backup any existing hooks

### Manual Installation

Alternatively, you can manually install the hooks:

```bash
# From the repository root
cp .githooks/commit-msg .git/hooks/commit-msg
chmod +x .git/hooks/commit-msg
```

## Usage

Once installed, the hooks will run automatically:

- **commit-msg**: Runs when you create a commit (via `git commit`)

If a hook detects an issue, it will:
1. Display an error message explaining the problem
2. Show the required format
3. Suggest a corrected version of your message
4. Prevent the commit from being created

You can then fix your commit message and try again.

## Bypassing Hooks

If you need to bypass the hooks (not recommended), you can use:

```bash
git commit --no-verify -m "your message"
```

However, please note that commits that don't follow the format may not be properly categorized in release notes.

## Features

The commit-msg hook provides:

- ✅ Validation of commit message format
- ✅ Validation of commit type (feat, fix, docs, etc.)
- ✅ Smart suggestions based on commit message content
- ✅ Warnings for generic/vague commit messages
- ✅ Automatic skipping for merge and revert commits
- ✅ Colored output for better readability

## Troubleshooting

### Hook not running

Make sure the hook is executable:
```bash
chmod +x .git/hooks/commit-msg
```

### Hook installed but not working

Check that the hook file exists and is in the correct location:
```bash
ls -la .git/hooks/commit-msg
```

### Need to update hooks

Re-run the installation script:
```bash
./.githooks/install-hooks.sh
```

## Contributing

If you have suggestions for improving these hooks, please open an issue or submit a pull request.
