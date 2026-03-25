
# Secure Boot & OTA Update System

A production-ready reference implementation for embedded secure boot and Over-The-Air (OTA) updates.

## Features
- **Secure Boot**: Cryptographic verification of bootloader and kernel using hardware-backed keys (TrustZone/TPM/STM32).
- **Rollback Protection**: Prevents downgrading to vulnerable firmware versions.
- **Secure Keystore**: Hardware-integrated key management with rotation and revocation.
- **OTA Manager**: Signed manifest verification and atomic partition switching.

## Architecture
See `docs/architecture.md` for detailed diagrams.

## Building
```bash
make
