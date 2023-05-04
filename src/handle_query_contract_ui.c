#include "stdio.h"
#include "rarible_plugin.h"

// Set UI for any address screen.
static void set_address_ui(ethQueryContractUI_t *msg, address_t *value) {
    // Prefix the address with `0x`.
    msg->msg[0] = '0';
    msg->msg[1] = 'x';

    // We need a random chainID for legacy reasons with `getEthAddressStringFromBinary`.
    // Setting it to `0` will make it work with every chainID :)
    uint64_t chainid = 0;

    // Get the string representation of the address stored in `context->beneficiary`. Put it in
    // `msg->msg`.
    getEthAddressStringFromBinary(
        value->address,
        msg->msg + 2,  // +2 here because we've already prefixed with '0x'.
        msg->pluginSharedRW->sha3,
        chainid);
}

// Set UI for "IsApproved" screen.
static void set_is_approved(ethQueryContractUI_t *msg, u_int8_t isApproved) {
    strlcpy(msg->title, "Approved", msg->titleLength);

    if (isApproved == 0) {
        snprintf(msg->msg, msg->msgLength, "%s", "False");
    } else {
        snprintf(msg->msg, msg->msgLength, "%s", "True");
    }
}

// Set UI for "Receive" screen.
static void set_token_id(ethQueryContractUI_t *msg, token_id_t *id) {
    strlcpy(msg->title, "ID", msg->titleLength);
    uint256_to_decimal(id->value, sizeof(id->value), msg->msg, msg->msgLength);
}

// Set UI for "Beneficiary" screen.
static void set_beneficiary_ui(ethQueryContractUI_t *msg, address_t *beneficiary) {
    strlcpy(msg->title, "Beneficiary", msg->titleLength);

    set_address_ui(msg, beneficiary);
}

// Set UI for "Seller" screen.
static void set_seller_ui(ethQueryContractUI_t *msg, address_t *seller) {
    strlcpy(msg->title, "Seller", msg->titleLength);

    set_address_ui(msg, seller);
}

// Set UI for "Creator" screen.
static void set_creator_ui(ethQueryContractUI_t *msg, address_t *creator) {
    strlcpy(msg->title, "Creator", msg->titleLength);

    set_address_ui(msg, creator);
}

// Set UI for empty "Creator" screen.
static void set_no_creator_ui(ethQueryContractUI_t *msg) {
    strlcpy(msg->title, "Creator", msg->titleLength);
    strlcpy(msg->msg, "No Creators", msg->msgLength);
}

// Set UI for "To" screen.
static void set_to_ui(ethQueryContractUI_t *msg, address_t *address) {
    strlcpy(msg->title, "To", msg->titleLength);

    set_address_ui(msg, address);
}

// Set UI for "Operator" screen.
static void set_operator_ui(ethQueryContractUI_t *msg, address_t *operator) {
    strlcpy(msg->title, "Operator", msg->titleLength);

    set_address_ui(msg, operator);
}

static void set_no_operator_ui(ethQueryContractUI_t *msg) {
    strlcpy(msg->title, "Operator", msg->titleLength);
    strlcpy(msg->msg, "No Operators", msg->msgLength);
}

// Set UI for "Transfer Proxy" screen.
static void set_transfer_proxy_ui(ethQueryContractUI_t *msg, address_t *address) {
    strlcpy(msg->title, "Transfer", msg->titleLength);

    set_address_ui(msg, address);
}

// Set UI for "Lazy Transfer Proxy" screen.
static void set_lazy_transfer_proxy_ui(ethQueryContractUI_t *msg, address_t *address) {
    strlcpy(msg->title, "Lazy Transfer", msg->titleLength);

    set_address_ui(msg, address);
}

// Set UI for "Name" screen.
static void set_name_ui(ethQueryContractUI_t *msg, name_t *name) {
    strlcpy(msg->title, "Name", msg->titleLength);

    snprintf(msg->msg, msg->msgLength, "%s", name->text);
}

// Set UI for "Symbol" screen.
static void set_symbol_ui(ethQueryContractUI_t *msg, symbol_t *symbol) {
    strlcpy(msg->title, "Symbol", msg->titleLength);

    snprintf(msg->msg, msg->msgLength, "%s", symbol->text);
}

/*
// Set UI for "Signature" screen.
static void set_signature_ui(ethQueryContractUI_t *msg, context_t *context) {
    strlcpy(msg->title, "Signature", msg->titleLength);

    // Prefix the address with `0x`.
    msg->msg[0] = '0';
    msg->msg[1] = 'x';

    // We need a random chainID for legacy reasons with `getEthAddressStringFromBinary`.
    // Setting it to `0` will make it work with every chainID :)
    uint64_t chainid = 0;

    // Get the string representation of the address stored in `context->beneficiary`. Put it in
    // `msg->msg`.
    getEthAddressStringFromBinary(
        context->tx.body.mint_and_transfer.signature,
        msg->msg + 2,  // +2 here because we've already prefixed with '0x'.
        msg->pluginSharedRW->sha3,
        chainid);
}
*/

// Set UI for "Royalties" screen.
static void set_royalties_ui(ethQueryContractUI_t *msg, uint16_t royalties) {
    strlcpy(msg->title, "Royalties", msg->titleLength);

    if (royalties <= 9) {
        snprintf(msg->msg, msg->msgLength, "0.0%d%%", royalties);
    } else if (royalties <= 99) {
        snprintf(msg->msg, msg->msgLength, "0.%d%%", royalties);
    } else if (royalties <= 10000){
        u_int8_t decimal = royalties % 100;
        if (decimal <= 9) {
            snprintf(msg->msg, msg->msgLength, "%d.0%d%%", royalties / 100, decimal);
        } else {
            snprintf(msg->msg, msg->msgLength, "%d.%d%%", royalties / 100, decimal);
        }
    } else {
        snprintf(msg->msg, msg->msgLength, "-");
    }
}

// Set UI for "Maker" screen.
static void set_maker_ui(ethQueryContractUI_t *msg, context_t *context) {
    strlcpy(msg->title, "Maker", msg->titleLength);

    set_address_ui(msg, &context->tx.body.cancel.maker);
}

// Set UI for "Taker" screen.
static void set_taker_ui(ethQueryContractUI_t *msg, context_t *context) {
    strlcpy(msg->title, "Taker", msg->titleLength);

    set_address_ui(msg, &context->tx.body.cancel.taker);
}

// Set UI for "Order Left Maker" screen.
static void set_order_left_maker_ui(ethQueryContractUI_t *msg, context_t *context) {
    strlcpy(msg->title, "Left Maker", msg->titleLength);

    set_address_ui(msg, &context->tx.body.match_orders.left.maker);
}

// Set UI for "Order Left Taker" screen.
static void set_order_left_taker_ui(ethQueryContractUI_t *msg, context_t *context) {
    strlcpy(msg->title, "Left Taker", msg->titleLength);

    set_address_ui(msg, &context->tx.body.match_orders.left.taker);
}

// Set UI for "Order Right Maker" screen.
static void set_order_right_maker_ui(ethQueryContractUI_t *msg, context_t *context) {
    strlcpy(msg->title, "Right Maker", msg->titleLength);

    set_address_ui(msg, &context->tx.body.match_orders.right.maker);
}

// Set UI for "Order Right Taker" screen.
static void set_order_right_taker_ui(ethQueryContractUI_t *msg, context_t *context) {
    strlcpy(msg->title, "Right Taker", msg->titleLength);

    set_address_ui(msg, &context->tx.body.match_orders.right.taker);
}

void handle_query_contract_ui(void *parameters) {
    ethQueryContractUI_t *msg = (ethQueryContractUI_t *) parameters;
    context_t *context = (context_t *) msg->pluginContext;

    // msg->title is the upper line displayed on the device.
    // msg->msg is the lower line displayed on the device.

    // Clean the display fields.
    memset(msg->title, 0, msg->titleLength);
    memset(msg->msg, 0, msg->msgLength);

    msg->result = ETH_PLUGIN_RESULT_OK;

    switch (context->selectorIndex) {
        case MINT_AND_TRANSFER:
            switch (msg->screenIndex) {
                case 0:
                    set_token_id(msg, &context->tx.body.mint_and_transfer.id);
                    break;
                case 1:
                    set_beneficiary_ui(msg, &context->tx.body.mint_and_transfer.beneficiary);
                    break;
                case 2:
                    if(context->tx.body.mint_and_transfer.creator_found) {
                        set_creator_ui(msg, &context->tx.body.mint_and_transfer.creator);
                    } else {
                        set_no_creator_ui(msg);
                    }
                    break;
                case 3:
                    set_royalties_ui(msg, context->tx.body.mint_and_transfer.royalties);
                    break;

                // Leave this commented as it is not necessary to show signatures
                // case 4:
                //    set_signature_ui(msg, context);
                //    break;
                // Keep this
                default:
                    PRINTF("Received an invalid screenIndex\n");
                    msg->result = ETH_PLUGIN_RESULT_ERROR;
                    return;
            }
            break;

        case CANCEL:
            switch (msg->screenIndex) {
                case 0:
                    set_maker_ui(msg, context);
                    break;
                case 1:
                    set_taker_ui(msg, context);
                    break;
                default:
                    PRINTF("Received an invalid screenIndex\n");
                    msg->result = ETH_PLUGIN_RESULT_ERROR;
                    return;
            }
            break;

        case MATCH_ORDERS:
            switch (msg->screenIndex) {
                case 0:
                    set_order_left_maker_ui(msg, context);
                    break;
                case 1:
                    set_order_left_taker_ui(msg, context);
                    break;
                case 2:
                    set_order_right_maker_ui(msg, context);
                    break;
                case 3:
                    set_order_right_taker_ui(msg, context);
                    break;
                default:
                    PRINTF("Received an invalid screenIndex\n");
                    msg->result = ETH_PLUGIN_RESULT_ERROR;
                    return;
            }
            break;

        case TRANSFER_FROM_OR_MINT:
            switch (msg->screenIndex) {
                case 0:
                    set_token_id(msg, &context->tx.body.transfer_from_or_mint.id);
                    break;
                case 1:
                    set_seller_ui(msg, &context->tx.body.transfer_from_or_mint.seller);
                    break;
                case 2:
                    set_beneficiary_ui(msg, &context->tx.body.transfer_from_or_mint.beneficiary);
                    break;
                case 3:
                    set_creator_ui(msg, &context->tx.body.transfer_from_or_mint.creator);
                    break;
                case 4:
                    set_royalties_ui(msg, context->tx.body.transfer_from_or_mint.royalties);
                    break;

                // Leave this commented as it is not necessary to show signatures
                // case 5:
                //    set_signature_ui(msg, context);
                //    break;
                // Keep this
                default:
                    PRINTF("Received an invalid screenIndex\n");
                    msg->result = ETH_PLUGIN_RESULT_ERROR;
                    return;
            }
            break;

        case SET_APPROVED_FOR_ALL:
            switch (msg->screenIndex) {
                case 0:
                    set_to_ui(msg, &context->tx.body.set_approval_for_all.to);
                    break;
                case 1:
                    set_is_approved(msg, context->tx.body.set_approval_for_all.isApproved);
                    break;
                default:
                    PRINTF("Received an invalid screenIndex\n");
                    msg->result = ETH_PLUGIN_RESULT_ERROR;
                    return;
            }
            break;

        case CREATE_TOKEN:
            switch (msg->screenIndex) {
                case 0:
                    set_name_ui(msg, &context->tx.body.create_token.name);
                    break;
                case 1:
                    set_symbol_ui(msg, &context->tx.body.create_token.symbol);
                    break;
                case 2:
                    if(context->tx.body.create_token.operator_found){
                        set_operator_ui(msg, &context->tx.body.create_token.operator);
                    } else {
                        set_no_operator_ui(msg);
                    }
                    break;
                default:
                    PRINTF("Received an invalid screenIndex\n");
                    msg->result = ETH_PLUGIN_RESULT_ERROR;
                    return;
            }
            break;

        case ERC721_RARIBLE_INIT:
            switch (msg->screenIndex) {
                case 0:
                    set_name_ui(msg, &context->tx.body.erc721_rarible_init.name);
                    break;
                case 1:
                    set_symbol_ui(msg, &context->tx.body.erc721_rarible_init.symbol);
                    break;
                case 2:
                    set_transfer_proxy_ui(msg,
                                          &context->tx.body.erc721_rarible_init.transfer_proxy);
                    break;
                case 3:
                    set_lazy_transfer_proxy_ui(
                        msg,
                        &context->tx.body.erc721_rarible_init.lazy_transfer_proxy);
                    break;
                default:
                    PRINTF("Received an invalid screenIndex\n");
                    msg->result = ETH_PLUGIN_RESULT_ERROR;
                    return;
            }
            break;
    }
}
