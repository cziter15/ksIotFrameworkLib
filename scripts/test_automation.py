#!/usr/bin/env python3
"""
Test script for the release automation
Validates that version management works correctly
"""

import tempfile
import shutil
import json
import os
import sys
from pathlib import Path

# Add scripts directory to path
sys.path.append(str(Path(__file__).parent))

# Import our version manager
from version_manager import VersionManager

def test_version_parsing():
    """Test version parsing functionality"""
    print("Testing version parsing...")
    
    vm = VersionManager()
    
    # Test cases
    test_cases = [
        ("1.0.21", {"major": 1, "minor": 0, "patch": 21, "suffix": ""}),
        ("1.0.21a", {"major": 1, "minor": 0, "patch": 21, "suffix": "a"}),
        ("2.5.0b", {"major": 2, "minor": 5, "patch": 0, "suffix": "b"}),
        ("10.15.99z", {"major": 10, "minor": 15, "patch": 99, "suffix": "z"}),
    ]
    
    for version_str, expected in test_cases:
        result = vm.parse_version(version_str)
        assert result == expected, f"Failed for {version_str}: expected {expected}, got {result}"
        print(f"  ✓ {version_str} parsed correctly")

def test_version_increment():
    """Test version increment logic"""
    print("\nTesting version increment...")
    
    vm = VersionManager()
    
    # Test cases: (current, increment_type, suffix, expected)
    test_cases = [
        ("1.0.21b", "patch", None, "1.0.21c"),
        ("1.0.21c", "patch", None, "1.0.22"),
        ("1.0.21", "patch", None, "1.0.22"),
        ("1.0.21", "minor", None, "1.1.0"),
        ("1.0.21b", "minor", None, "1.1.0"),
        ("1.0.21", "major", None, "2.0.0"),
        ("1.0.21b", "major", None, "2.0.0"),
        ("1.0.21", "suffix", "a", "1.0.21a"),
        ("1.0.21a", "suffix", "b", "1.0.21b"),
        ("1.0.21a", "suffix", "", "1.0.21"),
        ("1.0.21", "patch", "a", "1.0.21a"),
        ("1.0.21b", "patch", "", "1.0.21"),
    ]
    
    for current, increment_type, suffix, expected in test_cases:
        result = vm.increment_version(current, increment_type, suffix)
        assert result == expected, f"Failed: {current} --{increment_type}--> expected {expected}, got {result}"
        print(f"  ✓ {current} → {result} ({increment_type})")

def test_file_operations():
    """Test file update operations"""
    print("\nTesting file operations...")
    
    # Create temporary directory
    with tempfile.TemporaryDirectory() as temp_dir:
        temp_path = Path(temp_dir)
        
        # Create test library.json
        library_json = {
            "name": "TestLib",
            "version": "1.0.0",
            "description": "Test library"
        }
        
        json_path = temp_path / "library.json"
        with open(json_path, 'w') as f:
            json.dump(library_json, f, indent='\t')
        
        # Create test library.properties
        properties_content = """name=TestLib
version=1.0.0
author=Test Author
maintainer=Test Maintainer"""
        
        props_path = temp_path / "library.properties"
        with open(props_path, 'w') as f:
            f.write(properties_content)
        
        # Test version manager with temp directory
        vm = VersionManager(temp_dir)
        
        # Test reading current version
        current = vm.get_current_version()
        assert current == "1.0.0", f"Expected 1.0.0, got {current}"
        print(f"  ✓ Read current version: {current}")
        
        # Test updating version
        new_version = vm.update_version("patch")
        assert new_version == "1.0.1", f"Expected 1.0.1, got {new_version}"
        print(f"  ✓ Updated to version: {new_version}")
        
        # Verify JSON file was updated
        with open(json_path, 'r') as f:
            updated_json = json.load(f)
        assert updated_json["version"] == "1.0.1"
        print(f"  ✓ library.json updated correctly")
        
        # Verify properties file was updated
        with open(props_path, 'r') as f:
            updated_props = f.read()
        assert "version=1.0.1" in updated_props
        print(f"  ✓ library.properties updated correctly")

def test_edge_cases():
    """Test edge cases and error conditions"""
    print("\nTesting edge cases...")
    
    vm = VersionManager()
    
    # Test invalid version formats
    invalid_versions = ["1.0", "1.0.0.0", "v1.0.0", "1.0.0-alpha"]
    
    for invalid in invalid_versions:
        try:
            vm.parse_version(invalid)
            assert False, f"Should have failed for invalid version: {invalid}"
        except ValueError:
            print(f"  ✓ Correctly rejected invalid version: {invalid}")

def run_all_tests():
    """Run all tests"""
    print("🧪 Running Release Automation Tests\n")
    
    try:
        test_version_parsing()
        test_version_increment() 
        test_file_operations()
        test_edge_cases()
        
        print("\n✅ All tests passed!")
        return True
        
    except Exception as e:
        print(f"\n❌ Test failed: {e}")
        import traceback
        traceback.print_exc()
        return False

if __name__ == "__main__":
    success = run_all_tests()
    sys.exit(0 if success else 1)