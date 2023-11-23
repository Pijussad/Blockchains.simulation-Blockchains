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

class Blockchain;

class Block {
public:
    vector<Transaction> transactions;
    string previous_hash;
    time_t timestamp;
    int nonce;
    string hash;

    // Konstruktorius, kuris sukuria bloką su transakcijomis ir ankstesnio bloko "hash"
    Block(const vector<Transaction>& transactions, const string& previous_hash)
        : transactions(transactions), previous_hash(previous_hash), nonce(0), timestamp(time(nullptr)) {
        hash = calculate_hash();
    }

    // Funkcija, kuri skaičiuoja naują bloko "hash"
    string calculate_hash() const {
        string data;

        for (const auto& transaction : transactions) {
            data += transaction.getTransactionId();
        }

        data += previous_hash + to_string(timestamp) + to_string(nonce);
        return bytes_to_hex(customHash(data));
    }
};

class Blockchain {
private:
    vector<Block> chain;
    vector<Transaction> transaction_pool;

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
    vector<Transaction> create_random_transactions(int num_transactions, const vector<User>& users) {
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

    // Funkcija, kuri iškasuoja bloką su nurodytu sunkumu
    void mine_block(Block& block, int difficulty) const {
        while (stoi(block.hash.substr(0, difficulty), nullptr, 16) != 0) {
            block.nonce++;
            block.hash = block.calculate_hash();
        }
        cout << "Blokas iškastas!\nNonce: " << block.nonce << "\n";
    }

    // Funkcija, kuri prideda bloką prie grandinės
    void add_block_to_chain(const Block& block) {
    chain.push_back(block);

    // Remove transactions that are included in the new block
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


    // Funkcija, kuri vykdo simuliaciją su nurodytomis parametrais
    void run_simulation(int num_users, int num_transactions, int block_size, int difficulty, int max_blocks) {
        srand(static_cast<unsigned int>(time(nullptr)));

        vector<User> users = create_random_users(num_users);
        transaction_pool = create_random_transactions(num_transactions, users);

        int blocks_mined = 0;

        while (!transaction_pool.empty() && blocks_mined < max_blocks) {
            cout << "Transaction pool size: " << transaction_pool.size() << endl;
cout << "Blocks mined: " << blocks_mined << endl;
            vector<Transaction> selected_transactions;

            for (int i = 0; i < block_size && i < transaction_pool.size(); ++i) {
                selected_transactions.push_back(transaction_pool[i]);
            }

            Block new_block(selected_transactions, chain.back().hash);

            mine_block(new_block, difficulty);
            add_block_to_chain(new_block);
            printBlock(new_block.transactions.empty() ? -1 : chain.size() - 2);

            blocks_mined++;
        }

        cout << "Simuliacija baigta po iškasto " << blocks_mined << " bloko(-ų).\n";
        printBlock(2);
    }
};

int main() {
    Blockchain blockchain;
    blockchain.run_simulation(100, 1000, 10, 2, 100000);

    return 0;
}
