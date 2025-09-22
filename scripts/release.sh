#!/bin/bash
# Release Helper Script for ksIotFrameworkLib
# Helps prepare releases locally and trigger GitHub automation

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
ROOT_DIR="$(cd "$SCRIPT_DIR/.." && pwd)"

print_usage() {
    echo "Usage: $0 [COMMAND] [OPTIONS]"
    echo ""
    echo "Commands:"
    echo "  show              Show current version"
    echo "  bump              Bump version locally (doesn't create release)"
    echo "  release           Prepare and create release (local + GitHub)"
    echo "  help              Show this help"
    echo ""
    echo "Options for bump/release:"
    echo "  --type TYPE       Version increment type: patch, minor, major (default: patch)"
    echo "  --suffix SUFFIX   Version suffix: a, b, c, etc."
    echo "  --version VERSION Set specific version"
    echo "  --prerelease      Mark as prerelease (release command only)"
    echo "  --draft          Create as draft (release command only)"
    echo "  --dry-run        Show what would be done without making changes"
    echo ""
    echo "Examples:"
    echo "  $0 show                           # Show current version"
    echo "  $0 bump --type patch              # Bump patch version (1.0.21b -> 1.0.21c or 1.0.22)"
    echo "  $0 bump --type minor              # Bump minor version (1.0.21b -> 1.1.0)"
    echo "  $0 bump --suffix a                # Add suffix (1.0.21 -> 1.0.21a)"
    echo "  $0 release --type patch           # Create patch release"
    echo "  $0 release --version 2.0.0        # Create specific version release"
}

print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

check_git_status() {
    if ! git diff-index --quiet HEAD --; then
        print_error "Working directory is not clean. Please commit or stash changes first."
        exit 1
    fi
}

check_github_cli() {
    if ! command -v gh &> /dev/null; then
        print_warning "GitHub CLI (gh) not found. Release will only be created locally."
        return 1
    fi
    return 0
}

get_current_version() {
    cd "$ROOT_DIR"
    python3 scripts/version_manager.py --show | grep "Current version:" | cut -d' ' -f3
}

bump_version() {
    local args=()
    local dry_run=false
    
    while [[ $# -gt 0 ]]; do
        case $1 in
            --type)
                args+=(--type "$2")
                shift 2
                ;;
            --suffix)
                args+=(--suffix "$2")
                shift 2
                ;;
            --version)
                args+=(--version "$2")
                shift 2
                ;;
            --dry-run)
                dry_run=true
                shift
                ;;
            *)
                print_error "Unknown option: $1"
                exit 1
                ;;
        esac
    done
    
    cd "$ROOT_DIR"
    
    if [ "$dry_run" = true ]; then
        print_info "DRY RUN: Would execute: python3 scripts/version_manager.py ${args[*]}"
        return 0
    fi
    
    print_info "Bumping version..."
    python3 scripts/version_manager.py "${args[@]}"
}

create_release() {
    local type="patch"
    local suffix=""
    local version=""
    local prerelease=false
    local draft=false
    local dry_run=false
    
    while [[ $# -gt 0 ]]; do
        case $1 in
            --type)
                type="$2"
                shift 2
                ;;
            --suffix)
                suffix="$2"
                shift 2
                ;;
            --version)
                version="$2"
                shift 2
                ;;
            --prerelease)
                prerelease=true
                shift
                ;;
            --draft)
                draft=true
                shift
                ;;
            --dry-run)
                dry_run=true
                shift
                ;;
            *)
                print_error "Unknown option: $1"
                exit 1
                ;;
        esac
    done
    
    if [ "$dry_run" = true ]; then
        print_info "DRY RUN: Would create release with:"
        print_info "  Type: $type"
        [ -n "$suffix" ] && print_info "  Suffix: $suffix"
        [ -n "$version" ] && print_info "  Version: $version"
        print_info "  Prerelease: $prerelease"
        print_info "  Draft: $draft"
        return 0
    fi
    
    check_git_status
    
    local current_version
    current_version=$(get_current_version)
    print_info "Current version: $current_version"
    
    # Check if we can use GitHub CLI for automation
    if check_github_cli; then
        print_info "Using GitHub CLI to trigger automated release workflow..."
        
        local gh_args=()
        gh_args+=(-f "version_type=$type")
        [ -n "$suffix" ] && gh_args+=(-f "version_suffix=$suffix")
        [ -n "$version" ] && gh_args+=(-f "target_version=$version")
        [ "$prerelease" = true ] && gh_args+=(-f "prerelease=true")
        [ "$draft" = true ] && gh_args+=(-f "draft=true")
        
        print_info "Triggering GitHub Actions workflow..."
        gh workflow run release.yml "${gh_args[@]}"
        print_success "Release workflow triggered! Check GitHub Actions for progress."
        
    else
        print_warning "GitHub CLI not available. Creating local release only..."
        
        # Bump version locally
        local bump_args=()
        bump_args+=(--type "$type")
        [ -n "$suffix" ] && bump_args+=(--suffix "$suffix")
        [ -n "$version" ] && bump_args+=(--version "$version")
        
        bump_version "${bump_args[@]}"
        
        local new_version
        new_version=$(get_current_version)
        
        print_info "Creating git tag: $new_version"
        git add library.json library.properties
        git commit -m "Release version $new_version"
        git tag "$new_version"
        
        print_success "Local release prepared. Push with: git push origin HEAD && git push origin $new_version"
        print_warning "You'll need to create the GitHub release manually or install GitHub CLI"
    fi
}

# Main script logic
case "${1:-help}" in
    show)
        current=$(get_current_version)
        print_info "Current version: $current"
        ;;
    bump)
        shift
        bump_version "$@"
        ;;
    release)
        shift
        create_release "$@"
        ;;
    help|--help|-h)
        print_usage
        ;;
    *)
        print_error "Unknown command: $1"
        print_usage
        exit 1
        ;;
esac