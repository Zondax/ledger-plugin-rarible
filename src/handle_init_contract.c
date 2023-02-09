#include "rarible_plugin.h"

// Called once to init.
void handle_init_contract(void *parameters) {
    // Cast the msg to the type of structure we expect (here, ethPluginInitContract_t).
    ethPluginInitContract_t *msg = (ethPluginInitContract_t *) parameters;

    // Make sure we are running a compatible version.
    if (msg->interfaceVersion != ETH_PLUGIN_INTERFACE_VERSION_LATEST) {
        // If not the case, return the `UNAVAILABLE` status.
        msg->result = ETH_PLUGIN_RESULT_UNAVAILABLE;
        return;
    }

    // Double check that the `context_t` struct is not bigger than the maximum size (defined by
    // `msg->pluginContextLength`).
    if (msg->pluginContextLength < sizeof(context_t)) {
        PRINTF("Plugin parameters structure is bigger than allowed size\n");
        msg->result = ETH_PLUGIN_RESULT_ERROR;
        return;
    }

    context_t *context = (context_t *) msg->pluginContext;

    // Initialize the context (to 0).
    memset(context, 0, sizeof(context_t));

    // Look for the index of the selectorIndex passed in by `msg`.
    uint8_t i;
    for (i = 0; i < NUM_SELECTORS; i++) {
        if (memcmp((uint8_t *) PIC(RARIBLE_SELECTORS[i]), msg->selector, SELECTOR_SIZE) == 0) {
            context->selectorIndex = i;
            break;
        }
    }

    // If `i == NUM_SELECTORS` it means we haven't found the selector. Return an error.
    if (i == NUM_SELECTORS) {
        msg->result = ETH_PLUGIN_RESULT_UNAVAILABLE;
        return;
    }

    switch (context->selectorIndex) {
        case MINT_AND_TRANSFER:
            // Set `next_param` to be the first field we expect to parse.
            context->next_param = BODY_OFFSET;
            break;
        case CANCEL:
            // Set `next_param` to be the first field we expect to parse.
            context->next_param = BODY_OFFSET;
            break;
        case MATCH_ORDERS:
            // Set `next_param` to be the first field we expect to parse.
            context->next_param = LEFT_ORDER_OFFSET;
            break;
        case TRANSFER_FROM_OR_MINT:
            // Set `next_param` to be the first field we expect to parse.
            context->next_param = BODY_OFFSET;
            break;
        case SET_APPROVED_FOR_ALL:
            // Set `next_param` to be the first field we expect to parse.
            context->next_param = TO;
            break;
        case CREATE_TOKEN:
            // Set `next_param` to be the first field we expect to parse.
            context->next_param = NAME_OFFSET;
            break;
        case ERC721_RARIBLE_INIT:
            // Set `next_param` to be the first field we expect to parse.
            context->next_param = NAME_OFFSET;
            break;
        // Keep this
        default:
            PRINTF("Missing selectorIndex: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return;
    }

    // Return valid status.
    msg->result = ETH_PLUGIN_RESULT_OK;
}
