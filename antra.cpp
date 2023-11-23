#include <iostream>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm>
#include <iomanip>
#include <sstream>

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
            hash[i] = hash[HASH_SIZE_BYTES - i] * 10 + hash[i];
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

class User {
private:
    string name;
    string public_key;
    int balance;

// Konstruktorius, kuris sukuria vartotoją su unikaliu viešuoju raktu
public:
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

    void setBalance(int newBalance) {
        balance = newBalance;
    }

private:// Funkcija, kuri nustato viešąjį raktą iš baitų masyvo
    void setPublicKey(const vector<char>& bytes) {
        public_key = bytes_to_hex(bytes);
    }
};

class Transaction {
private:
    vector<char> transaction_id;
    string sender;
    string recipient;
    int amount;

public:
    Transaction(const string& sender, const string& recipient, int amount) // Konstruktorius, kuris sukuria unikalų pavedimo identifikatorių
        : sender(sender), recipient(recipient), amount(amount) {
        setTransactionId(customHash(sender + recipient + to_string(amount)));
    }

    const vector<char>& getTransactionId() const {
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

    bool operator==(const Transaction& other) const { // Palyginimo operatorius, kuris lygina pavedimus pagal identifikatorių
        return transaction_id == other.transaction_id &&
               sender == other.sender &&
               recipient == other.recipient &&
               amount == other.amount;
    }

private:
    void setTransactionId(const vector<char>& bytes) { // Funkcija, kuri nustato pavedimo identifikatorių iš baitų masyvo
        transaction_id = bytes;
    }
};

class Blockchain;

class Block {
public:
    vector<Transaction> transactions;
    string previous_hash;
    time_t timestamp;
    int nonce;
    string hash;

    Block(const vector<Transaction>& transactions, const string& previous_hash) // Konstruktorius, kuris sukuria bloką su transakcijomis ir ankstesnio bloko "hash"
        : transactions(transactions), previous_hash(previous_hash), nonce(0), timestamp(time(nullptr)) {
        hash = calculate_hash();
    }

    string calculate_merkle_root() const { // Funkcija, kuri skaičiuoja naują bloko "hash"
    vector<vector<char>> transaction_hashes;

    for (const auto& transaction : transactions) {
        transaction_hashes.push_back(transaction.getTransactionId());
    }

    while (transaction_hashes.size() > 1) {
        vector<vector<char>> new_hashes;

        for (size_t i = 0; i < transaction_hashes.size(); i += 2) {
    vector<char> combined_hash = transaction_hashes[i];

    if (i + 1 < transaction_hashes.size()) {
        combined_hash.insert(combined_hash.end(), transaction_hashes[i + 1].begin(), transaction_hashes[i + 1].end());
    }

    new_hashes.push_back(customHash(bytes_to_hex(combined_hash)));
}

        if (transaction_hashes.size() % 2 == 1) {
            new_hashes.push_back(transaction_hashes.back());
        }

        transaction_hashes = new_hashes;
    }

    return (transaction_hashes.empty() ? "" : bytes_to_hex(transaction_hashes[0]));
}


    string calculate_hash() const {
        string merkle_root = calculate_merkle_root();

        if (merkle_root.empty()) {
            return "";
        }

        string data = merkle_root + previous_hash + to_string(timestamp) + to_string(nonce);
        return bytes_to_hex(customHash(data));
    }
};

class Blockchain {
private:
    vector<Block> chain;
    vector<Transaction> transaction_pool;

public:
    Block create_genesis_block() {  // Funkcija, kuri sukuria pirminį bloką (genesis block)
        return Block(vector<Transaction>(), "0");
    }

    Blockchain() {
        chain.push_back(create_genesis_block());
    }

    vector<User> create_random_users(int num_users) { // Funkcija, kuri sukuria nurodytą kiekį atsitiktinių vartotojų
        vector<User> users;

        for (int i = 0; i < num_users; ++i) {
            users.push_back(User("Vartotojas_" + to_string(i), rand() % 1000000 + 100));
        }

        return users;
    }

    vector<Transaction> create_random_transactions(int num_transactions, const vector<User>& users) {  // Funkcija, kuri sukuria nurodytą kiekį atsitiktinių pavedimų tarp vartotojų
        vector<Transaction> transactions;

        for (int i = 0; i < num_transactions; ++i) {
            string sender = users[rand() % users.size()].getPublicKey();
            string recipient = users[rand() % users.size()].getPublicKey();
            int amount = rand() % 1000 + 1;

            transactions.push_back(Transaction(sender, recipient, amount));
        }

        return transactions;
    }

    const vector<Block>& getChain() const {
        return chain;
    }

    const vector<Transaction>& getTransactionPool() const {
        return transaction_pool;
    }

    void printBlock(int block_index = -1) const { // Funkcija, kuri spausdina bloko informaciją
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

    void print_transaction_details(const Transaction& transaction) const { // Funkcija, kuri spausdina pavedimo informaciją
    cout << "Pavedimo ID: " << bytes_to_hex(transaction.getTransactionId()) << "\n"
          << "Siuntėjas: " << transaction.getSender() << "\n"
          << "Gavėjas: " << transaction.getRecipient() << "\n"
          << "Suma: " << transaction.getAmount() << "\n";
}


    void mine_block(Block& block, int difficulty) const {  // Funkcija, kuri iškasuoja bloką su nurodytu sunkumu
        while (stoi(block.hash.substr(0, difficulty), nullptr, 16) != 0) {
            block.nonce++;
            block.hash = block.calculate_hash();
        }
        cout << "Blokas iškastas!\n" << "Nonce: " << block.nonce << "\n";
        cout << "Procesuojamos transakcijos:\n";
        for (const auto& transaction : block.transactions) {
            print_transaction_details(transaction);
        }
    }

    void add_block_to_chain(const Block& block) { // Funkcija, kuri prideda bloką prie grandinės
        chain.push_back(block);
        transaction_pool.erase(remove_if(transaction_pool.begin(), transaction_pool.end(),
                               [block](const Transaction& transaction) {
                                   return find(block.transactions.begin(), block.transactions.end(), transaction) != block.transactions.end();
                               }),
                               transaction_pool.end());
    }

    bool process_transaction(vector<User>& users, Transaction& transaction) {
    auto sender = find_if(users.begin(), users.end(), [&](const User& user) {
        return user.getPublicKey() == transaction.getSender();
    });

    auto recipient = find_if(users.begin(), users.end(), [&](const User& user) {
        return user.getPublicKey() == transaction.getRecipient();
    });

    if (sender != users.end() && recipient != users.end() && sender->getBalance() >= transaction.getAmount()) {
        if (bytes_to_hex(transaction.getTransactionId()) == bytes_to_hex(customHash(transaction.getSender() + transaction.getRecipient() + to_string(transaction.getAmount())))) {
            sender->setBalance(sender->getBalance() - transaction.getAmount());
            recipient->setBalance(recipient->getBalance() + transaction.getAmount());
            cout << "Transaction processed successfully." << endl;
            return true;
        } else {
            cout << "Transaction failed: Invalid transaction hash." << endl;
        }
    } else {
        cout << "Transaction failed: Insufficient funds." << endl;
    }

    // Remove the processed transaction from the transaction pool
    transaction_pool.erase(remove_if(transaction_pool.begin(), transaction_pool.end(),
                           [&](const Transaction& pool_transaction) {
                               return pool_transaction == transaction;
                           }),
                           transaction_pool.end());

    return false;
}


   // Funkcija, kuri vykdo simuliaciją su nurodytomis parametrais
void run_simulation(int num_users, int num_transactions, int block_size, int difficulty, int max_blocks) {
    srand(static_cast<unsigned int>(time(nullptr)));

    vector<User> users = create_random_users(num_users);
    transaction_pool = create_random_transactions(num_transactions, users);

    int blocks_mined = 0;

    while (!transaction_pool.empty() && blocks_mined < max_blocks) {
        vector<Transaction> selected_transactions;

        for (int i = 0; i < block_size; ++i) {
            selected_transactions.push_back(Transaction(users[rand() % users.size()].getPublicKey(),
                                                        users[rand() % users.size()].getPublicKey(),
                                                        rand() % 1000 + 1));
        }

        Block new_block(selected_transactions, chain.back().hash);

        mine_block(new_block, difficulty);
        add_block_to_chain(new_block);
        printBlock(new_block.transactions.empty() ? -1 : chain.size() - 2);

        for (auto& transaction : new_block.transactions) {
            process_transaction(users, transaction);
        }

        blocks_mined++;
    }

    cout << "Simuliacija baigta po iškasto " << blocks_mined << " bloko(-ų).\n";
    printBlock(2);
}
};

int main() {
    Blockchain blockchain;
    blockchain.run_simulation(100, 1000, 2, 3, 100000);

    return 0;
}
