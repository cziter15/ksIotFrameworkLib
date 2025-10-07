# GitHub Copilot Instructions for ksIotFrameworkLib

## Commit Message Format

When creating commits for this repository, follow these guidelines:

### Commit Name Template

Use the following format for commit messages:

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
(fix) Spelling errors and comment inconsistencies throughout the repository #49
(fix) Two bugs in ksDevicePortal.cpp #48
(feat) Add WiFi reconnection logic to network component
(docs) Update README with ESP32 platform requirements
(chore) Update packages in device portal
(perf) Optimize memory usage in MQTT component
(refactor) Simplify authentication logic in device portal
```

**Avoid:**
```
Initial plan
Update files
Fixed stuff
WIP
```

### Important Notes

1. **Never use "Initial plan" as a commit message** - This should be avoided completely. Use descriptive commit messages that explain what was actually done.

2. **Be specific** - Describe what was changed, not just that something was changed.

3. **Keep it concise** - The description should be brief but informative.

4. **Reference issues** - When working on a specific issue, include the issue number at the end.

5. **Use lowercase** - Start the description with a lowercase letter unless it's a proper noun.

### Release Workflow Integration

The repository uses an automated release workflow that:

- Parses commit messages to generate release notes
- Groups commits by type (Features, Fixes, Documentation, etc.)
- Supports both `(type)` and `type:` or `type(scope):` formats
- Creates semantic version tags (e.g., `1.0.33`)

Your commit messages will appear in release notes, so write them with end users in mind.
