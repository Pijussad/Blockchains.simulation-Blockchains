## Paprasto blokų grandinės modelio simuliacija

Pirmoje versijoje dar nėra pilnai veikiančių transakcijų įgyvendinimo.

Šis Python scenarijus simuliuoja pagrindinį blokų grandinės sistemą su naudotojais, sandoriais ir kasimu. Jame naudojama anksčiau parašyta C++ hash funkcijų biblioteka. 

### Priklausomybės

- `ctypes`: Naudojama sąsajai su C++ hash funkcijų biblioteka.
- `random`: Naudota generuojant atsitiktinius dydžius simuliacijos metu.
- `time`: Teikia laiko žymės funkcionalumą blokams.

### Svarbios klasės

- **User**: Atstovauja naudotojui su vardu, viešuoju raktu ir balansu.
- **Transaction**: Atstovauja sandoriui su unikaliu ID, siuntėju, gavėju ir suma.
- **Block**: Atstovauja blokui su sandoriais, ankstesnio bloko hashu, laiko žyma, Nonce ir paties bloko hashu.

### Blokų grandinės veiksmai

- `print_block`: Rodo naujausio arba nurodyto bloko informaciją.
- `mine_block`: Kasuoja bloką su nurodytu sunkumu.
- `process_transactions`: Atnaujina naudotojų balansus pagal sandorius.
- `add_block_to_chain`: Prideda iškastą bloką į blokų grandinę.
- `run_simulation`: Simuliuoja blokų grandiną su atsitiktiniais naudotojais ir sandoriais.

### Kaip paleisti

1. Užtikrinkite, kad `custom_hash.so` biblioteka būtų tame pačiame faile.
2. Paleiskite:

```bash
python3 blokugrandine.py
```

Jūs galite laisvai keisti simuliacijos parametrus `run_simulation`, norėdami matyti skirtingas situacijas.
blockchain.run_simulation(num_users=100, num_transactions=1000, block_size=2, difficulty=1, max_blocks=10000000000)
