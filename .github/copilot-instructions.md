# GitHub Copilot Instructions for ksIotFrameworkLib

## Commit Message Format

When creating commits for this repository, follow the [Conventional Commits](https://www.conventionalcommits.org/) specification as documented in CONTRIBUTING.md.

Your commit messages MUST follow this format:

```
<type>[optional scope]: <description>

[optional body]

[optional footer(s)]
```

### Valid Types
- **feat**: A new feature
- **fix**: A bug fix
- **docs**: Documentation changes
- **style**: Code style changes (formatting, missing semicolons, etc.)
- **refactor**: Code refactoring without changing functionality
- **perf**: Performance improvements
- **test**: Adding or updating tests
- **build**: Changes to build system or dependencies
- **ci**: Changes to CI configuration
- **chore**: Other maintenance tasks
- **revert**: Reverting a previous commit

### Examples
```
feat: add WiFi reconnection logic
fix(mqtt): handle reconnect crash
docs: update README with setup steps
feat!: remove deprecated API
```

Your commit messages will appear in release notes, so write them clearly and with end users in mind.
