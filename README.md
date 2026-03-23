
A demonstration project simulating the lifecycle of a secure embedded device. This project covers bootloader/kernel verification at startup and secure firmware update management (Over-The-Air).

> **⚠️ Disclaimer:** This is a **demonstration project** intended for educational purposes. It uses mock implementations for cryptographic operations and hardware interactions. **Do not use this code in production environments.**

##  Architecture Overview

The project is structured into four main logical components:

| Component | Files | Responsibility |
| :--- | :--- | :--- |
| **Configuration** | `config.h` | Defines constants (key sizes, partitions, logging levels). |
| **Cryptography** | `crypto_utils.*` | Handles hashing (mock SHA-256), signing, and verification. Generates manifests binding firmware versions to digital signatures. |
| **Secure Boot** | `secure_boot.*` | Verifies bootloader and kernel integrity before execution. Maintains an event log of verification attempts. |
| **OTA Manager** | `ota_manager.*` | Manages the update lifecycle: checking versions, downloading binaries, verifying signatures, installing to secondary partitions, and handling rollbacks. |

##  Key Security Concepts Demonstrated

*   **Chain of Trust:** The system verifies the bootloader first, then the kernel, ensuring only trusted code executes.
*   **A/B Partitioning:** Simulates an A/B update scheme (`PARTITION_ACTIVE` vs `PARTITION_INACTIVE`) to prevent bricking the device if an update fails.
*   **Rollback Protection:** Automatically restores the previous working firmware if the new update is corrupted or fails verification.
*   **Manifest Verification:** Firmware is cryptographically signed. The `firmware_manifest_t` structure ensures the version, timestamp, and hash match the signature before installation.

##  Prerequisites

*   A C compiler (GCC or Clang)
*   GNU Make (for Option 1)
*   CMake (optional, for Option 2)

##  Building and Running

### Option 1: Using Make (Recommended)

```bash
# Compile with optimization flags (-O2)
make release

# Run the executable
./build/secure_boot_ota_demoOption 2: Using CMakemkdir build && cd build
cmake ..
cmake --build .
./secure_boot_ota_demoOption 3: Manual Compilationgcc -Wall -Wextra -std=c11 -I include src/*.c -o secure_boot_demo
./secure_boot_demo📝 Expected OutputUpon successful execution, the console output will resemble:============================================================
       SECURE BOOT + OTA UPDATE DEMO (C Implementation)
============================================================
[INFO] Cryptographic subsystem initialized
[INFO] Secure boot initialized
[INFO] OTA manager initialized

[PHASE 1] SECURE BOOT SEQUENCE
----------------------------------------
Bootloader verification: PASS
Kernel verification: PASS

Boot Status: SECURE

[PHASE 2] OTA UPDATE CHECK
----------------------------------------
[INFO] New version available: 1.1.0
[INFO] Size: 1024.00 KB

[PHASE 3] DOWNLOAD & VERIFY
----------------------------------------
[INFO] Firmware downloaded: 1048576 bytes
[INFO] Manifest retrieved: v1.1.0
[INFO] Update verified successfully

[PHASE 4] INSTALL UPDATE
----------------------------------------
[INFO] Update installed, pending reboot
[INFO] Current version: 1.0.0
[INFO] Target version: 1.1.0

============================================================
FINAL STATUS
============================================================
Boot Status: SECURE
OTA State: PENDING_REBOOT
============================================================⚠️ Production Readiness NotesThis code is a simplified simulation. For production use, you must replace the following:
Cryptographic Algorithms: Replace the mock XOR loops with real implementations (e.g., SHA-256, RSA/ECC) using libraries like mbedTLS, wolfSSL, or Hardware Security Modules (HSM/TPM).
Storage: Replace malloc and file I/O with flash memory drivers and static memory pools to avoid fragmentation.
Networking: Replace the mocked download with secure TLS/HTTPS connections featuring certificate pinning.

 License
MIT License - See LICENSE file for details.

---

### 2. `CONTRIBUTING.md`

```markdown
# Contributing to Secure Boot + OTA Demo

Thank you for your interest in contributing to this educational project! Since this is a demonstration of secure embedded concepts, we welcome contributions that improve clarity, add educational value, or fix bugs in the simulation logic.

##  Scope of Contributions

Please focus your efforts on:
*   **Educational Clarity:** Improving comments, documentation, or code structure to better explain the concepts.
*   **Simulation Logic:** Enhancing the mock implementations (e.g., adding more realistic error states, improving the rollback logic).
*   **Build System:** Fixing issues with the Makefile or CMake configuration.
*   **Testing:** Adding unit tests for the crypto or OTA logic (using mock frameworks).

##  What We Are Not Looking For

*   **Production-Ready Crypto:** Do not attempt to integrate real cryptographic libraries (mbedTLS, OpenSSL) directly into the core logic unless it is strictly for educational comparison. The goal is to keep the code lightweight and understandable.
*   **Hardware Abstraction Layers (HAL):** Avoid adding specific hardware drivers (STM32, ESP32, etc.). Keep the code portable and generic.

##  Guidelines

### Code Style
*   Follow the existing C coding style (K&R style braces, 4-space indentation).
*   Use `config.h` for all magic numbers and constants.
*   Ensure all new functions have clear comments explaining their purpose and parameters.

### Pull Request Process
1.  Fork the repository.
2.  Create a feature branch (`git checkout -b feature/amazing-improvement`).
3.  Commit your changes (`git commit -m 'Add amazing improvement'`).
4.  Push to the branch (`git push origin feature/amazing-improvement`).
5.  Open a Pull Request.

### Testing
Before submitting a PR, please ensure:
*   The project compiles cleanly with `make release` (no warnings).
*   The output matches the expected behavior described in `README.md`.
*   Any new features have been tested locally.

## 💬 Questions?

If you have questions about the architecture or the security concepts being demonstrated, feel free to open an Issue or start a Discussion.


3. CHANGELOG.md
# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
*   Initial release of the Secure Boot + OTA Update Demo.
*   Mock implementation of SHA-256 hashing and digital signing.
*   A/B partition simulation logic.
*   Rollback protection mechanism.
*   Makefile and CMakeLists.txt build configurations.

## [1.0.0] - 2026-03-23

### Added
*   **Core Architecture:** Implemented `config.h`, `crypto_utils`, `secure_boot`, and `ota_manager` modules.
*   **Secure Boot Phase:** Added verification logic for bootloader and kernel with event logging.
*   **OTA Manager:** Implemented update checking, mock download, manifest verification, and installation flow.
*   **Safety Features:** Added rollback capability if firmware verification fails.
*   **Documentation:** Created `README.md`, `CONTRIBUTING.md`, and this `CHANGELOG.md`.

### Changed
*   Updated mock crypto functions to use simple XOR loops for educational clarity (not for production).

### Fixed
*   N/A (Initial Release)

### Security Note
*   **Warning:** This version uses mock cryptographic primitives. It is **not secure** for real-world deployment.
