## Simple Blockchain Model Simulation

In the initial version, the full implementation of transactions is not yet complete.

This C++ code simulates a basic blockchain system with users, transactions, and mining. It uses a previously written C++ library for hash functions.

### Dependencies

- `iostream`: Standard input/output stream.
- `vector`: Dynamic array container.
- `ctime`: Functions to work with time.
- `cstdlib`: C Standard General Utilities Library.
- `algorithm`: Provides a collection of functions for performing operations on ranges of elements.
- `iomanip`: Set width and formatting of output.
- `sstream`: Stream class to operate on strings.

### Important Classes

- **User**: Represents a user with a name, public key, and balance.
- **Transaction**: Represents a transaction with a unique ID, sender, recipient, and amount.
- **Block**: Represents a block with transactions, the hash of the previous block, timestamp, nonce, and the block's hash.

### Blockchain Operations

- `printBlock`: Displays information about the latest or specified block.
- `mine_block`: Mines a block with a specified difficulty.
- `process_transactions`: Updates user balances based on transactions.
- `add_block_to_chain`: Adds a mined block to the blockchain.
- `run_simulation`: Simulates a blockchain with random users and transactions.

### How to Run

1. Ensure that the `custom_hash.so` library is in the same directory.
2. Compile and run:

```bash
g++ -o blockchain blockchain.cpp
./blockchain
```

Feel free to modify simulation parameters in the `run_simulation` function to observe different scenarios.

```cpp
blockchain.run_simulation(100, 1000, 2, 3, 100000);
```

### Code Explanation

The code includes a hash function `customHash` and three main classes: `User`, `Transaction`, and `Block`. The `Blockchain` class manages the blockchain operations, including mining and simulation. The `main` function initializes a blockchain and runs a simulation with specified parameters.

**Note**: Make sure to adapt the compilation and execution steps based on your specific C++ environment.
