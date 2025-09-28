# Package Distribution Guide for AlgebraicHashing

This comprehensive guide covers the entire process of distributing the AlgebraicHashing library through various package managers and distribution channels.

## Overview

AlgebraicHashing is designed as a modern C++20 header-only library with professional package management support. This guide provides step-by-step instructions for:

1. **Conan Center Index** - Primary C++ package manager
2. **vcpkg** - Microsoft's C++ package manager  
3. **GitHub Releases** - Direct distribution
4. **Alternative channels** - build2, Hunter, etc.

## Prerequisites

Before starting the distribution process, ensure:

- ✅ All tests pass on multiple platforms (Linux, Windows, macOS)
- ✅ Code coverage is >95%
- ✅ Documentation is complete and accurate
- ✅ Version numbers are consistent across all files
- ✅ CI/CD pipeline is green
- ✅ Security vulnerabilities are addressed

## 1. Conan Center Index Submission

Conan Center Index is the primary distribution channel for C++ packages.

### 1.1 Preparation

```bash
# Validate local Conan package
conan create . --build missing
cd test_package
conan test . algebraic_hashing/2.0.0@

# Test across different profiles
conan create . --profile=default --build missing
conan create . --profile=gcc11 --build missing  
conan create . --profile=clang13 --build missing
```

### 1.2 Fork and Prepare Repository

```bash
# Fork the Conan Center Index repository
# https://github.com/conan-io/conan-center-index

git clone https://github.com/YOUR_USERNAME/conan-center-index.git
cd conan-center-index

# Create the recipe directory
mkdir -p recipes/algebraic_hashing/all
cd recipes/algebraic_hashing/all
```

### 1.3 Create Recipe Structure

Create the following files in `recipes/algebraic_hashing/all/`:

**conanfile.py**:
```python
# Copy from project root, but adapt for Conan Center requirements
# Remove test dependencies and options not suitable for distribution
```

**conandata.yml**:
```yaml
sources:
  "2.0.0":
    url: "https://github.com/algebraic-hashing/algebraic_hashing/archive/v2.0.0.tar.gz"
    sha256: "SHA256_HASH_HERE"  # Calculate using: sha256sum v2.0.0.tar.gz
    strip_root: true
```

**test_package/conanfile.py**:
```python
# Copy from project test_package/ directory
```

**test_package/CMakeLists.txt** and **test_package/test_basic_usage.cpp**:
Copy from project test_package directory.

### 1.4 Validate Recipe

```bash
# Test the recipe
conan create . algebraic_hashing/2.0.0@

# Run Conan Center validation hooks
pip install conan-center-index-hooks
python -m conans.client.command.hooks.conan_center_index_hooks recipes/algebraic_hashing/all/conanfile.py
```

### 1.5 Submit Pull Request

```bash
git add recipes/algebraic_hashing/
git commit -m "Add algebraic_hashing/2.0.0"
git push origin master

# Create pull request on GitHub
# Title: "Add algebraic_hashing/2.0.0"
# Description: Include library description, features, and validation steps
```

### 1.6 Address Review Feedback

Common feedback areas:
- Recipe validation and compatibility
- License file inclusion
- Test coverage and platforms
- Dependency specifications
- Build system integration

## 2. vcpkg Submission

Microsoft's vcpkg package manager submission process.

### 2.1 Fork vcpkg Repository

```bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
git checkout master
```

### 2.2 Create Port Files

Create directory: `ports/algebraic-hashing/`

**portfile.cmake**:
```cmake
vcpkg_from_github(
    OUT_SOURCE_PATH SOURCE_PATH
    REPO algebraic-hashing/algebraic_hashing
    REF v2.0.0
    SHA512 SHA512_HASH_HERE
    HEAD_REF master
)

vcpkg_cmake_configure(
    SOURCE_PATH "${SOURCE_PATH}"
    OPTIONS
        -DBUILD_TESTING=OFF
        -DBUILD_EXAMPLES=OFF
)

vcpkg_cmake_install()
vcpkg_cmake_config_fixup(CONFIG_PATH lib/cmake/AlgebraicHashing)

file(REMOVE_RECURSE "${CURRENT_PACKAGES_DIR}/debug")

vcpkg_install_copyright(FILE_LIST "${SOURCE_PATH}/LICENSE.md")
```

**vcpkg.json**: Copy from project root

### 2.3 Test Port

```bash
./vcpkg install algebraic-hashing

# Test on different triplets
./vcpkg install algebraic-hashing:x64-windows
./vcpkg install algebraic-hashing:x64-linux
./vcpkg install algebraic-hashing:x64-osx
```

### 2.4 Submit PR

Follow similar process as Conan Center Index.

## 3. GitHub Releases

Automated through GitHub Actions, but manual process:

### 3.1 Prepare Release

```bash
# Update version in all files
python scripts/update_version.py 2.0.1

# Commit changes
git add -A
git commit -m "Bump version to 2.0.1"

# Create and push tag
git tag -a v2.0.1 -m "Release v2.0.1"
git push origin v2.0.1
```

### 3.2 GitHub Actions Automation

The release workflow automatically:
- Validates the release
- Builds artifacts
- Runs comprehensive tests
- Creates GitHub release
- Uploads source archives
- Generates documentation

### 3.3 Manual Release (if needed)

```bash
# Create release archives
mkdir release
cp -r include examples cmake CMakeLists.txt README.md LICENSE.md conanfile.py release/
cd release
zip -r ../algebraic_hashing-2.0.1.zip .
tar -czf ../algebraic_hashing-2.0.1.tar.gz .
```

## 4. Alternative Distribution Channels

### 4.1 build2 Package Manager

Create `.build2/` configuration:

**manifest** (in project root):
```
: 1
name: algebraic_hashing
version: 2.0.0
summary: Modern C++20 library for algebraic hash function composition
license: MIT
url: https://github.com/algebraic-hashing/algebraic_hashing
depends: * build2 >= 0.15.0
depends: * bpkg >= 0.15.0
```

### 4.2 Hunter Package Manager

Submit to HunterGate community packages.

### 4.3 Spack Package Manager

For HPC environments:

```python
# package.py
from spack import *

class AlgebraicHashing(CMakePackage):
    """Modern C++20 library for algebraic hash function composition"""

    homepage = "https://github.com/algebraic-hashing/algebraic_hashing"
    url = "https://github.com/algebraic-hashing/algebraic_hashing/archive/v2.0.0.tar.gz"
    
    maintainers = ['algebraic-hashing']
    
    version('2.0.0', sha256='SHA256_HERE')
    
    depends_on('cmake@3.20:', type='build')
    depends_on('googletest', type='test')
```

## 5. Quality Assurance

### 5.1 Pre-submission Checklist

- [ ] **Code Quality**
  - [ ] All tests pass on CI
  - [ ] Code coverage >95%
  - [ ] Static analysis clean
  - [ ] No security vulnerabilities

- [ ] **Documentation**
  - [ ] API documentation complete
  - [ ] Usage examples provided
  - [ ] README is comprehensive
  - [ ] CHANGELOG is up to date

- [ ] **Package Management**
  - [ ] Version consistency across files
  - [ ] License file present and correct
  - [ ] Dependencies properly specified
  - [ ] Test packages work correctly

- [ ] **Distribution**
  - [ ] Source archives build cleanly
  - [ ] Multiple platform testing
  - [ ] Performance benchmarks
  - [ ] Memory usage validation

### 5.2 Testing Matrix

Test the package across:

**Operating Systems:**
- Ubuntu 20.04, 22.04
- Windows Server 2019, 2022
- macOS 11, 12, 13

**Compilers:**
- GCC 10, 11, 12, 13
- Clang 12, 13, 14, 15
- MSVC 2019, 2022

**Build Types:**
- Release (optimized)
- Debug (with sanitizers)
- RelWithDebInfo

### 5.3 Performance Validation

```bash
# Run comprehensive benchmarks
cmake -B build -DBUILD_BENCHMARKS=ON
cmake --build build --target benchmarks
./build/benchmarks

# Validate performance regression
python scripts/validate_performance.py baseline.json current.json
```

## 6. Maintenance and Updates

### 6.1 Version Management

Use semantic versioning strictly:
- **Major**: Breaking API changes
- **Minor**: New features, backward compatible
- **Patch**: Bug fixes only

### 6.2 Update Process

1. Update version using script: `python scripts/update_version.py --minor`
2. Update CHANGELOG.md with changes
3. Run full test suite
4. Update package repositories
5. Communicate changes to users

### 6.3 Deprecation Process

For breaking changes:
1. Mark old APIs as deprecated (compiler warnings)
2. Provide migration guide
3. Support old APIs for at least one major version
4. Remove in next major version

## 7. Community Engagement

### 7.1 Documentation Website

Deploy documentation using GitHub Pages:
- API reference (Doxygen)
- Tutorials and examples
- Performance benchmarks
- Migration guides

### 7.2 Communication Channels

- **GitHub Issues**: Bug reports and feature requests
- **GitHub Discussions**: Community questions
- **Stack Overflow**: Tag with `algebraic-hashing`
- **Reddit**: r/cpp for major announcements

### 7.3 Conference Presentations

Submit to C++ conferences:
- CppCon
- Meeting C++  
- C++Now
- Regional C++ meetups

## 8. Legal and Licensing

### 8.1 License Compliance

- MIT License allows commercial use
- Include license in all distributions
- Respect third-party licenses (GTest, etc.)

### 8.2 Contributor Agreement

For external contributions:
- Require DCO (Developer Certificate of Origin)
- Clear contribution guidelines
- Code of conduct enforcement

### 8.3 Export Control

Header-only mathematical library:
- No export restrictions for most jurisdictions
- Review local regulations if needed

## 9. Monitoring and Analytics

### 9.1 Package Usage Metrics

Monitor through:
- Conan Center download statistics
- vcpkg usage reports
- GitHub repository insights
- Documentation website analytics

### 9.2 Quality Metrics

Track:
- Bug report frequency
- Test failure rates
- Performance regression alerts
- Security vulnerability reports

### 9.3 Community Health

Monitor:
- Contribution frequency
- Issue response times
- Documentation quality feedback
- User satisfaction surveys

## 10. Success Metrics

Define success criteria:
- [ ] **Adoption**: >1000 downloads/month within 6 months
- [ ] **Quality**: <1% critical bug reports
- [ ] **Performance**: No regressions >5%
- [ ] **Community**: >10 external contributors
- [ ] **Documentation**: >90% user satisfaction
- [ ] **Maintenance**: <24 hour response to critical issues

---

## Quick Start Checklist

For immediate package distribution:

1. **Verify Package Quality** (2-3 days)
   ```bash
   # Run full test suite
   cmake -B build -DBUILD_TESTING=ON && cmake --build build && cd build && ctest
   
   # Validate Conan package
   conan create . --build missing
   ```

2. **Prepare Documentation** (1-2 days)
   ```bash
   # Generate API docs
   cmake --build build --target docs
   
   # Update README and examples
   # Review CHANGELOG.md
   ```

3. **Submit to Conan Center** (1-2 weeks including review)
   - Fork repository
   - Create recipe
   - Submit PR
   - Address feedback

4. **Create GitHub Release** (automated)
   ```bash
   python scripts/update_version.py 2.0.0
   git tag -a v2.0.0 -m "Release v2.0.0"
   git push origin v2.0.0
   ```

5. **Monitor and Maintain** (ongoing)
   - Respond to issues
   - Update dependencies
   - Performance monitoring

This guide provides a comprehensive roadmap for professional package distribution of the AlgebraicHashing library.