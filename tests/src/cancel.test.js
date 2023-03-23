import "core-js/stable";
import "regenerator-runtime/runtime";
import {waitForAppScreen, zemu, genericTx, networks} from './test.fixture';
import {ethers} from "ethers";

const {exchangeNetworks} = networks

// Transactions can be found on etherscan with https://ropsten.etherscan.io/address/0xd4224267C4aB4a184bD1aa066b3361E70EfBBEaf

test.each(exchangeNetworks)("%s - Cancel from Contract", async ({name, device, abi, contractAddr}) => {
    await zemu(device, name, async (sim, eth) => {
        const contract = new ethers.Contract(contractAddr, abi);

        // Constants used to create the transaction
        const order = {
            maker: "0x15557c8b7246C38EE71eA6dc69e4347F5DAc2104",
            makeAsset: {
                assetType: {
                    assetClass: Buffer.from("30450221", "hex"),
                    data: Buffer.from("3045022100f6e1a922c745e244fa", "hex")
                },
                value: 1000
            },
            taker: "0x15557c8b7246C38EE71eA6dc69e4347F5DAc2104",
            takeAsset: {
                assetType: {
                    assetClass: Buffer.from("30450221", "hex"),
                    data: Buffer.from("3045022100f6e1a922c745e244fa", "hex")
                },
                value: 1000
            },
            salt: 1000,
            start: 1000,
            end: 1000,
            dataType: Buffer.from("30450221", "hex"),
            data: Buffer.from("3045022100f6e1a922c745e244fa", "hex")
        }

        const {data} = await contract.populateTransaction.cancel(order);

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
        await sim.navigateAndCompareSnapshots('.', `${device}_${name}_cancel_from_contract`, [device === "nanos" ? 9 : 5, 0]);

        await tx;
    })();
})

test.each(exchangeNetworks)("%s - Cancel from Etherscan", async ({name, device, abi, contractAddr}) => {
    await zemu(device, name, async (sim, eth) => {
        // Rather than constructing the tx ourselves, one can can obtain it directly through etherscan.
        // The rawTx of the tx up above is accessible through: https://etherscan.io/tx/0xc51ee06a524906a857a52223acc7f6b16a6c1bed82b6d3752fe9cea371b9a3d4
        // In this case, you could remove the above code, and simply create your tx like so:
        const data = "0xe2864fe3000000000000000000000000000000000000000000000000000000000000002000000000000000000000000019010d5d3db8658af6cec235177d475a1839a89e000000000000000000000000000000000000000000000000000000000000012000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000200395894c930d3f9a014487b04a8c7b2b5ef77c36758288ca09ba782971f8b9541000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000641db51623d235ef0000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000002a00000000000000000000000000000000000000000000000000000000000000040000000000000000000000000000000000000000000000000000000000000000173ad21460000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000400000000000000000000000000000000000000000000000000000000000000040000000000000000000000000899aea45b984e404db9698d294fa2dabe3945b5f000000000000000000000000000000000000000000000000000000000000003e00000000000000000000000000000000000000000000000000000000000000400000000000000000000000000000000000000000000000005fc1b97136320000aaaebeba000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000040000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000020000000000000000000000000000000000000000000000000000000000000006000000000000000000000000000000000000000000000000000000000000000800000000000000000000000000000000000000000000000000000000000000001000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010000000000000000000000001cf0df2a5a20cd61d68d4489eebbf85b8d39e18a0000000000000000000000000000000000000000000000000000000000000064";

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
        await sim.navigateAndCompareSnapshots('.', `${device}_${name}_cancel_from_etherscan`, [device === "nanos" ? 9 : 5, 0]);

        await tx;
    })();
})

