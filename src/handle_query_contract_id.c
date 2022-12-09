#include "rarible_plugin.h"

// Sets the first screen to display.
void handle_query_contract_id(void *parameters) {
    ethQueryContractID_t *msg = (ethQueryContractID_t *) parameters;
    context_t *context = (context_t *) msg->pluginContext;
    // msg->name will be the upper sentence displayed on the screen.
    // msg->version will be the lower sentence displayed on the screen.

    // For the first screen, display the plugin name.
    strlcpy(msg->name, PLUGIN_NAME, msg->nameLength);

    switch (context->selectorIndex) {
        case MINT_AND_TRANSFER:
            strlcpy(msg->version, "Mint&Transfer", msg->versionLength);
            break;
        case CANCEL:
            strlcpy(msg->version, "Cancel", msg->versionLength);
            break;
        case MATCH_ORDERS:
            strlcpy(msg->version, "Match Orders", msg->versionLength);
            break;
        case TRANSFER_FROM_OR_MINT:
            strlcpy(msg->version, "TransferFromOrMint", msg->versionLength);
            break;
        case SET_APPROVED_FOR_ALL:
            strlcpy(msg->version, "SetApprovalForAll", msg->versionLength);
            break;
        case CREATE_TOKEN:
            strlcpy(msg->version, "Create Token", msg->versionLength);
            break;
        case ERC721_RARIBLE_INIT:
            strlcpy(msg->version, "ERC 721 Init", msg->versionLength);
            break;
        // Keep this
        default:
            PRINTF("Selector index: %d not supported\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }

    msg->result = ETH_PLUGIN_RESULT_OK;
}