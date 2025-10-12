# Contributing to ksIotFrameworkLib

Use clear PR titles following the [Conventional Commits](https://www.conventionalcommits.org/) specification for consistency and automated releases.

## PR Title Format

When creating a pull request, the PR title should follow this structure:

```
<type>[optional scope]: <description>
```

This format will be used as the commit message when your PR is merged.

### Commit Types

| Type       | Description                      | Example                                      |
|------------|----------------------------------|----------------------------------------------|
| **feat**   | Adds a new feature or capability | `feat: add WiFi reconnection logic`          |
|            |                                  | `feat(mqtt): add auto-reconnect support`     |
| **fix**    | Resolves a bug or issue          | `fix: handle MQTT reconnect crash`           |
|            |                                  | `fix(wifi): resolve connection timeout #12`  |
| **docs**   | Updates or improves documentation| `docs: update README with setup steps`       |
|            |                                  | `docs(api): add missing parameter descriptions` |
| **chore**  | Maintenance or build-related task| `chore: update dependency versions`          |
|            |                                  | `chore(ci): update workflow actions`         |
| **perf**   | Improves performance or efficiency| `perf: reduce memory usage in parser`       |
| **refactor**| Code cleanup or structure change| `refactor: simplify config loader`           |
| **test**   | Adds or updates tests            | `test: add unit tests for network manager`   |
| **build**  | Changes to build system or deps  | `build: add support for ESP32-S3`            |
| **ci**     | Changes to CI configuration      | `ci: add automated release workflow`         |
| **style**  | Code style changes (formatting)  | `style: format code according to guidelines` |
| **revert** | Reverts a previous commit        | `revert: revert "feat: add new feature"`     |

### Breaking Changes

If your commit introduces breaking changes, add `!` after the type/scope:

```
feat!: remove deprecated WiFi API
```

Or add a footer:

```
feat: update WiFi connection API

BREAKING CHANGE: WiFi.connect() now requires explicit credentials parameter
```

## Requirements

The PR title validation enforces these rules:

- **Type**: Must be lowercase (e.g., `feat`, not `Feat`)
- **Scope**: Optional, must be lowercase alphanumeric with hyphens, underscores, or slashes (e.g., `wifi`, `mqtt`, `esp32-s3`, `wifi/manager`)
- **Separator**: Must have exactly ONE space after the colon (`: `)
- **Description**: Must start with a lowercase letter and be at least 3 characters long
- Individual commits within the PR can use any format

## Common Mistakes

Avoid these common errors:

- ❌ `Feat: add feature` - Type must be lowercase
- ❌ `feat: Add feature` - Description must start with lowercase
- ❌ `feat:add feature` - Missing space after colon
- ❌ `feat:  add feature` - Multiple spaces after colon
- ❌ `feat: a` - Description too short (minimum 3 characters)
- ❌ `feat(Bad Scope): add` - Scope must be lowercase
- ❌ `feat(scope with spaces): add` - Scope can't contain spaces
- ✅ `feat: add feature` - Correct format

## Tips

- Keep the PR title under 72 characters
- Don't end the PR title with a period
- Use the imperative mood ("add feature" not "added feature")
- Be concise but meaningful (avoid very short descriptions)
- Reference issues when possible (e.g., `fixes #123`)

## Before pull requests

- Follow PR title format (will be validated automatically)
- Describe changes clearly in PR description
- Ensure builds and tests pass
