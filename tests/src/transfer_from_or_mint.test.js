import "core-js/stable";
import "regenerator-runtime/runtime";
import {waitForAppScreen, zemu, genericTx, networks} from './test.fixture';
import {ethers} from "ethers";

const {assetNetworks} = networks

// Transactions can be found on etherscan with https://ropsten.etherscan.io/address/0xd4224267C4aB4a184bD1aa066b3361E70EfBBEaf

// Nanos S test
test.each(assetNetworks)("%s - Transfer From Or Mint from Contract", async ({name, device, abi, contractAddr}) => {
    await zemu(device, name, async (sim, eth) => {
        const contract = new ethers.Contract(contractAddr, abi);

        // Constants used to create the transaction
        const body = {
            tokenId: 51427049545,
            tokenURI: "/ipfs/QmWLsBu6nS4ovaHbGAXprD1qEssJu4r5taQfB74sCG51tp",
            creators: [
                {account: "0xc02aaa39b223fe8d0a0e5c4f27ead9083c756cc2", value: 1000},
                {account: "0xc02aaa39b223fe8d0a0e5c4f27ead9083c756cc2", value: 1000}
            ],
            royalties: [
                {account: "0x6b3595068778dd592e39a122f4f5a5cf09c90fe2", value: 1000},
                {account: "0x6b3595068778dd592e39a122f4f5a5cf09c90fe2", value: 1000}
            ],
            signatures: [
                [...Buffer.from("3045022100f6e1a922c745e244fa3ed9a865491672808ef93f492ee0410861d7", "hex")],
                [...Buffer.from("3045022100f6e1a922c745e244fa3ed9a865491672808ef93f492ee0410861d7", "hex")]
            ],
        }
        const from = "0xc02aaa39b223fe8d0a0e5c4f27ead9083c756cc2"
        const to = "0x15557c8b7246C38EE71eA6dc69e4347F5DAc2104"

        // Adapt the signature to the method
        const {data} = await contract.populateTransaction.transferFromOrMint(body, from, to);

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
        await sim.navigateAndCompareSnapshots('.', `${device}_${name}_transfer_from_or_mint_from_contract`, [device === "nanos" ? 14 : 8, 0]);

        await tx;
    })();
})

