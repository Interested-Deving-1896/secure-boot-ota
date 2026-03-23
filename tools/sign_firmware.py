
#!/usr/bin/env python3
"""
OTA Firmware Signing Tool
Signs firmware binaries with ECDSA P-256 for secure boot verification
"""

import argparse
import hashlib
import struct
import sys
from pathlib import Path
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.hazmat.backends import default_backend


class FirmwareSigner:
    """Handles firmware signing for OTA updates"""
   
    SIGNATURE_SIZE = 64  # ECDSA P-256
   
    def __init__(self, private_key_path: str, passphrase: bytes):
        """Load private key for signing"""
        with open(private_key_path, 'rb') as f:
            self.private_key = serialization.load_pem_private_key(
                f.read(),
                password=passphrase,
                backend=default_backend()
            )
   
    def compute_hash(self, firmware_data: bytes) -> bytes:
        """Compute SHA-256 hash of firmware"""
        return hashlib.sha256(firmware_data).digest()
   
    def sign_firmware(self, firmware_path: str, output_path: str) -> bool:
        """Sign firmware binary and create signed package"""
       
        # Read firmware
        firmware_path = Path(firmware_path)
        if not firmware_path.exists():
            print(f"Error: Firmware file not found: {firmware_path}")
            return False
       
        with open(firmware_path, 'rb') as f:
            firmware_data = f.read()
       
        # Compute hash
        firmware_hash = self.compute_hash(firmware_data)
       
        # Sign hash
        signature = self.private_key.sign(
            firmware_hash,
            ec.ECDSA(hashes.SHA256())
        )
       
        if len(signature) != self.SIGNATURE_SIZE:
            print(f"Error: Invalid signature length: {len(signature)}")
            return False
       
        # Create signed package with metadata
        package_path = Path(output_path)
        with open(package_path, 'wb') as f:
            # Header: version (4 bytes) + size (4 bytes) + hash (32 bytes)
            header = struct.pack('<II', 1, len(firmware_data))
            f.write(header)
            f.write(firmware_hash)
            f.write(signature)
            f.write(firmware_data)
       
        print(f"✓ Signed firmware created: {output_path}")
        print(f"  Size: {len(firmware_data)} bytes")
        print(f"  Hash: {firmware_hash.hex()}")
        print(f"  Signature: {signature.hex()}")
       
        return True
   
    def verify_signature(self, firmware_path: str, signature_path: str) -> bool:
        """Verify firmware signature"""
       
        # Read firmware
        with open(firmware_path, 'rb') as f:
            firmware_data = f.read()
       
        # Read signature
        with open(signature_path, 'rb') as f:
            signature = f.read()
       
        # Compute hash
        firmware_hash = self.compute_hash(firmware_data)
       
        # Verify
        try:
            public_key = self.private_key.public_key()
            public_key.verify(signature, firmware_hash, ec.ECDSA(hashes.SHA256()))
            print("✓ Signature verification successful")
            return True
        except Exception as e:
            print(f"✗ Signature verification failed: {e}")
            return False


def main():
    parser = argparse.ArgumentParser(
        description='OTA Firmware Signing Tool',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s sign -k signing_key.pem -f firmware.bin -o firmware_signed.bin
  %(prog)s verify -k signing_key.pem -f firmware.bin -s signature.bin
        """
    )
   
    subparsers = parser.add_subparsers(dest='command', help='Commands')
   
    # Sign command
    sign_parser = subparsers.add_parser('sign', help='Sign firmware binary')
    sign_parser.add_argument('--key', '-k', required=True,
                            help='Private key PEM file')
    sign_parser.add_argument('--firmware', '-f', required=True,
                            help='Firmware binary to sign')
    sign_parser.add_argument('--output', '-o', required=True,
                            help='Output signed firmware file')
    sign_parser.add_argument('--passphrase', '-p', default=b'',
                            help='Key passphrase (default: empty)')
   
    # Verify command
    verify_parser = subparsers.add_parser('verify', help='Verify firmware signature')
    verify_parser.add_argument('--key', '-k', required=True,
                              help='Private key PEM file')
    verify_parser.add_argument('--firmware', '-f', required=True,
                              help='Firmware binary')
    verify_parser.add_argument('--signature', '-s', required=True,
                              help='Signature file')
    verify_parser.add_argument('--passphrase', '-p', default=b'',
                              help='Key passphrase (default: empty)')
   
    args = parser.parse_args()
   
    if args.command is None:
        parser.print_help()
        sys.exit(1)
   
    passphrase = args.passphrase.encode() if isinstance(args.passphrase, str) else args.passphrase
   
    signer = FirmwareSigner(args.key, passphrase)
   
    if args.command == 'sign':
        success = signer.sign_firmware(args.firmware, args.output)
        sys.exit(0 if success else 1)
   
    elif args.command == 'verify':
        success = signer.verify_signature(args.firmware, args.signature)
        sys.exit(0 if success else 1)


if __name__ == '__main__':
    main()

