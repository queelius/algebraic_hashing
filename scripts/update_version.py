#!/usr/bin/env python3
"""
Version management script for AlgebraicHashing library.

This script updates version numbers across all relevant files to ensure consistency.
It supports semantic versioning and automatically updates:
- CMakeLists.txt
- conanfile.py  
- package.json
- vcpkg.json
- VERSION file

Usage:
    python scripts/update_version.py 2.0.1
    python scripts/update_version.py --major  # 2.0.0 -> 3.0.0
    python scripts/update_version.py --minor  # 2.0.0 -> 2.1.0
    python scripts/update_version.py --patch  # 2.0.0 -> 2.0.1
"""

import argparse
import re
import sys
from pathlib import Path
from typing import Tuple


def parse_version(version_str: str) -> Tuple[int, int, int]:
    """Parse a semantic version string into components."""
    match = re.match(r'^v?(\d+)\.(\d+)\.(\d+)(?:-.*)?$', version_str)
    if not match:
        raise ValueError(f"Invalid version format: {version_str}")
    return tuple(map(int, match.groups()[:3]))


def format_version(major: int, minor: int, patch: int) -> str:
    """Format version components into a semantic version string."""
    return f"{major}.{minor}.{patch}"


def get_current_version(root_dir: Path) -> Tuple[int, int, int]:
    """Get current version from CMakeLists.txt."""
    cmake_file = root_dir / "CMakeLists.txt"
    if not cmake_file.exists():
        raise FileNotFoundError("CMakeLists.txt not found")
    
    content = cmake_file.read_text()
    match = re.search(r'VERSION\s+(\d+\.\d+\.\d+)', content)
    if not match:
        raise ValueError("Version not found in CMakeLists.txt")
    
    return parse_version(match.group(1))


def update_cmake_version(root_dir: Path, version: str) -> None:
    """Update version in CMakeLists.txt."""
    cmake_file = root_dir / "CMakeLists.txt"
    content = cmake_file.read_text()
    
    # Update the VERSION line in project() declaration
    content = re.sub(
        r'(project\([^)]*VERSION\s+)\d+\.\d+\.\d+',
        rf'\g<1>{version}',
        content
    )
    
    cmake_file.write_text(content)
    print(f"✓ Updated CMakeLists.txt to version {version}")


def update_conan_version(root_dir: Path, version: str) -> None:
    """Update version in conanfile.py."""
    conan_file = root_dir / "conanfile.py"
    if not conan_file.exists():
        print("⚠ conanfile.py not found, skipping")
        return
    
    content = conan_file.read_text()
    content = re.sub(
        r'version\s*=\s*["\'][^"\']*["\']',
        f'version = "{version}"',
        content
    )
    
    conan_file.write_text(content)
    print(f"✓ Updated conanfile.py to version {version}")


def update_package_json_version(root_dir: Path, version: str) -> None:
    """Update version in package.json."""
    package_file = root_dir / "package.json"
    if not package_file.exists():
        print("⚠ package.json not found, skipping")
        return
    
    content = package_file.read_text()
    content = re.sub(
        r'"version"\s*:\s*"[^"]*"',
        f'"version": "{version}"',
        content
    )
    
    package_file.write_text(content)
    print(f"✓ Updated package.json to version {version}")


def update_vcpkg_version(root_dir: Path, version: str) -> None:
    """Update version in vcpkg.json."""
    vcpkg_file = root_dir / "vcpkg.json"
    if not vcpkg_file.exists():
        print("⚠ vcpkg.json not found, skipping")
        return
    
    content = vcpkg_file.read_text()
    content = re.sub(
        r'"version"\s*:\s*"[^"]*"',
        f'"version": "{version}"',
        content
    )
    
    vcpkg_file.write_text(content)
    print(f"✓ Updated vcpkg.json to version {version}")


def create_version_file(root_dir: Path, version: str) -> None:
    """Create a VERSION file with the current version."""
    version_file = root_dir / "VERSION"
    version_file.write_text(f"{version}\n")
    print(f"✓ Created VERSION file with version {version}")


def update_changelog(root_dir: Path, version: str) -> None:
    """Update CHANGELOG.md with new version entry."""
    changelog_file = root_dir / "CHANGELOG.md"
    
    if not changelog_file.exists():
        # Create a new changelog
        changelog_content = f"""# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

## [{version}] - {import datetime; print(datetime.date.today().isoformat(), end='')}

### Added
- Initial release of AlgebraicHashing library
- Modern C++20 concepts-based design
- Elegant DSL for hash function composition
- Comprehensive test suite and documentation

### Changed
- N/A (initial release)

### Deprecated
- N/A (initial release)

### Removed
- N/A (initial release)

### Fixed
- N/A (initial release)

### Security
- N/A (initial release)
"""
        changelog_file.write_text(changelog_content)
        print(f"✓ Created CHANGELOG.md with version {version}")
    else:
        print(f"⚠ CHANGELOG.md exists - please update manually for version {version}")


def main():
    """Main version update function."""
    parser = argparse.ArgumentParser(description="Update AlgebraicHashing version numbers")
    
    group = parser.add_mutually_exclusive_group(required=True)
    group.add_argument("version", nargs="?", help="Specific version to set (e.g., 2.1.0)")
    group.add_argument("--major", action="store_true", help="Increment major version")
    group.add_argument("--minor", action="store_true", help="Increment minor version")
    group.add_argument("--patch", action="store_true", help="Increment patch version")
    
    parser.add_argument("--dry-run", action="store_true", help="Show what would be changed without making changes")
    
    args = parser.parse_args()
    
    # Find project root
    root_dir = Path(__file__).parent.parent
    if not (root_dir / "CMakeLists.txt").exists():
        print("Error: Could not find project root (CMakeLists.txt not found)", file=sys.stderr)
        sys.exit(1)
    
    try:
        # Get current version
        current_major, current_minor, current_patch = get_current_version(root_dir)
        current_version = format_version(current_major, current_minor, current_patch)
        
        # Determine new version
        if args.version:
            new_major, new_minor, new_patch = parse_version(args.version)
        elif args.major:
            new_major, new_minor, new_patch = current_major + 1, 0, 0
        elif args.minor:
            new_major, new_minor, new_patch = current_major, current_minor + 1, 0
        elif args.patch:
            new_major, new_minor, new_patch = current_major, current_minor, current_patch + 1
        
        new_version = format_version(new_major, new_minor, new_patch)
        
        print(f"Updating version from {current_version} to {new_version}")
        
        if args.dry_run:
            print("DRY RUN - No files will be modified")
            print(f"Would update:")
            print(f"  - CMakeLists.txt")
            print(f"  - conanfile.py") 
            print(f"  - package.json")
            print(f"  - vcpkg.json")
            print(f"  - VERSION")
            return
        
        # Update all files
        update_cmake_version(root_dir, new_version)
        update_conan_version(root_dir, new_version)
        update_package_json_version(root_dir, new_version)
        update_vcpkg_version(root_dir, new_version)
        create_version_file(root_dir, new_version)
        update_changelog(root_dir, new_version)
        
        print(f"\n🎉 Successfully updated all files to version {new_version}")
        print(f"\nNext steps:")
        print(f"  1. Review the changes: git diff")
        print(f"  2. Update CHANGELOG.md manually if needed")
        print(f"  3. Commit the changes: git add -A && git commit -m 'Bump version to {new_version}'")
        print(f"  4. Tag the release: git tag -a v{new_version} -m 'Release v{new_version}'")
        print(f"  5. Push changes: git push origin --tags")
        
    except (ValueError, FileNotFoundError) as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)


if __name__ == "__main__":
    main()