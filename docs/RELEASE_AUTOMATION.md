# Release Automation for ksIotFrameworkLib

This repository now includes automated release management that handles version incrementing in both `library.json` and `library.properties` files, as well as GitHub release creation.

## 🚀 Quick Start

### Using GitHub Actions (Recommended)

1. Go to the **Actions** tab in your GitHub repository
2. Select **Release Automation** workflow
3. Click **Run workflow**
4. Choose your options:
   - **Version increment type**: patch, minor, or major
   - **Version suffix**: Optional (a, b, c, etc.)
   - **Target version**: Optional specific version
   - **Prerelease**: Mark as prerelease
   - **Draft**: Create as draft

### Using Command Line Scripts

```bash
# Show current version
./scripts/release.sh show

# Bump version locally (no release)
./scripts/release.sh bump --type patch
./scripts/release.sh bump --suffix a
./scripts/release.sh bump --version 2.0.0

# Create a release (with GitHub CLI)
./scripts/release.sh release --type patch
./scripts/release.sh release --type minor --prerelease
./scripts/release.sh release --version 2.0.0 --draft

# Dry run (see what would happen)
./scripts/release.sh bump --dry-run --type minor
./scripts/release.sh release --dry-run --type patch
```

## 📋 Version Management

### Version Format

The library uses semantic versioning with optional alphabetic suffixes:
- `1.0.21` - Standard semantic version
- `1.0.21a` - Pre-release variant
- `1.0.21b` - Second pre-release variant
- etc.

### Version Increment Rules

1. **Patch increment** (`--type patch`):
   - If current version has suffix: `1.0.21a` → `1.0.21b` → `1.0.21c` → `1.0.22`
   - If no suffix: `1.0.21` → `1.0.22`

2. **Minor increment** (`--type minor`):
   - `1.0.21b` → `1.1.0`
   - Resets patch to 0, removes suffix

3. **Major increment** (`--type major`):
   - `1.0.21b` → `2.0.0`
   - Resets minor and patch to 0, removes suffix

4. **Suffix management** (`--suffix`):
   - Add suffix: `1.0.21` → `1.0.21a`
   - Change suffix: `1.0.21a` → `1.0.21b`
   - Remove suffix: `1.0.21a` → `1.0.21` (use `--suffix ""`)

## 🛠 Script Details

### `scripts/version_manager.py`

Python script that handles version management in both `library.json` and `library.properties`.

**Usage:**
```bash
python3 scripts/version_manager.py [OPTIONS]

Options:
  --show              Show current version
  --type TYPE         Version increment type (patch, minor, major, suffix)
  --suffix SUFFIX     Add/change version suffix
  --version VERSION   Set specific version
  --root PATH         Project root directory
```

**Examples:**
```bash
# Show current version
python3 scripts/version_manager.py --show

# Increment patch version
python3 scripts/version_manager.py --type patch

# Add alpha suffix
python3 scripts/version_manager.py --type suffix --suffix a

# Set specific version
python3 scripts/version_manager.py --version 2.0.0
```

### `scripts/release.sh`

Bash wrapper script that provides a user-friendly interface and can trigger GitHub Actions.

**Features:**
- Color-coded output
- Git status checking
- GitHub CLI integration
- Dry-run capability
- Local and remote release creation

### `.github/workflows/release.yml`

GitHub Actions workflow that automates the entire release process:

1. **Version Management**: Updates `library.json` and `library.properties`
2. **Git Operations**: Commits changes and creates tags
3. **Build Validation**: Builds examples to ensure library works
4. **Release Creation**: Creates GitHub release with changelog
5. **Asset Upload**: Optionally attaches build artifacts

**Workflow Inputs:**
- `version_type`: patch, minor, or major
- `version_suffix`: Optional suffix (a, b, c, etc.)
- `target_version`: Override with specific version
- `prerelease`: Mark as prerelease
- `draft`: Create as draft

## 📝 Examples

### Scenario 1: Regular Patch Release
Current: `1.0.21b` → Target: `1.0.21c`
```bash
./scripts/release.sh release --type patch
```

### Scenario 2: Minor Version with Alpha
Current: `1.0.21b` → Target: `1.1.0a`
```bash
./scripts/release.sh release --type minor --suffix a --prerelease
```

### Scenario 3: Major Version Release
Current: `1.0.21b` → Target: `2.0.0`
```bash
./scripts/release.sh release --type major
```

### Scenario 4: Specific Version
Current: `1.0.21b` → Target: `1.5.0`
```bash
./scripts/release.sh release --version 1.5.0
```

### Scenario 5: Remove Suffix (Stable Release)
Current: `1.0.21b` → Target: `1.0.21`
```bash
./scripts/release.sh release --type suffix --suffix ""
```

## 🔧 Setup Requirements

### For GitHub Actions (Automatic)
- Repository with Actions enabled
- `GITHUB_TOKEN` permissions for releases (automatically available)

### For Local Scripts
- Python 3.6+
- Git
- GitHub CLI (`gh`) - optional but recommended for remote releases

### Installing GitHub CLI
```bash
# macOS
brew install gh

# Ubuntu/Debian
curl -fsSL https://cli.github.com/packages/githubcli-archive-keyring.gpg | sudo dd of=/usr/share/keyrings/githubcli-archive-keyring.gpg
echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/githubcli-archive-keyring.gpg] https://cli.github.com/packages stable main" | sudo tee /etc/apt/sources.list.d/github-cli.list > /dev/null
sudo apt update
sudo apt install gh

# Login
gh auth login
```

## 🚨 Important Notes

1. **Clean Working Directory**: Ensure no uncommitted changes before creating releases
2. **Testing**: The automation includes build validation, but test your changes locally first
3. **Version Consistency**: Both `library.json` and `library.properties` are always updated together
4. **Rollback**: If a release fails, you may need to delete the tag and reset the commit
5. **Permissions**: Ensure your GitHub token has write access to contents and releases

## 📊 Changelog Generation

The automation automatically generates changelogs based on Git commits since the last tag:

```markdown
## Changes in 1.0.22

- Fixed critical bug in WiFi connection (a1b2c3d)
- Updated device portal UI (e4f5g6h)
- Improved error handling (i7j8k9l)

**Full Changelog**: https://github.com/cziter15/ksIotFrameworkLib/compare/1.0.21b...1.0.22
```

## 🔍 Troubleshooting

### Version Script Issues
```bash
# Check Python version
python3 --version

# Test version manager
python3 scripts/version_manager.py --show

# Check file permissions
ls -la scripts/
```

### GitHub Actions Issues
- Check Actions tab for detailed logs
- Verify repository permissions
- Ensure workflow file syntax is correct

### Git Issues
```bash
# Check repository status
git status

# Verify remote configuration
git remote -v

# Check for upstream tracking
git branch -vv
```

## 🎯 Best Practices

1. **Regular Releases**: Use patch increments for bug fixes, minor for features, major for breaking changes
2. **Alpha/Beta Testing**: Use suffixes for pre-releases and testing
3. **Documentation**: Update README and documentation before major releases
4. **Testing**: Always test examples after version bumps
5. **Communication**: Use clear commit messages as they appear in changelogs