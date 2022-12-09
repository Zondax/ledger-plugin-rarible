import "core-js/stable";
import "regenerator-runtime/runtime";
import {waitForAppScreen, zemu, genericTx, networks} from './test.fixture';
import {ethers} from "ethers";

const {factoryNetworks} = networks

// Transactions can be found on etherscan with https://etherscan.io/address/0x6E42262978de5233C8d5B05B128C121fBa110DA4

// Nanos S test
test.each(factoryNetworks)("%s - Create Token from Contract", async ({name, device, abi, contractAddr}) => {
    await zemu(device, name, async (sim, eth) => {
        const contract = new ethers.Contract(contractAddr, abi);

        // Constants used to create the transaction
        const _name = "Black and White"
        const _symbol = "BWA"
        const baseURI = "ipfs:/"
        const contractURI = "https://api-mainnet.rarible.com/contractMetadata/{address}"
        const operators = [
            "0xc02aaa39b223fe8d0a0e5c4f27ead9083c756cc2"
        ]
        const salt = 1000

        // Adapt the signature to the method
        const {data} = await contract.populateTransaction["createToken(string,string,string,string,address[],uint256)"](_name, _symbol, baseURI, contractURI, operators, salt);

        // Get the generic transaction template
        let unsignedTx = genericTx;
        // Modify `to` to make it interact with the contract
        unsignedTx.to = contractAddr;
        // Modify the attached data
        unsignedTx.data = data;

        // Create serializedTx and remove the "0x" prefix
        const serializedTx = ethers.utils.serializeTransaction(unsignedTx).slice(2);

        const tx = eth.signTransaction(
            "44'/60'/0'/0",
            serializedTx
        );

        // Wait for the application to actually load and parse the transaction
        await waitForAppScreen(sim);

        // Navigate the display by pressing the right button 10 times, then pressing both buttons to accept the transaction.
        await sim.navigateAndCompareSnapshots('.', `${device}_${name}_create_token_from_contract`, [device === "nanos" ? 8 : 6, 0]);

        await tx;
    })();
})


test.each(factoryNetworks)("%s - Create Token from Etherscan", async ({name, device, abi, contractAddr}) => {
    await zemu(device, name, async (sim, eth) => {
        // Rather than constructing the tx ourselves, one can can obtain it directly through etherscan.
        // The rawTx of the tx up above is accessible through: https://ropsten.etherscan.io/tx/0x3d3fe4e8a00235269a7fd994bf925b04748a40250eb8651392c0bd194c3832ae
        // In this case, you could remove the above code, and simply create your tx like so:
        const data = "0x27050d1f00000000000000000000000000000000000000000000000000000000000000c000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000140000000000000000000000000000000000000000000000000000000000000018000000000000000000000000000000000000000000000000000000000000001e0c62b33085b3c0e19039b6cec2ea30536d673a212f20ad1920440e032c2d8650c000000000000000000000000000000000000000000000000000000000000000653756e7365740000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000253740000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000006697066733a2f0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000003a68747470733a2f2f6170692d726f707374656e2e72617269626c652e636f6d2f636f6e74726163744d657461646174612f7b616464726573737d0000000000000000000000000000000000000000000000000000000000000000000000000000";

        // Get the generic transaction template
        let unsignedTx = genericTx;
        // Modify `to` to make it interact with the contract
        unsignedTx.to = contractAddr;
        // Modify the attached data
        unsignedTx.data = data;

        // Create serializedTx and remove the "0x" prefix
        const serializedTx = ethers.utils.serializeTransaction(unsignedTx).slice(2);

        const tx = eth.signTransaction(
            "44'/60'/0'/0",
            serializedTx
        );

        // Wait for the application to actually load and parse the transaction
        await waitForAppScreen(sim);
        // Navigate the display by pressing the right button 10 times, then pressing both buttons to accept the transaction.
        // EDIT THIS: modify `10` to fix the number of screens you are expecting to navigate through.
        await sim.navigateAndCompareSnapshots('.', `${device}_${name}_create_token_from_etherscan`, [device === "nanos" ? 8 : 6, 0]);

        await tx;
    })();
})

