
This project is **dual-licensed** under two licenses:

1. **GPL v3 (GNU General Public License v3.0)**:
   - You are free to use, modify, and distribute this software under the terms of the GPL v3 license.
   - See the [LICENSE](./LICENSE) file for the full text of the GPL v3 license.
   - If you modify and distribute this software, you must release your modifications under the same GPL v3 license.

2. **Commercial License**:
   - If you wish to use the software for commercial purposes **without the requirements of the GPL**, you may obtain a commercial license.
   - The commercial license provides more flexibility for use in proprietary applications.
   - Please see the [COMMERCIAL_LICENCE.md](./COMMERCIAL_LICENSE.md) file for details on obtaining a commercial license.

### Which License Should You Choose?

- **Choose GPL v3** if you are working on open-source projects or if you are happy to share your modifications under the same license.
- **Choose the Commercial License** if you need to use the software in a proprietary or closed-source project, or if you want to avoid the copyleft requirements of the GPL.

If you have any questions about which license applies to your use case, or if you would like to discuss commercial licensing options, please contact us at [info@mafeforge.com].



---

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

```

## Testing

```bash

make test

```

## Security Model

Refer to docs/security_model.md.




