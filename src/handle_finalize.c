#include "rarible_plugin.h"

void handle_finalize(void *parameters) {
    ethPluginFinalize_t *msg = (ethPluginFinalize_t *) parameters;
    context_t *context = (context_t *) msg->pluginContext;

    msg->uiType = ETH_UI_TYPE_GENERIC;

    // The total number of screen you will need.
    switch (context->selectorIndex) {
        case MINT_AND_TRANSFER:
            msg->numScreens = 4;
            break;
        case CANCEL:
            msg->numScreens = 2;
            break;
        case MATCH_ORDERS:
            msg->numScreens = 4;
            break;
        case TRANSFER_FROM_OR_MINT:
            msg->numScreens = 5;
            break;
        case SET_APPROVED_FOR_ALL:
            msg->numScreens = 2;
            break;
        case CREATE_TOKEN:
            msg->numScreens = 3;
            break;
        case ERC721_RARIBLE_INIT:
            msg->numScreens = 4;
            break;
        default:
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }

    msg->result = ETH_PLUGIN_RESULT_OK;
}
