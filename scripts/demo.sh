#!/bin/bash
# Demo script showing common release scenarios for ksIotFrameworkLib

set -e

# Colors for output
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo -e "${BLUE}🚀 ksIotFrameworkLib Release Automation Demo${NC}"
echo "=================================================="
echo

# Show current version
echo -e "${GREEN}📋 Current Version:${NC}"
./scripts/release.sh show
echo

# Demo version increment scenarios
echo -e "${GREEN}🔄 Version Increment Scenarios:${NC}"
echo

echo -e "${YELLOW}Scenario 1: Patch increment (1.0.21b → 1.0.21c)${NC}"
echo "Command: ./scripts/release.sh bump --dry-run --type patch"
./scripts/release.sh bump --dry-run --type patch
echo

echo -e "${YELLOW}Scenario 2: Minor increment (1.0.21b → 1.1.0)${NC}"
echo "Command: ./scripts/release.sh bump --dry-run --type minor"
./scripts/release.sh bump --dry-run --type minor
echo

echo -e "${YELLOW}Scenario 3: Major increment (1.0.21b → 2.0.0)${NC}"
echo "Command: ./scripts/release.sh bump --dry-run --type major"
./scripts/release.sh bump --dry-run --type major
echo

echo -e "${YELLOW}Scenario 4: Add alpha suffix (1.0.21 → 1.0.21a)${NC}"
echo "Command: ./scripts/release.sh bump --dry-run --suffix a"
./scripts/release.sh bump --dry-run --suffix a
echo

echo -e "${YELLOW}Scenario 5: Specific version (→ 2.0.0)${NC}"
echo "Command: ./scripts/release.sh bump --dry-run --version 2.0.0"
./scripts/release.sh bump --dry-run --version 2.0.0
echo

# Demo release scenarios
echo -e "${GREEN}🚀 Release Scenarios:${NC}"
echo

echo -e "${YELLOW}Scenario 6: Create patch release${NC}"
echo "Command: ./scripts/release.sh release --dry-run --type patch"
./scripts/release.sh release --dry-run --type patch
echo

echo -e "${YELLOW}Scenario 7: Create prerelease with alpha suffix${NC}"
echo "Command: ./scripts/release.sh release --dry-run --type minor --suffix a --prerelease"
./scripts/release.sh release --dry-run --type minor --suffix a --prerelease
echo

echo -e "${YELLOW}Scenario 8: Create draft release${NC}"
echo "Command: ./scripts/release.sh release --dry-run --version 2.0.0 --draft"
./scripts/release.sh release --dry-run --version 2.0.0 --draft
echo

# Show GitHub Actions info
echo -e "${GREEN}🔧 GitHub Actions Workflow:${NC}"
echo "File: .github/workflows/release.yml"
echo "Trigger: Manual dispatch with options"
echo "Features:"
echo "  - Automatic version increment"
echo "  - Git tag creation"
echo "  - Changelog generation"
echo "  - Build validation"
echo "  - GitHub release creation"
echo

# Show file locations
echo -e "${GREEN}📁 Automation Files:${NC}"
echo "Core Scripts:"
echo "  - scripts/version_manager.py  (Version management)"
echo "  - scripts/release.sh          (User-friendly wrapper)"
echo "  - scripts/test_automation.py  (Test suite)"
echo "Workflows:"
echo "  - .github/workflows/release.yml (GitHub Actions)"
echo "Documentation:"
echo "  - docs/RELEASE_AUTOMATION.md     (Complete guide)"
echo

echo -e "${GREEN}✅ Demo completed! Ready for production use.${NC}"
echo "Next steps:"
echo "  1. Run: ./scripts/release.sh release --type patch"
echo "  2. Or use GitHub Actions: Actions → Release Automation → Run workflow"