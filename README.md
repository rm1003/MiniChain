# MiniChain - MiniCoin Simplified Blockchain

## project description

This project involves implementing minicoin, a virtual currency based on blockchain technology where a single server maintains the entire chain. the system is implemented as a linked sequence of transaction records for a minicoins account.

## Idea for TO-DO list

### Data Struct (class)

- Block
- Blockchain
- Transaction
- OparationType Enum (DEPOSIT/WITHDRAWAL)

### Genesis Block (Initial Block function idea)

- Define owner data
- Establish initial deposit
- Calculate initial hash
- Set creation timestamp

### Hashing System (function idea)

- Define Algorithm (SHA-256, RSA)
- Implement hash calculation for single block
- Implement Chained hashing (current hash + previous hash)
- Verify chain integrity 

### Transaction Management (function idea)

- Validate balance before withdrawals
- Register deposits
- Block insufficient withdrawals
- Update account balance

### Interity Verification

- Verify sequential hashes
- Validate cumulative balances
- Verify transactions (negative valors)
- Detect fraudulent transactions (?maybe)

### Interface and Simulation

- Interactive menu
- Simulate valid/invalid scenarios
- Activity logging
- Display blockchain state

