# Contributing to ksIotFrameworkLib

Thank you for your interest in contributing to ksIotFrameworkLib! This document provides guidelines and information for contributors.

## 📝 Commit Message Format

This repository uses a standardized commit message format to maintain consistency and generate meaningful release notes.

### Format

```
(type) Brief description of changes #issue_number
```

**Components:**

- `(type)` - The type of change, enclosed in parentheses
- Brief description - A concise, descriptive message about what was changed
- `#issue_number` - (Optional) Reference to the related issue number

### Valid Commit Types

The following commit types are recognized by the release workflow:

- `(feat)` - New features
- `(fix)` - Bug fixes
- `(docs)` - Documentation changes
- `(chore)` - Maintenance tasks (build, configs, dependencies)
- `(perf)` - Performance improvements
- `(refactor)` - Code refactoring
- `(test)` - Test additions or modifications

### Examples

**Good commit messages:**

```
(fix) resolve memory leak in MQTT connector #123
(feat) add WiFi reconnection logic
(docs) update README with installation steps
(chore) update dependency versions
(perf) optimize memory usage in component
(refactor) simplify authentication logic
(test) add unit tests for config provider
```

**Avoid:**

```
Initial plan
Update files
Fixed stuff
WIP
```

### Important Notes

1. **Be specific** - Describe what was changed, not just that something was changed.
2. **Keep it concise** - The description should be brief but informative.
3. **Reference issues** - When working on a specific issue, include the issue number at the end.
4. **Use lowercase** - Start the description with a lowercase letter unless it's a proper noun.

## 🪝 Git Hooks

### Commit Message Validation Hook

A Git commit-msg hook is available to automatically validate commit messages against the repository's format.

#### Installation

##### Automatic Installation

Run the installation script from the repository root:

```bash
./.githooks/install-hooks.sh
```

This script will:

- Copy the hooks to your `.git/hooks` directory
- Make them executable
- Backup any existing hooks

##### Manual Installation

Alternatively, you can manually install the hooks:

```bash
# From the repository root
cp .githooks/commit-msg .git/hooks/commit-msg
chmod +x .git/hooks/commit-msg
```

#### Usage

Once installed, the hook will run automatically when you create a commit (via `git commit`).

If the hook detects an issue, it will:

1. Display an error message explaining the problem
2. Show the required format
3. Suggest a corrected version of your message
4. Prevent the commit from being created

You can then fix your commit message and try again.

#### Examples

**Invalid format - hook rejects with suggestion:**

```bash
$ git commit -m "added WiFi feature"
✗ Commit message error: Message does not follow the required format
Suggested format: (feat) added WiFi feature
```

**Invalid type - hook suggests correct type:**

```bash
$ git commit -m "(feature) add WiFi"
✗ Invalid commit type 'feature'
Suggested format: (feat) add WiFi
```

**Valid format - hook accepts:**

```bash
$ git commit -m "(feat) add WiFi reconnection logic"
✓ Commit message format is valid
```

**Generic message - hook warns but accepts:**

```bash
$ git commit -m "(chore) WIP"
⚠ Warning: Commit message is too generic: 'WIP'
✓ Commit message format is valid
```

#### Features

The commit-msg hook provides:

- ✅ Validation of commit message format
- ✅ Validation of commit type (feat, fix, docs, etc.)
- ✅ Smart suggestions based on commit message content
- ✅ Warnings for generic/vague commit messages
- ✅ Automatic skipping for merge and revert commits
- ✅ Colored output for better readability

#### Bypassing Hooks

If you need to bypass the hooks (not recommended), you can use:

```bash
git commit --no-verify -m "your message"
```

However, please note that commits that don't follow the format may not be properly categorized in release notes.

#### Troubleshooting

**Hook not running:**

Make sure the hook is executable:

```bash
chmod +x .git/hooks/commit-msg
```

**Hook installed but not working:**

Check that the hook file exists and is in the correct location:

```bash
ls -la .git/hooks/commit-msg
```

**Need to update hooks:**

Re-run the installation script:

```bash
./.githooks/install-hooks.sh
```

## 🔄 Release Workflow Integration

The repository uses an automated release workflow that:

- Parses commit messages to generate release notes
- Groups commits by type (Features, Fixes, Documentation, etc.)
- Supports both `(type)` and `type:` or `type(scope):` formats
- Creates semantic version tags (e.g., `1.0.33`)

Your commit messages will appear in release notes, so write them with end users in mind.

## 📋 Pull Request Guidelines

When submitting a pull request:

1. Ensure all commits follow the commit message format
2. Include a clear description of the changes
3. Reference any related issues
4. Ensure your code builds without errors
5. Test your changes thoroughly

## 🐛 Reporting Issues

When reporting issues:

1. Use a clear and descriptive title
2. Provide steps to reproduce the issue
3. Include relevant code snippets or error messages
4. Specify your environment (ESP32/ESP8266, Arduino version, etc.)

## 💡 Suggesting Features

Feature suggestions are welcome! Please:

1. Check if the feature has already been requested
2. Provide a clear use case
3. Explain how it would benefit the project
4. Be open to discussion and feedback

## 📚 Documentation

Documentation improvements are always appreciated. Whether it's:

- Fixing typos
- Clarifying instructions
- Adding examples
- Improving code comments

All contributions help make the project better!

## 🤝 Code of Conduct

Be respectful and considerate in all interactions. We're all here to learn and improve the project together.

## ❓ Questions

If you have questions about contributing, feel free to:

- Open an issue for discussion
- Check the [Wiki](https://github.com/cziter15/ksIotFrameworkLib/wiki)
- Review existing issues and pull requests

Thank you for contributing to ksIotFrameworkLib!
