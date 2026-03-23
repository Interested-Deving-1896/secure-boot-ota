### **2. docs/incident-response.md**

```markdown
# Incident Response Plan

## Purpose

This document defines procedures for detecting, responding to, and recovering from security incidents affecting our embedded devices.

## Incident Classification

| Severity | Definition | Response Time | Examples |
|----------|------------|---------------|----------|
| **Critical** | Active exploitation, data breach | 24 hours | Remote code execution, key compromise |
| **High** | Significant vulnerability, no active exploit | 72 hours | Authentication bypass, privilege escalation |
| **Medium** | Moderate impact, mitigatable | 7 days | Information disclosure, DoS |
| **Low** | Minimal impact, cosmetic | 30 days | Minor bugs, documentation errors |

## Response Procedures

### Phase 1: Detection & Triage

1. **Receive Alert**
   - Monitor security advisories (CVE databases, vendor notifications)
   - Customer reports via support channels
   - Automated vulnerability scanning

2. **Initial Assessment**
   - Determine affected product versions
   - Assess severity using CVSS scoring
   - Identify potential impact scope

3. **Escalation**
   - Critical/High: Immediate notification to Security Officer
   - Medium/Low: Weekly security review meeting

### Phase 2: Containment

1. **Immediate Actions**
   - Issue security advisory to customers
   - Deploy emergency patches if available
   - Disable vulnerable features if necessary

2. **Communication**
   - Internal: Notify development, support, legal teams
   - External: Coordinate with affected customers
   - Public: Prepare press statement if required

### Phase 3: Eradication

1. **Root Cause Analysis**
   - Review code changes leading to vulnerability
   - Identify process gaps
   - Document findings

2. **Remediation**
   - Develop and test patch
   - Update secure coding guidelines
   - Implement additional security controls

### Phase 4: Recovery

1. **Patch Deployment**
   - Release OTA update with fix
   - Monitor deployment success rate
   - Provide rollback option if needed

2. **Verification**
   - Confirm vulnerability is resolved
   - Conduct regression testing
   - Update threat model

### Phase 5: Lessons Learned

1. **Post-Incident Review**
   - Document timeline and actions taken
   - Identify improvements to response process
   - Update training materials

2. **Continuous Improvement**
   - Update security controls
   - Enhance monitoring capabilities
   - Conduct additional testing

## Contact Escalation Matrix

| Role | Primary | Backup | Contact Method |
|------|---------|--------|----------------|
| Security Officer | security-officer@company.com | backup@company.com | Email + Phone |
| Engineering Lead | eng-lead@company.com | backup@company.com | Email + Slack |
| Legal Counsel | legal@company.com | backup@company.com | Email |
| PR/Communications | pr@company.com | backup@company.com | Email |

## Reporting Timeline

| Event | Deadline | Recipient |
|-------|----------|-----------|
| Incident detected | Immediate | Security Team |
| Initial assessment | 2 hours | Security Officer |
| Customer notification | 72 hours (Critical) | Affected Customers |
| Regulatory notification | 72 hours (if required) | Relevant Authorities |
| Final report | 30 days | Internal Stakeholders |

## Appendix: Vulnerability Disclosure Policy

See `.github/SECURITY.md` for responsible disclosure guidelines.

