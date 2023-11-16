import ctypes
import random
import time

# c++ hashavimas
custom_hash_lib = ctypes.CDLL('./custom_hash.so')

#hashuojama
def custom_hash(input_text):
    hash_result = (ctypes.c_char * 32)()
    custom_hash_lib.calculateHash(input_text.encode('utf-8'), hash_result)
    return bytes(hash_result).hex()

#Sukuriamos klases naudotojmas, transakcijoms ir blokams
class User:
    def __init__(self, name, balance):
        self.name = name
        self.public_key = custom_hash(name)
        self.balance = balance

class Transaction:
    def __init__(self, sender, recipient, amount):
        self.transaction_id = custom_hash(f"{sender}{recipient}{amount}")
        self.sender = sender
        self.recipient = recipient
        self.amount = amount

class Block:
    def __init__(self, transactions, previous_hash):
        self.transactions = transactions
        self.previous_hash = previous_hash
        self.timestamp = time.time()
        self.nonce = 0
        self.hash = self.calculate_hash()

    def calculate_hash(self):# paduodama informacija reikalinga hashui
        data = f"{self.transactions}{self.previous_hash}{self.timestamp}{self.nonce}"
        return custom_hash(data)

#Klase sudaranti blokus
class Blockchain:
    #pirma karta
    def create_genesis_block(self):
        return Block([], "0")

    def __init__(self):
        self.chain = [self.create_genesis_block()]#chain yra genesis block
        self.transaction_pool = []

        #sukuriami naudotojai ir transakcijos
    def create_random_users(self, num_users): 
        return [User(f"User_{i}", random.randint(100, 1000000)) for i in range(num_users)]

    def create_random_transactions(self, num_transactions, users):
        return [Transaction(random.choice(users).public_key,
                            random.choice(users).public_key,
                            random.randint(1, 1000)) for _ in range(num_transactions)]

    #spausdinimas
    def print_block(self, block_index=None):
        if block_index is not None and not isinstance(block_index, int):
            print("Invalid block index. Printing the most recent block:")
            block_index = None

        if block_index is None:
            block = self.chain[-1]  # Print the most recent block
        else:
            if block_index < 0 or block_index >= len(self.chain):
                print(f"Block index {block_index} is out of bounds.")
                return
            block = self.chain[block_index]

        print(f"Block Hash: {block.hash}\n"
              f"Previous Hash: {block.previous_hash}\n"
              f"Timestamp: {block.timestamp}\n"
              f"Nonce: {block.nonce}\n"
              "Transactions:")
        for transaction in block.transactions:
            self.print_transaction_details(transaction)

    def print_transaction_details(self, transaction):
        print(f"Transaction ID: {transaction.transaction_id}\n"
              f"Sender: {transaction.sender}\n"
              f"Recipient: {transaction.recipient}\n"
              f"Amount: {transaction.amount}\n")

    #bloko apdorojimas
    def mine_block(self, block, difficulty):
        while int(block.hash[:difficulty], 16) != 0:
            block.nonce += 1
            block.hash = block.calculate_hash()
        print("Block mined!\n", block.nonce)

    def process_transactions(self, block):
        for transaction in block.transactions:
            sender = next(user for user in self.users if user.public_key == transaction.sender)
            recipient = next(user for user in self.users if user.public_key == transaction.recipient)

            if sender.balance >= transaction.amount:
                sender.balance -= transaction.amount
                recipient.balance += transaction.amount
                print(f"Transaction {transaction.transaction_id} processed successfully.")
            else:
                print(f"Transaction {transaction.transaction_id} failed: Insufficient funds.")
        print()
    #Bloko pridejimas
    def add_block_to_chain(self, block):
        self.chain.append(block)
        self.transaction_pool = [transaction for transaction in self.transaction_pool if transaction not in block.transactions]






    def run_simulation(self, num_users, num_transactions, block_size, difficulty, max_blocks=5):
        self.users = self.create_random_users(num_users)
        self.transaction_pool = self.create_random_transactions(num_transactions, self.users)

        blocks_mined = 0

        while self.transaction_pool and blocks_mined < max_blocks:
            selected_transactions = random.sample(self.transaction_pool, block_size)
            new_block = Block(selected_transactions, self.chain[-1].hash)

            self.process_transactions(new_block)
            self.mine_block(new_block, difficulty)
            self.add_block_to_chain(new_block)
            self.print_block(new_block)

            blocks_mined += 1

        print(f"Simulation completed after mining {blocks_mined} blocks.")
        blockchain.print_block(2)

if __name__ == "__main__":
    blockchain = Blockchain()
    blockchain.run_simulation(num_users=100, num_transactions=1000, block_size=2, difficulty=1, max_blocks=10000000000)
