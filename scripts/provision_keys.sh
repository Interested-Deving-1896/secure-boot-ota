
#!/bin/bash
echo "Generating Root of Trust keys..."
# In production: Use HSM or secure enclave
python3 ../tools/key_generator.py --type ROOT_OF_TRUST --output keys/root.pem
echo "Keys provisioned successfully."
