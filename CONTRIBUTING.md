# Contributing to ksIotFrameworkLib

Use clear commit messages following the [Conventional Commits](https://www.conventionalcommits.org/) specification for consistency and automated releases.

## Commit Message Format

Each commit message should follow this structure:

```
<type>[optional scope]: <description>

[optional body]

[optional footer(s)]
```

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

## Tips

- Keep the subject line under 72 characters
- Use lowercase for type and description
- Don't end the subject line with a period
- Use the imperative mood ("add feature" not "added feature")
- Be concise and specific
- Reference issues when possible (e.g., `fixes #123`)
- Use the body to explain *what* and *why*, not *how*

## Before pull requests

- Follow commit format
- Describe changes clearly
- Ensure builds and tests pass
