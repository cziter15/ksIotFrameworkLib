#!/usr/bin/env python3
"""
Version Manager for ksIotFrameworkLib
Automatically increments version numbers in library.json and library.properties files
Copyright (c) 2025, Krzysztof Strehlau
"""

import json
import re
import sys
import argparse
from pathlib import Path

class VersionManager:
    def __init__(self, root_dir=None):
        self.root_dir = Path(root_dir) if root_dir else Path(__file__).parent.parent
        self.library_json_path = self.root_dir / "library.json"
        self.library_properties_path = self.root_dir / "library.properties"
    
    def get_current_version(self):
        """Get current version from library.json"""
        try:
            with open(self.library_json_path, 'r') as f:
                library_json = json.load(f)
                return library_json.get('version')
        except (FileNotFoundError, json.JSONDecodeError, KeyError):
            return None
    
    def parse_version(self, version_str):
        """Parse version string into components"""
        # Match pattern like 1.0.21b or 1.0.21 or 1.0.21a
        match = re.match(r'^(\d+)\.(\d+)\.(\d+)([a-z]?)$', version_str)
        if not match:
            raise ValueError(f"Invalid version format: {version_str}")
        
        major, minor, patch, suffix = match.groups()
        return {
            'major': int(major),
            'minor': int(minor),
            'patch': int(patch),
            'suffix': suffix
        }
    
    def increment_version(self, version_str, increment_type='patch', suffix=None):
        """Increment version based on type"""
        parsed = self.parse_version(version_str)
        
        if increment_type == 'major':
            parsed['major'] += 1
            parsed['minor'] = 0
            parsed['patch'] = 0
            parsed['suffix'] = suffix or ''
        elif increment_type == 'minor':
            parsed['minor'] += 1
            parsed['patch'] = 0
            parsed['suffix'] = suffix or ''
        elif increment_type == 'patch':
            if parsed['suffix']:
                # If we have a suffix, increment it or remove it
                if suffix == '':
                    parsed['suffix'] = ''
                elif suffix:
                    parsed['suffix'] = suffix
                else:
                    # Auto-increment suffix: a->b, b->c, etc., or remove if already advanced
                    if parsed['suffix'] == 'a':
                        parsed['suffix'] = 'b'
                    elif parsed['suffix'] == 'b':
                        parsed['suffix'] = 'c'
                    else:
                        # For anything else, increment patch and remove suffix
                        parsed['patch'] += 1
                        parsed['suffix'] = ''
            else:
                # No suffix, just increment patch
                if suffix:
                    parsed['suffix'] = suffix
                else:
                    parsed['patch'] += 1
        elif increment_type == 'suffix':
            parsed['suffix'] = suffix or ''
        
        # Build new version string
        new_version = f"{parsed['major']}.{parsed['minor']}.{parsed['patch']}{parsed['suffix']}"
        return new_version
    
    def update_library_json(self, new_version):
        """Update version in library.json"""
        with open(self.library_json_path, 'r') as f:
            library_json = json.load(f)
        
        library_json['version'] = new_version
        
        with open(self.library_json_path, 'w') as f:
            json.dump(library_json, f, indent='\t')
            f.write('\n')  # Add trailing newline
    
    def update_library_properties(self, new_version):
        """Update version in library.properties"""
        with open(self.library_properties_path, 'r') as f:
            content = f.read()
        
        # Replace version line
        content = re.sub(r'^version=.*$', f'version={new_version}', content, flags=re.MULTILINE)
        
        with open(self.library_properties_path, 'w') as f:
            f.write(content)
    
    def update_version(self, increment_type='patch', suffix=None, target_version=None):
        """Update version in both files"""
        if target_version:
            new_version = target_version
        else:
            current_version = self.get_current_version()
            if not current_version:
                raise ValueError("Could not read current version from library.json")
            
            new_version = self.increment_version(current_version, increment_type, suffix)
        
        print(f"Updating version to: {new_version}")
        
        self.update_library_json(new_version)
        self.update_library_properties(new_version)
        
        return new_version

def main():
    parser = argparse.ArgumentParser(description='Manage version numbers for ksIotFrameworkLib')
    parser.add_argument('--type', choices=['major', 'minor', 'patch', 'suffix'], 
                       default='patch', help='Type of version increment')
    parser.add_argument('--suffix', help='Add or change version suffix (e.g., a, b, c)')
    parser.add_argument('--version', help='Set specific version directly')
    parser.add_argument('--show', action='store_true', help='Show current version')
    parser.add_argument('--root', help='Root directory of the project')
    
    args = parser.parse_args()
    
    try:
        vm = VersionManager(args.root)
        
        if args.show:
            current = vm.get_current_version()
            print(f"Current version: {current}")
            return
        
        new_version = vm.update_version(
            increment_type=args.type,
            suffix=args.suffix,
            target_version=args.version
        )
        
        print(f"Successfully updated to version: {new_version}")
        
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)

if __name__ == '__main__':
    main()