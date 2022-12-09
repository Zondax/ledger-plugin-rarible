import "core-js/stable";
import "regenerator-runtime/runtime";
import {waitForAppScreen, zemu, genericTx, networks} from './test.fixture';
import {ethers} from "ethers";

const {assetNetworks} = networks

// Transactions can be found on etherscan with https://rinkeby.etherscan.io/tx/0x0ca11708e11e86917a82a0dee4b7a40e561cbbc5da8772754ba0b2c6ab914eae

test.each(assetNetworks)("%s - Set Approval For All from Contract", async ({name, device, abi, contractAddr}) => {
    await zemu(device, name, async (sim, eth) => {
        const contract = new ethers.Contract(contractAddr, abi);

        // Constants used to create the transaction
        const operator = "0x15557c8b7246C38EE71eA6dc69e4347F5DAc2104";
        const approved = true;

        const {data} = await contract.populateTransaction.setApprovalForAll(operator, approved);

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
        await sim.navigateAndCompareSnapshots('.', `${device}_${name}_set_approval_for_all_from_contract`, [device === "nanos" ? 7 : 5, 0]);

        await tx;
    })();
})

test.each(assetNetworks)("%s - Set Approval For All from Etherscan", async ({name, device, abi, contractAddr}) => {
    await zemu(device, name, async (sim, eth) => {
        // Rather than constructing the tx ourselves, one can can obtain it directly through etherscan.
        // The rawTx of the tx up above is accessible through: https://rinkeby.etherscan.io/tx/0x0ca11708e11e86917a82a0dee4b7a40e561cbbc5da8772754ba0b2c6ab914eae
        // In this case, you could remove the above code, and simply create your tx like so:
        const data = "0xa22cb4650000000000000000000000007d47126a2600e22eab9ed6cf0e515678727779a60000000000000000000000000000000000000000000000000000000000000001";

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
        await sim.navigateAndCompareSnapshots('.', `${device}_${name}_set_approval_for_all_from_etherscan`, [device === "nanos" ? 7 : 5, 0]);

        await tx;
    })();
})

