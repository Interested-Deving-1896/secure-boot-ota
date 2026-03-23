#!/usr/bin/env python3
"""
Firmware Signing Key Generation Utility
Generates ECDSA P-256 key pairs for secure boot
"""

import argparse
import hashlib
import os
from cryptography.hazmat.primitives import serialization
from cryptography.hazmat.primitives.asymmetric import ec
from cryptography.hazmat.backends import default_backend


def generate_key_pair(output_path: str) -> None:
    """Generate ECDSA P-256 key pair"""
   
    # Generate private key
    private_key = ec.generate_private_key(
        ec.SECP256R1(),
        default_backend()
    )
   
    # Extract public key
    public_key = private_key.public_key()
   
    # Serialize private key (PEM format, encrypted)
    private_pem = private_key.private_bytes(
        encoding=serialization.Encoding.PEM,
        format=serialization.PrivateFormat.PKCS8,
        encryption_algorithm=serialization.BestAvailableEncryption(
            b'your-secure-passphrase'  # Change in production!
        )
    )
   
    # Serialize public key (raw bytes for embedded storage)
    public_numbers = public_key.public_numbers()
    public_bytes = public_numbers.x.to_bytes(32, 'big') + \
                   public_numbers.y.to_bytes(32, 'big')
   
    # Write files
    with open(output_path, 'wb') as f:
        f.write(private_pem)
   
    # Write public key to separate file for embedding in device
    public_path = output_path.replace('.pem', '_public.bin')
    with open(public_path, 'wb') as f:
        f.write(public_bytes)
   
    print(f"Private key written to: {output_path}")
    print(f"Public key written to: {public_path}")
    print("WARNING: Store private key securely. Never commit to version control!")


def sign_firmware(firmware_path: str, key_path: str, output_path: str) -> None:
    """Sign firmware binary with ECDSA P-256"""
   
    # Load private key
    with open(key_path, 'rb') as f:
        private_key = serialization.load_pem_private_key(
            f.read(),
            password=b'your-secure-passphrase',
            backend=default_backend()
        )
   
    # Read firmware
    with open(firmware_path, 'rb') as f:
        firmware = f.read()
   
    # Compute SHA-256 hash
    digest = hashlib.sha256(firmware).digest()
   
    # Sign hash
    signature = private_key.sign(
        digest,
        ec.ECDSA(hashes.SHA256())
    )
   
    # Write signature (64 bytes for P-256)
    with open(output_path, 'wb') as f:
        f.write(signature)
   
    print(f"Signature written to: {output_path}")
    print(f"Signature length: {len(signature)} bytes")


if __name__ == '__main__':
    parser = argparse.ArgumentParser(description='Firmware signing utility')
    subparsers = parser.add_subparsers(dest='command')
   
    # Generate key command
    gen_parser = subparsers.add_parser('generate', help='Generate key pair')
    gen_parser.add_argument('--output', '-o', default='signing_key.pem',
                           help='Output path for private key')
   
    # Sign command
    sign_parser = subparsers.add_parser('sign', help='Sign firmware')
    sign_parser.add_argument('--firmware', '-f', required=True,
                            help='Firmware binary to sign')
    sign_parser.add_argument('--key', '-k', required=True,
                            help='Private key file')
    sign_parser.add_argument('--output', '-o', required=True,
                            help='Output signature file')
   
    args = parser.parse_args()
   
    if args.command == 'generate':
        generate_key_pair(args.output)
    elif args.command == 'sign':
        sign_firmware(args.firmware, args.key, args.output)
    else:
        parser.print_help()

