# Ledger Plugin Rarible

--- 

This repo contains a ledger plugin for rarible smart contract on ethereum. It was built following
this [guide](https://developers.ledger.com/docs/dapp/nano-plugin/overview/).

## Project Status

This repository is up-to-date with updates and modifications in this [commit](https://github.com/LedgerHQ/app-plugin-boilerplate/tree/3c9f5811dbaa7de4297af857786089f76d474eae) on the [Ledger Plugin Boilerplate](https://github.com/LedgerHQ/app-plugin-boilerplate) repository.

## Device support

We are currently compiling and testing the plugin using zemu for Nano S, Nano S Plus and X devices.

## Smart contract addresses

These are the smart contract addresses linked to the methods we support in the plugin. They can be
found [here](https://docs.rarible.org/reference/contract-addresses/)

| Network | Contract Address                           |
|---------|--------------------------------------------|
| Mainnet | 0x6e42262978de5233c8d5b05b128c121fba110da4 |
| Mainnet |0x9757f2d2b135150bbeb65308d4a91804107cd8d6|
| Mainnet |0xf6793da657495ffeff9ee6350824910abc21356c|
| Mainnet |0xc9154424b823b10579895ccbe442d41b9abd96ed|
| Ropsten |0x33aef288c093bf7b36fbe15c3190e616a993b0ad|
| Ropsten |0x939d0308ce4274c287e7305d381b336b77dbfcd3|
| Ropsten |0xb0ea149212eb707a1e5fc1d2d3fd318a8d94cf05|

## Methods

| Method                | Supported          | Selector   | Fields Displayed                                  |
|-----------------------|--------------------|------------|---------------------------------------------------|
| Cancel                | :heavy_check_mark: | 0xe2864fe3 | maker, taker                                      | 
| Create Token          | :heavy_check_mark: | 0x27050d1f | operator, name, symbol                            |
| ERC721 Rarible Init   | :heavy_check_mark: | 0x3db397c6 | transfer_proxy, lazy_transfer_proxy, name, symbol |
| Match Orders          | :heavy_check_mark: | 0xe99a3f80 | left maker, left taker, right  maker, right taker |
| Mint And Transfer     | :heavy_check_mark: | 0x22a775b6 | id, beneficiary, creator, royalties               |
| Set Approval For All  | :heavy_check_mark: | 0xa22cb465 | to, is_approved                                   |
| Transfer From Or Mint | :heavy_check_mark: | 0x832fbb29 | id, beneficiary, seller, creator, royalties       |

## Development
### Build apps and plugins
In order to compile the ethereum app and rarible plugin, you need to use Docker to spin up a container. The script located in `builder` folder facilitates that task. I will leave you inside the container terminal, ready to compile.
```shell
cd builder
sh ./start.sh
```

In order to compile the binaries, just do the following steps

```shell
cd tests
./build_local_test_elfs.sh
```

Now you can quit the container terminal. Everything is ready. You can do that by simply run
```shell
exit
```

The final binaries will be located in the folder `tests/elfs`. You will find binaries for the three of them.

### Run test cases
Just build the plugins first. In order to run tests, you will need docker running on your machine again. Just simply run the following commands

```shell
cd tests
yarn test
```

## Tests

We are currently testing all supported methods with a matrix of device type and contract abis from mainnet and ropsten
networks. The following table will be executed for each method we support.

| Device      | Network |
|-------------|---------|
| Nano X      | Mainnet |
| Nano X      | Ropsten |
| Nano S Plus | Mainnet |
| Nano S Plus | Ropsten |
| Nano S      | Mainnet |
| Nano S      | Ropsten |

