#!/bin/bash
# Test script for PR title validation
# This script validates that the PR title validation regex works correctly
# Usage: ./test-pr-title-validation.sh

set -eo pipefail

# Use the same pattern as in validate-commits.yml
PATTERN='^(feat|fix|docs|style|refactor|perf|test|build|ci|chore|revert)(\([a-z0-9_/-]+\))?!?: [a-z].{2,}$'

PASS_COUNT=0
FAIL_COUNT=0
ERROR_COUNT=0

test_title() {
    local title="$1"
    local should_pass="$2"
    local description="$3"
    
    if [[ "$title" =~ $PATTERN ]]; then
        result="PASS"
    else
        result="FAIL"
    fi
    
    if [ "$should_pass" = "$result" ]; then
        PASS_COUNT=$((PASS_COUNT + 1))
        printf "  ✅ %-6s: %s\n" "$result" "$title"
    else
        ERROR_COUNT=$((ERROR_COUNT + 1))
        printf "  🔴 %-6s: %s (expected: %s) - %s\n" "$result" "$title" "$should_pass" "$description"
    fi
}

echo "=========================================="
echo "PR Title Validation Test Suite"
echo "=========================================="
echo ""
echo "Pattern: $PATTERN"
echo ""

echo "Valid PR titles (should PASS):"
test_title "feat: add wifi support" "PASS" "Basic feature"
test_title "fix: resolve connection issue" "PASS" "Basic fix"
test_title "docs: update readme" "PASS" "Basic docs"
test_title "feat(wifi): add reconnection" "PASS" "Feature with scope"
test_title "fix(mqtt): handle crash" "PASS" "Fix with scope"
test_title "feat!: breaking change" "PASS" "Breaking change without scope"
test_title "feat(api)!: remove endpoint" "PASS" "Breaking change with scope"
test_title "fix(wifi/manager): resolve issue" "PASS" "Scope with slash"
test_title "feat(esp32-s3): add support" "PASS" "Scope with hyphen"
test_title "chore: bump version to 1.0.46" "PASS" "Version bump"
test_title "refactor: simplify config loader" "PASS" "Refactoring"
test_title "perf: reduce memory usage" "PASS" "Performance improvement"
test_title "test: add unit tests" "PASS" "Tests"
test_title "build: update dependencies" "PASS" "Build changes"
test_title "ci: update workflow" "PASS" "CI changes"
test_title "style: format code" "PASS" "Style changes"
test_title "revert: previous commit" "PASS" "Revert"

echo ""
echo "Invalid PR titles (should FAIL):"
test_title "Feat: add feature" "FAIL" "Capitalized type"
test_title "FEAT: add feature" "FAIL" "Uppercase type"
test_title "feat :add feature" "FAIL" "Space before colon"
test_title "feat:add feature" "FAIL" "No space after colon"
test_title "feat(scope):add feature" "FAIL" "No space after colon with scope"
test_title "feat: " "FAIL" "Empty description"
test_title "feat:" "FAIL" "No description"
test_title "Add feature" "FAIL" "No type"
test_title "add feature" "FAIL" "Invalid type"
test_title "feat: a" "FAIL" "Single character description"
test_title "feat: ab" "FAIL" "Two character description"
test_title "feat:  double space" "FAIL" "Multiple spaces after colon"
test_title "feat(any scope with spaces): desc" "FAIL" "Scope with spaces"
test_title "feat(CAPS): description" "FAIL" "Capitalized scope"
test_title "feat: Add feature" "FAIL" "Capitalized description"
test_title "feat: ADD FEATURE" "FAIL" "Uppercase description"
test_title "feat(scope!): description" "FAIL" "Exclamation in scope"
test_title "feat(scope@version): desc" "FAIL" "Special char in scope"

echo ""
echo "=========================================="
echo "Test Results"
echo "=========================================="
echo "Correct validations: $PASS_COUNT"
echo "Incorrect validations: $ERROR_COUNT"
echo ""

if [ $ERROR_COUNT -eq 0 ]; then
    echo "✅ All tests passed!"
    exit 0
else
    echo "❌ Some tests failed!"
    exit 1
fi
