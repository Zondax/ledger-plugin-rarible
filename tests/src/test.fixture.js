import Zemu from '@zondax/zemu';
import Eth from '@ledgerhq/hw-app-eth';
import {generate_plugin_config} from './generate_plugin_config';
import {parseEther, parseUnits, RLP} from "ethers/lib/utils";
import {getPluginConfigBasePath} from "./utils";

const transactionUploadDelay = 60000;

async function waitForAppScreen(sim) {
    await sim.waitUntilScreenIsNot(sim.getMainMenuSnapshot(), transactionUploadDelay);
}

const sim_options_generic = {
    logging: true,
    startDelay: 5000,
    custom: '',
    startText: 'ready'
};

const Resolve = require('path').resolve;

const NANOS_ETH_PATH = Resolve('elfs/ethereum_nanos.elf');
const NANOSP_ETH_PATH = Resolve('elfs/ethereum_nanosp.elf');
const NANOX_ETH_PATH = Resolve('elfs/ethereum_nanox.elf');

const NANOS_PLUGIN_PATH = Resolve('elfs/plugin_nanos.elf');
const NANOSP_PLUGIN_PATH = Resolve('elfs/plugin_nanosp.elf');
const NANOX_PLUGIN_PATH = Resolve('elfs/plugin_nanox.elf');

const NANOS_PLUGIN = {"Rarible": NANOS_PLUGIN_PATH};
const NANOSP_PLUGIN = {"Rarible": NANOSP_PLUGIN_PATH};
const NANOX_PLUGIN = {"Rarible": NANOX_PLUGIN_PATH};

const SPECULOS_ADDRESS = '0xFE984369CE3919AA7BB4F431082D027B4F8ED70C';
const RANDOM_ADDRESS = '0xaaaabbbbccccddddeeeeffffgggghhhhiiiijjjj'

const mainnetName = "mainnet";
const ropstenName = "ropsten";
const ethereumBlockchain = "ethereum";

const devices = [
    "nanos",
    "nanosp",
    "nanox"
]

const networks = {
    exchangeNetworks: [
        {
            name: mainnetName,
            blockchain: ethereumBlockchain,
            contractAddr: "0x9757f2d2b135150bbeb65308d4a91804107cd8d6",
            abi: require(`../${getPluginConfigBasePath(mainnetName, ethereumBlockchain)}/abis/0x9757f2d2b135150bbeb65308d4a91804107cd8d6.json`)
        },
        {
            name: ropstenName,
            blockchain: ethereumBlockchain,
            contractAddr: "0x33aef288c093bf7b36fbe15c3190e616a993b0ad",
            abi: require(`../${getPluginConfigBasePath(ropstenName, ethereumBlockchain)}/abis/0x33aef288c093bf7b36fbe15c3190e616a993b0ad.json`)
        }],
    assetNetworks: [
        {
            name: mainnetName,
            blockchain: ethereumBlockchain,
            contractAddr: "0xf6793da657495ffeff9ee6350824910abc21356c",
            abi: require(`../${getPluginConfigBasePath(mainnetName, ethereumBlockchain)}/abis/0xf6793da657495ffeff9ee6350824910abc21356c.json`)
        },
        {
            name: ropstenName,
            blockchain: ethereumBlockchain,
            contractAddr: "0xb0ea149212eb707a1e5fc1d2d3fd318a8d94cf05",
            abi: require(`../${getPluginConfigBasePath(ropstenName, ethereumBlockchain)}/abis/0xb0ea149212eb707a1e5fc1d2d3fd318a8d94cf05.json`)
        }],
    factoryNetworks: [
        {
            name: mainnetName,
            blockchain: ethereumBlockchain,
            contractAddr: "0x6e42262978de5233c8d5b05b128c121fba110da4",
            abi: require(`../${getPluginConfigBasePath(mainnetName, ethereumBlockchain)}/abis/0x6e42262978de5233c8d5b05b128c121fba110da4.json`)
        },
        {
            name: ropstenName,
            blockchain: ethereumBlockchain,
            contractAddr: "0x939d0308ce4274c287e7305d381b336b77dbfcd3",
            abi: require(`../${getPluginConfigBasePath(ropstenName, ethereumBlockchain)}/abis/0x939d0308ce4274c287e7305d381b336b77dbfcd3.json`)
        }],
}

for (const networkName in networks) {
    networks[networkName] = networks[networkName].reduce((networkByDevice, currentNetwork) => {
        devices.forEach((device) => networkByDevice.push({
            ...currentNetwork,
            device,
            toString: function () {
                return `[${this.device}][${this.blockchain}][${this.name}]`;
            }
        }))

        return networkByDevice
    }, [])
}

let genericTx = {
    nonce: Number(0),
    gasLimit: Number(21000),
    gasPrice: parseUnits('1', 'gwei'),
    value: parseEther('1'),
    chainId: 1,
    to: RANDOM_ADDRESS,
    data: null,
};

const TIMEOUT = 1000000;

// Generates a serializedTransaction from a rawHexTransaction copy pasted from etherscan.
function txFromEtherscan(rawTx) {
    // Remove 0x prefix
    rawTx = rawTx.slice(2);

    let txType = rawTx.slice(0, 2);
    if (txType == "02" || txType == "01") {
        // Remove "02" prefix
        rawTx = rawTx.slice(2);
    } else {
        txType = "";
    }

    let decoded = RLP.decode("0x" + rawTx);
    if (txType != "") {
        decoded = decoded.slice(0, decoded.length - 3); // remove v, r, s
    } else {
        decoded[decoded.length - 1] = "0x"; // empty
        decoded[decoded.length - 2] = "0x"; // empty
        decoded[decoded.length - 3] = "0x01"; // chainID 1
    }

    // Encode back the data, drop the '0x' prefix
    let encoded = RLP.encode(decoded).slice(2);

    // Don't forget to prepend the txtype
    return txType + encoded;
}

function zemu(device, folderName, func) {
    return async () => {
        jest.setTimeout(TIMEOUT);
        let eth_path;
        let plugin;
        let sim_options = sim_options_generic;

        device = device.replace(" ", "").toLowerCase()

        switch(device){
            case "nanos":
                eth_path = NANOS_ETH_PATH;
                plugin = NANOS_PLUGIN;
                sim_options.model = "nanos";
                break;
            case "nanosp":
                eth_path = NANOSP_ETH_PATH;
                plugin = NANOSP_PLUGIN;
                sim_options.model = "nanosp";
                break;
            case "nanox":
                eth_path = NANOX_ETH_PATH;
                plugin = NANOX_PLUGIN;
                sim_options.model = "nanox";
                break;
            default:
                throw new Error(`device ${device} not supported`);
        }

        const sim = new Zemu(eth_path, plugin);

        try {
            await sim.start(sim_options);
            const transport = await sim.getTransport();
            const eth = new Eth(transport);
            eth.setLoadConfig({
                baseURL: null,
                extraPlugins: generate_plugin_config(folderName),
            });
            await func(sim, eth);
        } finally {
            await sim.close();
        }
    };
}

module.exports = {
    zemu,
    networks,
    waitForAppScreen,
    genericTx,
    SPECULOS_ADDRESS,
    RANDOM_ADDRESS,
    txFromEtherscan,
}