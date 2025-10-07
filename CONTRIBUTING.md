# Contributing to ksIotFrameworkLib

Use clear commit messages for consistency and automated releases.

## Format:

(type) short description #issue_number

## Commit Types

| Type       | Description                      | Example                                      |
|-------------|----------------------------------|----------------------------------------------|
| **feat**    | Adds a new feature or capability | `(feat) Add WiFi reconnection logic`         |
| **fix**     | Resolves a bug or issue          | `(fix) Handle MQTT reconnect crash #12`      |
| **docs**    | Updates or improves documentation| `(docs) Update README with setup steps`      |
| **chore**   | Maintenance or build-related task| `(chore) Update dependency versions`         |
| **perf**    | Improves performance or efficiency| `(perf) Reduce memory usage in parser`      |
| **refactor**| Code cleanup or structure change | `(refactor) Simplify config loader`          |
| **test**    | Adds or updates tests            | `(test) Add unit tests for network manager`  |


## Tips:

Be concise and specific
Reference issues when possible

## Git hook (optional):

Run `./.githooks/install-hooks.sh` to enable automatic commit message checks.

## Before pull requests:

- Follow commit format
- Describe changes clearly
- Ensure builds and tests pass

## Thank you for contributing!
