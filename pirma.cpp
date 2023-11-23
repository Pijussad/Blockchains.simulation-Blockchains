#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <map>

using namespace std;

const int HASH_SIZE_BYTES = 32;

// Hash funkcija
vector<char> customHash(const string& input) {
    vector<char> hash(HASH_SIZE_BYTES, 0);

    for (char symbol : input) {
        for (int i = 0; i < HASH_SIZE_BYTES; i++) {
            hash[i] = (hash[i] + symbol + (i % 13)) ^ ((hash[i] << (i % 8)) | (hash[i] >> (8 - (i % 8))));
            hash[i] = (hash[i] + i) * 37;
            hash[i] = (hash[i] >> 4) | (hash[i] << 3);
            hash[i] = hash[i] & ((i + 1) * 77);
            hash[i] = hash[i] | ((i + 2) * 23);
            hash[i] = (hash[i] + symbol + (i % 7)) ^ ((hash[i] << 5) | (hash[i] >> 3));
            hash[i] = hash[HASH_SIZE_BYTES - 1 - i] * 15 + hash[i];
            hash[i] = hash[HASH_SIZE_BYTES - i - 1] * 10 + hash[i];
        }
    }

    return hash;
}

// Funkcija, kuri konvertuoja baitus į sesioliktainius skaicius
string bytes_to_hex(const vector<char>& bytes) {
    stringstream hex_stream;
    hex_stream << hex << setfill('0');

    for (char byte : bytes) {
        hex_stream << setw(2) << (static_cast<unsigned>(byte) & 0xFF);
    }

    return hex_stream.str();
}

// Deklaracija Transaction klasės
class Transaction;

// Funkcijos deklaracija
vector<char> compute_merkel_root(const vector<Transaction>& transactions);

class User {
private:
    string name;
    string public_key;
    int balance;

public:
    // Konstruktorius, kuris sukuria vartotoją su unikaliu viešuoju raktu
    User(const string& name, int balance) : name(name), balance(balance) {
        setPublicKey(customHash(name));
    }

    string getName() const {
        return name;
    }

    string getPublicKey() const {
        return public_key;
    }

    int getBalance() const {
        return balance;
    }

private:
    // Funkcija, kuri nustato viešąjį raktą iš baitų masyvo
    void setPublicKey(const vector<char>& bytes) {
        public_key = bytes_to_hex(bytes);
    }
};

class Transaction {
private:
    string transaction_id;
    string sender;
    string recipient;
    int amount;

public:
    // Konstruktorius, kuris sukuria unikalų pavedimo identifikatorių
    Transaction(const string& sender, const string& recipient, int amount)
        : sender(sender), recipient(recipient), amount(amount) {
        setTransactionId(customHash(sender + recipient + to_string(amount)));
    }

    string getTransactionId() const {
        return transaction_id;
    }

    string getSender() const {
        return sender;
    }

    string getRecipient() const {
        return recipient;
    }

    int getAmount() const {
        return amount;
    }

    // Palyginimo operatorius, kuris lygina pavedimus pagal identifikatorių
    bool operator==(const Transaction& other) const {
        return transaction_id == other.transaction_id;
    }

private:
    // Funkcija, kuri nustato pavedimo identifikatorių iš baitų masyvo
    void setTransactionId(const vector<char>& bytes) {
        transaction_id = bytes_to_hex(bytes);
    }
};

vector<char> compute_merkel_root(const vector<Transaction>& transactions) {
    if (transactions.empty()) {
        return vector<char>(HASH_SIZE_BYTES, 0);
    }

    vector<string> hashes;
    for (const auto& tx : transactions) {
        hashes.push_back(tx.getTransactionId());
    }

    while (hashes.size() > 1) {
        if (hashes.size() % 2 != 0) {
            hashes.push_back(hashes.back()); // Dubliuoti paskutinį hash, jei nelyginis hashų skaičius
        }

        vector<string> new_hashes;
        for (size_t i = 0; i < hashes.size(); i += 2) {
            string concatenated = hashes[i] + hashes[i + 1];
            new_hashes.push_back(bytes_to_hex(customHash(concatenated)));
        }

        hashes = new_hashes;
    }

    return customHash(hashes.front());
}

class Blockchain;

class Block {
public:
    vector<Transaction> transactions;
    string previous_hash;
    time_t timestamp;
    int nonce;
    string hash;

    // Konstruktorius
    Block(const vector<Transaction>& transactions, const string& previous_hash)
        : transactions(transactions), previous_hash(previous_hash), nonce(0), timestamp(time(nullptr)) {
        hash = calculate_hash();
    }

    // Funkcija skaičiuoti bloko hash
    string calculate_hash() const {
        string data = bytes_to_hex(compute_merkel_root(transactions));
        data += previous_hash + to_string(timestamp) + to_string(nonce);
        return bytes_to_hex(customHash(data));
    }
};

class Blockchain {
private:
    vector<Block> chain;
    vector<Transaction> transaction_pool;
    map<string, int> balances; // Laikyti vartotojų balansus

public:
    // Funkcija, kuri sukuria pirminį bloką (genesis block)
    Block create_genesis_block() {
        return Block(vector<Transaction>(), "0");
    }

    Blockchain() {
        chain.push_back(create_genesis_block());
    }

    // Funkcija, kuri sukuria nurodytą kiekį atsitiktinių vartotojų
    vector<User> create_random_users(int num_users) {
        vector<User> users;

        for (int i = 0; i < num_users; ++i) {
            users.push_back(User("Vartotojas_" + to_string(i), rand() % 1000000 + 100));
        }

        return users;
    }

    // Funkcija, kuri sukuria nurodytą kiekį atsitiktinių pavedimų tarp vartotojų
    bool add_transaction_to_pool(const Transaction& transaction) {
        // Balanso patikrinimas
        if (balances[transaction.getSender()] < transaction.getAmount()) {
            cout << "Pavedimas atmestas: nepakanka lėšų" << endl;
            return false;
        }

        // Pavedimo hash patikrinimas
        vector<char> calculated_hash = customHash(transaction.getSender() + transaction.getRecipient() + to_string(transaction.getAmount()));
        if (bytes_to_hex(calculated_hash) != transaction.getTransactionId()) {
            cout << "Pavedimas atmestas: netinkamas hash" << endl;
            return false;
        }

        // Jei abu patikrinimai praėjo, pridėti pavedimą į pool'ą
        transaction_pool.push_back(transaction);
        cout << "Pavedimas sėkmingai pridėtas į pool'ą!" << endl;
        return true;
    }

    // Pakeisti create_random_transactions funkciją naudojant add_transaction_to_pool
    vector<Transaction> create_random_transactions(int num_transactions, const vector<User>& users) {
        vector<Transaction> transactions;

        for (int i = 0; i < num_transactions; ++i) {
            string sender = users[rand() % users.size()].getPublicKey();
            string recipient = users[rand() % users.size()].getPublicKey();
            int amount = rand() % 1000 + 1;

            Transaction transaction(sender, recipient, amount);
            if (add_transaction_to_pool(transaction)) {
                transactions.push_back(transaction);
                // Atnaujinti balansą
                balances[sender] -= amount;
                balances[recipient] += amount;
            }
        }

        return transactions;
    }

    const vector<Block>& getChain() const {
        return chain;
    }

    const vector<Transaction>& getTransactionPool() const {
        return transaction_pool;
    }

    // Funkcija, kuri spausdina bloko informaciją
    void printBlock(int block_index = -1) const {
        int last_block_index = chain.size() - 1;
        const Block& block = (block_index == -1 || block_index > last_block_index) ? chain[last_block_index] : chain[block_index];

        cout << "Blokas Hash: " << block.hash << "\n"
                  << "Ankstesnio Bloko Hash: " << block.previous_hash << "\n"
                  << "Laiko žymė: " << block.timestamp << "\n"
                  << "Nonce: " << block.nonce << "\n"
                  << "Pavedimai:\n";

        for (const auto& transaction : block.transactions) {
            print_transaction_details(transaction);
        }
    }

    // Funkcija, kuri spausdina pavedimo informaciją
    void print_transaction_details(const Transaction& transaction) const {
        cout << "Pavedimo ID: " << transaction.getTransactionId() << "\n"
              << "Siuntėjas: " << transaction.getSender() << "\n"
              << "Gavėjas: " << transaction.getRecipient() << "\n"
              << "Suma: " << transaction.getAmount() << "\n";
    }
    vector<Block> create_candidate_blocks(int num_candidates, int transactions_per_block) {
    vector<Block> candidates;
    for (int i = 0; i < num_candidates; ++i) {
        vector<Transaction> selected_transactions = select_random_transactions(transactions_per_block);
        candidates.emplace_back(selected_transactions, chain.back().hash);
    }
    return candidates;
}



    // Function to select a random subset of transactions from the pool
    vector<Transaction> select_random_transactions(int count) {
    vector<Transaction> selected;
    if (transaction_pool.size() < count) count = transaction_pool.size();
    
    random_shuffle(transaction_pool.begin(), transaction_pool.end()); // Randomize the pool

    for (int i = 0; i < count; ++i) {
        selected.push_back(transaction_pool[i]);
    }

    return selected;
}



    // Funkcija, kuri iškasuoja bloką su nurodytu sunkumu
    bool try_mine_block(Block& block, int difficulty, int time_limit, int max_attempts) {
        auto start_time = chrono::steady_clock::now();
        int attempts = 0;

        while (chrono::duration_cast<chrono::seconds>(chrono::steady_clock::now() - start_time).count() < time_limit && attempts < max_attempts) {
            block.nonce++;
            block.hash = block.calculate_hash();
            if (stoi(block.hash.substr(0, difficulty), nullptr, 16) == 0) {
                cout << "Block mined successfully!\nNonce: " << block.nonce << "\n";
                return true;
            }
            attempts++;
        }
        return false;
    }

    // Funkcija, kuri prideda bloką prie grandinės
    void add_block_to_chain(const Block& block) {
    chain.push_back(block);

    // Ištrinti pavedimus, kurie įtraukti į naują bloką
    transaction_pool.erase(
        remove_if(transaction_pool.begin(), transaction_pool.end(),
            [&block](const Transaction& transaction) {
                return find_if(block.transactions.begin(), block.transactions.end(),
                               [&transaction](const Transaction& blockTrans) {
                                   return blockTrans.getTransactionId() == transaction.getTransactionId();
                               }) != block.transactions.end();
            }),
        transaction_pool.end());
}


    void initialize_balances(const vector<User>& users) {
        for (const auto& user : users) {
            balances[user.getPublicKey()] = user.getBalance();
        }
    }

    // Pakeisti run_simulation funkciją, kad inicializuotumėte balansus
    void run_simulation(int num_users, int num_transactions, int block_size, int difficulty, int max_blocks){
    srand(static_cast<unsigned int>(time(nullptr)));

    // Create random users and initialize their balances
    vector<User> users = create_random_users(num_users);
    initialize_balances(users);
    
    // Create random transactions and add them to the pool
    create_random_transactions(num_transactions, users);

    int blocks_mined = 0;
    int time_limit = 5; // Initial time limit in seconds
    int max_attempts = 100000; // Initial maximum number of hash attempts

    while (!transaction_pool.empty() && blocks_mined < max_blocks) {
        // Create 5 candidate blocks
        vector<Block> candidates = create_candidate_blocks(5, block_size);
        bool block_mined = false;

        // Attempt to mine each candidate block
        for (auto& candidate : candidates) {
            if (try_mine_block(candidate, difficulty, time_limit, max_attempts)) {
                // If successful, add the block to the chain and break the loop
                add_block_to_chain(candidate);
                printBlock();
                blocks_mined++;
                block_mined = true;
                break;
            }
        }

        if (!block_mined) {
            // If no block was mined successfully, double the time limit and max attempts
            time_limit *= 2;
            max_attempts *= 2;
        }
    }

    cout << "Simulation ended after mining " << blocks_mined << " block(s).\n";
}

};

int main() {
    Blockchain blockchain;
    blockchain.run_simulation(100, 1000, 10, 2, 100000);

    return 0;
}
