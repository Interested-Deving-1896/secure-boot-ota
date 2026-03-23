
# Compliance Mapping Document

## Overview

This document maps the secure embedded system implementation to relevant regulatory and industry standards. It serves as evidence for security audits and compliance assessments.

| Standard | Status | Last Updated | Owner |
|----------|--------|--------------|-------|
| EU Cyber Resilience Act (CRA) | ✅ Compliant | 2026-03-22 | Security Team |
| ISO/SAE 21434 (Automotive) | ✅ Compliant | 2026-03-22 | Security Team |
| FDA Cybersecurity Guidance | ✅ Compliant | 2026-03-22 | Regulatory Affairs |
| NIST SP 800-53 | ⚠️ Partial | 2026-03-22 | Security Team |
| IEC 62443 (Industrial) | ⚠️ Partial | 2026-03-22 | Security Team |

---

## EU Cyber Resilience Act (CRA)

### Applicability
- **Effective Date**: December 2024 (full enforcement 2027)
- **Scope**: All products with digital elements placed on EU market
- **Classification**: Default conformity assessment (Annex I)

### Requirements Mapping

| CRA Requirement | Implementation | Evidence |
|-----------------|----------------|----------|
| **Security by Design** | Secure boot, encrypted storage, signed updates | `docs/architecture.md`, `firmware/src/boot/` |
| **Vulnerability Handling** | Coordinated disclosure policy, patch timeline | `.github/SECURITY.md`, `CHANGELOG.md` |
| **Security Updates** | OTA update mechanism with rollback | `firmware/src/app/ota_handler.c` |
| **Default Credentials** | Unique device identity, no hardcoded credentials | `docs/security-architecture.md` |
| **Documentation** | Security documentation for users and authorities | This document, `docs/api-reference.md` |
| **Conformity Assessment** | Internal control procedure | `docs/conformity-assessment.md` |

### Declaration of Conformity Statement

> This product has been designed and manufactured in accordance with Regulation (EU) 2024/... (Cyber Resilience Act). The manufacturer declares that the product meets the essential cybersecurity requirements set out in Annex I, including security by design, vulnerability handling, and security update provisions.

---

## ISO/SAE 21434 (Road Vehicles Cybersecurity)

### Applicability
- **Scope**: Automotive electronic/electrical systems
- **Classification**: Cybersecurity engineering throughout vehicle lifecycle

### Requirements Mapping

| ISO 21434 Clause | Implementation | Evidence |
|------------------|----------------|----------|
| **5.4 Cybersecurity Goals** | Threat analysis and risk assessment (TARA) | `docs/threat-model.md` |
| **8.3 Concept Phase** | Security requirements definition | `docs/security-requirements.md` |
| **9.4 Product Development** | Secure coding practices, code review | `.github/workflows/security-scan.yml` |
| **10.5 Validation** | Penetration testing, fuzz testing | `firmware/tests/` |
| **11.6 Post-Market** | Incident monitoring, patch deployment | `docs/incident-response.md` |
| **12.4 Supply Chain** | Supplier cybersecurity requirements | `docs/supply-chain-security.md` |

### TARA Summary

| Asset | Threat | Risk Level | Mitigation |
|-------|--------|------------|------------|
| Firmware Integrity | Unauthorized modification | HIGH | Secure boot with ECDSA verification |
| Cryptographic Keys | Key extraction | HIGH | Hardware secure storage (OTP/eFuse) |
| Communication Channel | Man-in-the-middle | MEDIUM | TLS 1.3 with certificate pinning |
| Debug Interface | Unauthorized access | MEDIUM | JTAG/SWD disabled in production |

---

## FDA Cybersecurity Guidance (2023)

### Applicability
- **Scope**: Medical device software and firmware
- **Classification**: Premarket submission requirements

### Requirements Mapping

| FDA Requirement | Implementation | Evidence |
|-----------------|----------------|----------|
| **SBOM (Software Bill of Materials)** | Complete component inventory | `docs/sbom.json` |
| **Vulnerability Monitoring** | CVE tracking, security advisories | `docs/vulnerability-monitoring.md` |
| **Patch Management** | OTA update capability | `firmware/src/app/ota_handler.c` |
| **Security Testing** | Static analysis, penetration testing | `.github/workflows/security-scan.yml` |
| **Incident Response** | Documented procedures | `docs/incident-response.md` |
| **User Documentation** | Security guidance for healthcare providers | `docs/user-security-guide.md` |

### SBOM Structure

```json
{
  "sbom_version": "1.0",
  "generated_date": "2026-03-22",
  "components": [
    {
      "name": "mbedTLS",
      "version": "3.5.0",
      "license": "Apache-2.0",
      "cpe": "cpe:2.3:a:mbed-tls:mbed-tls:3.5.0:*:*:*:*:*:*:*"
    },
    {
      "name": "CMSIS-NN",
      "version": "2.0.0",
      "license": "Apache-2.0",
      "cpe": "cpe:2.3:a:arm:cmsis-nn:2.0.0:*:*:*:*:*:*:*"
    }
  ],
  "vulnerabilities": []
}
