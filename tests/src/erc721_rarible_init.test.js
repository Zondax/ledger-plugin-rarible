import "core-js/stable";
import "regenerator-runtime/runtime";
import {waitForAppScreen, zemu, genericTx, networks} from './test.fixture';
import {ethers} from "ethers";

const {assetNetworks} = networks

// Nanos S test
test.each(assetNetworks)("%s - ERC721 Rarible Init from Contract", async ({name, device, abi, contractAddr}) => {
    await zemu(device, name, async (sim, eth) => {
        const contract = new ethers.Contract(contractAddr, abi);

        // Constants used to create the transaction
        const _name = "Black and White"
        const _symbol = "BWA"
        const baseURI = "ipfs:/"
        const contractURI = "https://api-mainnet.rarible.com/contractMetadata/{address}"
        const transferProxy = "0xc02aaa39b223fe8d0a0e5c4f27ead9083c756cc2"
        const lazyTransferProxy = "0xc02aaa39b223fe8d0a0e5c4f27ead9083c756cc2"

        // Adapt the signature to the method
        const {data} = await contract.populateTransaction["__ERC721Rarible_init"](_name, _symbol, baseURI, contractURI, transferProxy, lazyTransferProxy);

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
        await sim.navigateAndCompareSnapshots('.', `${device}_${name}_erc721_rarible_init_from_contract`, [device === "nanos" ? 11 : 7, 0]);

        await tx;
    })();
})