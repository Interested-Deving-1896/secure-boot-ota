# Threat Model: Secure Boot System

## Assets to Protect

1. Firmware integrity
2. Cryptographic keys
3. Device identity

## Threat Categories (STRIDE)

| Category | Threat | Mitigation |
|----------|--------|------------|
| Spoofing | Attacker flashes malicious firmware | Secure boot with signature verification |
| Tampering | Firmware modified in transit | Digital signatures + encrypted storage |
| Repudiation | Unauthorized updates not logged | Secure audit logging |
| Information Disclosure | Keys extracted from device | Hardware root of trust (OTP/eFuse) |
| Denial of Service | Boot process blocked | Recovery mode with authenticated fallback |
| Elevation of Privilege | Debug interface exploited | JTAG/SWD disabled in production |

## Risk Assessment

| Risk | Severity | Likelihood | Mitigation Status |
|------|----------|------------|-------------------|
| Firmware tampering | Critical | Medium | ✅ Secure boot implemented |
| Key extraction | Critical | Low | ✅ Hardware secure storage |
| Replay attacks | High | Medium | ✅ Nonce-based update protocol |
| Side-channel attacks | Medium | Low | ⚠️ Under evaluation |

