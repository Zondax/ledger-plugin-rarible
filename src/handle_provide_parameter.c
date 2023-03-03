#include "rarible_plugin.h"

static void copy_text(uint8_t *dst, uint16_t dst_len, uint16_t max_len, uint8_t *src) {
    size_t len = MIN(dst_len, max_len);
    memcpy(dst, src, len);
}

static uint16_t get_counter(uint16_t counter) {
    if (counter % PARAMETER_LENGTH != 0) {
        return counter / PARAMETER_LENGTH + 1;
    } else {
        return counter / PARAMETER_LENGTH;
    }
}

static uint8_t parse_asset(ethPluginProvideParameter_t *msg, context_t *context) {
    switch (context->sub_param) {
        case ASSET_TYPE_OFFSET:
            context->sub_param = ASSET_VALUE;
            return 0;
        case ASSET_VALUE:
            context->sub_param = ASSET_CLASS;
            return 0;
        case ASSET_CLASS:
            context->sub_param = ASSET_DATA_OFFSET;
            return 0;
        case ASSET_DATA_OFFSET:
            context->sub_param = ASSET_DATA_LENGTH;
            return 0;
        case ASSET_DATA_LENGTH:
            context->counter = get_counter(U2BE(msg->parameter, PARAMETER_LENGTH - 2));
            context->sub_param = ASSET_DATA;
            return 0;
        case ASSET_DATA:  // wait until reach next field
            context->counter--;
            if (context->counter == 0) {
                return 1;
            }
            return 0;
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            return 0;
    }
}

static void parse_order(ethPluginProvideParameter_t *msg,
                        context_t *context,
                        order_t *order,
                        int8_t next_order) {
    switch (context->next_param) {
        case MAKER:
            copy_address(order->maker.address, msg->parameter, sizeof(order->maker.address));
            context->next_param = MAKE_ASSET_OFFSET;
            break;
        case MAKE_ASSET_OFFSET:
            context->next_param = TAKER;
            break;
        case TAKER:
            copy_address(order->taker.address, msg->parameter, sizeof(order->taker.address));
            context->next_param = TAKE_ASSET_OFFSET;
            break;
        case TAKE_ASSET_OFFSET:
            context->next_param = SALT;
            break;
        case SALT:
            context->next_param = START;
            break;
        case START:
            context->next_param = END;
            break;
        case END:
            context->next_param = DATA_TYPE;
            break;
        case DATA_TYPE:
            context->next_param = DATA_OFFSET;
            break;
        case DATA_OFFSET:
            context->next_param = MAKE_ASSET;
            context->sub_param = ASSET_TYPE_OFFSET;
            break;
        case MAKE_ASSET:
            if (parse_asset(msg, context) == 1) {
                context->next_param = TAKE_ASSET;
                context->sub_param = ASSET_TYPE_OFFSET;
            }
            break;
        case TAKE_ASSET:
            if (parse_asset(msg, context) == 1) {
                context->next_param = DATA_LENGTH;
                context->sub_param = ASSET_TYPE_OFFSET;
            }
            break;
        case DATA_LENGTH:
            context->counter = get_counter(U2BE(msg->parameter, PARAMETER_LENGTH - 2));
            context->next_param = DATA;
            break;
        case DATA:  // wait until reach next field
            context->counter--;
            if (context->counter == 0) {
                context->next_param = SIGNATURE_LENGTH;
            }
            break;
        case SIGNATURE_LENGTH:
            context->counter = get_counter(U2BE(msg->parameter, PARAMETER_LENGTH - 2));
            context->next_param = SIGNATURE;
            break;
        case SIGNATURE:  // wait until reach next field
            context->counter--;
            if (context->counter == 0) {
                context->next_param = MAKER;
                context->tx.body.match_orders.order_side = next_order;
            }
            break;

        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_create_token(ethPluginProvideParameter_t *msg, context_t *context) {
    switch (context->next_param) {
        case NAME_OFFSET:
            context->next_param = SYMBOL_OFFSET;
            break;
        case SYMBOL_OFFSET:
            context->next_param = BASE_URI_OFFSET;
            break;
        case BASE_URI_OFFSET:
            context->next_param = CONTRACT_URI_OFFSET;
            break;
        case CONTRACT_URI_OFFSET:
            context->next_param = OPERATORS_OFFSET;
            break;
        case OPERATORS_OFFSET:
            context->next_param = SALT;
            break;
        case SALT:
            context->next_param = NAME_LENGTH;
            break;
        case NAME_LENGTH:
            context->tx.body.create_token.name.len = U2BE(msg->parameter, PARAMETER_LENGTH - 2);
            context->counter = get_counter(context->tx.body.create_token.name.len);
            context->max_counter = context->counter;
            context->next_param = NAME;
            break;
        case NAME:  // wait until reach next field
            if (context->counter == context->max_counter) {
                copy_text(context->tx.body.create_token.name.text,
                          context->tx.body.create_token.name.len,
                          PARAMETER_LENGTH,
                          msg->parameter);
            }

            context->counter--;
            if (context->counter == 0) {
                context->next_param = SYMBOL_LENGTH;
            }
            break;
        case SYMBOL_LENGTH:
            context->tx.body.create_token.symbol.len = U2BE(msg->parameter, PARAMETER_LENGTH - 2);
            context->counter = get_counter(context->tx.body.create_token.symbol.len);
            context->max_counter = context->counter;
            context->next_param = SYMBOL;
            break;
        case SYMBOL:  // wait until reach next field
            if (context->counter == context->max_counter) {
                copy_text(context->tx.body.create_token.symbol.text,
                          context->tx.body.create_token.symbol.len,
                          PARAMETER_LENGTH,
                          msg->parameter);
            }

            context->counter--;
            if (context->counter == 0) {
                context->next_param = BASE_URI_LENGTH;
            }
            break;
        case BASE_URI_LENGTH:
            context->counter = get_counter(U2BE(msg->parameter, PARAMETER_LENGTH - 2));
            context->next_param = BASE_URI;
            break;
        case BASE_URI:  // wait until reach next field
            context->counter--;
            if (context->counter == 0) {
                context->next_param = CONTRACT_URI_LENGTH;
            }
            break;
        case CONTRACT_URI_LENGTH:
            context->counter = get_counter(U2BE(msg->parameter, PARAMETER_LENGTH - 2));
            context->next_param = CONTRACT_URI;
            break;
        case CONTRACT_URI:  // wait until reach next field
            context->counter--;
            if (context->counter == 0) {
                context->next_param = OPERATORS_QTY;
            }
            break;
        case OPERATORS_QTY:
            context->counter = U2BE(msg->parameter, PARAMETER_LENGTH - 2);

            context->next_param = OPERATORS_ADDRESS;
            if (context->counter == 0) {
                context->next_param = UNEXPECTED_PARAMETER;
            }
            break;
        case OPERATORS_ADDRESS:
            if (context->counter == 1) {  // Fix creators addresses to only 1, the last one.
                copy_address(context->tx.body.create_token.operator.address,
                             msg->parameter,
                             sizeof(context->tx.body.create_token.operator));
            }

            context->next_param = OPERATORS_VALUE;
            break;
        case OPERATORS_VALUE:
            context->counter--;
            if (context->counter != 0) {
                context->next_param = OPERATORS_ADDRESS;
            } else {
                context->next_param = UNEXPECTED_PARAMETER;
            }
            break;
        // Keep this
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_erc721_rarible_init(ethPluginProvideParameter_t *msg, context_t *context) {
    switch (context->next_param) {
        case NAME_OFFSET:
            context->next_param = SYMBOL_OFFSET;
            break;
        case SYMBOL_OFFSET:
            context->next_param = BASE_URI_OFFSET;
            break;
        case BASE_URI_OFFSET:
            context->next_param = CONTRACT_URI_OFFSET;
            break;
        case CONTRACT_URI_OFFSET:
            context->next_param = TRANSFER_PROXY_ADDRESS;
            break;
        case TRANSFER_PROXY_ADDRESS:
            copy_address(context->tx.body.erc721_rarible_init.transfer_proxy.address,
                         msg->parameter,
                         sizeof(context->tx.body.erc721_rarible_init.transfer_proxy));
            context->next_param = LAZY_TRANSFER_PROXY_ADDRESS;
            break;
        case LAZY_TRANSFER_PROXY_ADDRESS:
            copy_address(context->tx.body.erc721_rarible_init.lazy_transfer_proxy.address,
                         msg->parameter,
                         sizeof(context->tx.body.erc721_rarible_init.lazy_transfer_proxy));
            context->next_param = NAME_LENGTH;
            break;
        case NAME_LENGTH:
            context->tx.body.erc721_rarible_init.name.len =
                U2BE(msg->parameter, PARAMETER_LENGTH - 2);
            context->counter = get_counter(context->tx.body.erc721_rarible_init.name.len);
            context->max_counter = context->counter;
            context->next_param = NAME;
            break;
        case NAME:  // wait until reach next field
            if (context->counter == context->max_counter) {
                copy_text(context->tx.body.erc721_rarible_init.name.text,
                          context->tx.body.erc721_rarible_init.name.len,
                          PARAMETER_LENGTH,
                          msg->parameter);
            }

            context->counter--;
            if (context->counter == 0) {
                context->next_param = SYMBOL_LENGTH;
            }
            break;
        case SYMBOL_LENGTH:
            context->tx.body.erc721_rarible_init.symbol.len =
                U2BE(msg->parameter, PARAMETER_LENGTH - 2);
            context->counter = get_counter(context->tx.body.erc721_rarible_init.symbol.len);

            context->max_counter = context->counter;
            context->next_param = SYMBOL;
            break;
        case SYMBOL:  // wait until reach next field
            if (context->counter == context->max_counter) {
                copy_text(context->tx.body.erc721_rarible_init.symbol.text,
                          context->tx.body.erc721_rarible_init.symbol.len,
                          PARAMETER_LENGTH,
                          msg->parameter);
            }

            context->counter--;
            if (context->counter == 0) {
                context->next_param = BASE_URI_LENGTH;
            }
            break;
        case BASE_URI_LENGTH:
            context->counter = get_counter(U2BE(msg->parameter, PARAMETER_LENGTH - 2));
            context->next_param = BASE_URI;
            break;
        case BASE_URI:  // wait until reach next field
            context->counter--;
            if (context->counter == 0) {
                context->next_param = CONTRACT_URI_LENGTH;
            }
            break;
        case CONTRACT_URI_LENGTH:
            context->counter = get_counter(U2BE(msg->parameter, PARAMETER_LENGTH - 2));
            context->next_param = CONTRACT_URI;
            break;
        case CONTRACT_URI:  // wait until reach next field
            context->counter--;
            if (context->counter == 0) {
                context->next_param = UNEXPECTED_PARAMETER;
            }
            break;
        // Keep this
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_set_approved_for_all(ethPluginProvideParameter_t *msg, context_t *context) {
    switch (context->next_param) {
        case TO:  // to
            copy_address(context->tx.body.set_approval_for_all.to.address,
                         msg->parameter,
                         sizeof(context->tx.body.set_approval_for_all.to));
            context->next_param = IS_APPROVED;
            break;
        case IS_APPROVED:  // approved
            context->tx.body.set_approval_for_all.isApproved =
                U2BE(msg->parameter, PARAMETER_LENGTH - 2);
            context->next_param = UNEXPECTED_PARAMETER;
            break;
        // Keep this
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}
static void handle_transfer_from_or_mint(ethPluginProvideParameter_t *msg, context_t *context) {
    switch (context->next_param) {
        case BODY_OFFSET:  // data offset
            context->next_param = BENEFICIARY;
            break;
        case BENEFICIARY:  // to
            copy_address(context->tx.body.transfer_from_or_mint.beneficiary.address,
                         msg->parameter,
                         sizeof(context->tx.body.transfer_from_or_mint.beneficiary));
            context->next_param = SELLER;
            break;
        case SELLER:  // from
            copy_address(context->tx.body.transfer_from_or_mint.seller.address,
                         msg->parameter,
                         sizeof(context->tx.body.transfer_from_or_mint.seller));
            context->next_param = ID;
            break;
        case ID:  // token id
            copy_parameter(context->tx.body.transfer_from_or_mint.id.value,
                           msg->parameter,
                           sizeof(context->tx.body.transfer_from_or_mint.id));
            context->next_param = URI_OFFSET;
            break;
        case URI_OFFSET:  // uri offset
            context->next_param = CREATORS_OFFSET;
            break;
        case CREATORS_OFFSET:  // creators offset
            context->next_param = ROYALTIES_OFFSET;
            break;
        case ROYALTIES_OFFSET:  // royalties offset
            context->next_param = SIGNATURES_OFFSET;
            break;
        case SIGNATURES_OFFSET:  // signatures offset
            context->next_param = URI_CHUNK_OFFSET;
            break;
        case URI_CHUNK_OFFSET:  // uri length
            context->tx.body.transfer_from_or_mint.uri_length =
                    get_counter(U2BE(msg->parameter, PARAMETER_LENGTH - 2));
            context->next_param = WAIT_UNTIL_CREATORS;
            break;
        case WAIT_UNTIL_CREATORS:  // wait until reach next field
            context->tx.body.transfer_from_or_mint.uri_length--;
            if (context->tx.body.transfer_from_or_mint.uri_length == 0) {
                context->next_param = CREATORS_QTY;
            }
            break;
        case CREATORS_QTY:
            context->tx.body.transfer_from_or_mint.creators_qty =
                U2BE(msg->parameter, PARAMETER_LENGTH - 2);

            context->next_param = CREATORS_ADDRESS;
            if (context->tx.body.transfer_from_or_mint.creators_qty == 0) {
                context->next_param = ROYALTIES_QTY;
            }
            break;
        case CREATORS_ADDRESS:
            if (context->tx.body.transfer_from_or_mint.creators_qty ==
                1) {  // Fix creators addresses to only 1, the last one.
                copy_address(context->tx.body.transfer_from_or_mint.creator.address,
                             msg->parameter,
                             sizeof(context->tx.body.transfer_from_or_mint.creator));
            }

            context->next_param = CREATORS_VALUE;
            break;
        case CREATORS_VALUE:
            context->tx.body.transfer_from_or_mint.creators_qty--;
            if (context->tx.body.transfer_from_or_mint.creators_qty != 0) {
                context->next_param = CREATORS_ADDRESS;
            } else {
                context->next_param = ROYALTIES_QTY;
            }
            break;
        case ROYALTIES_QTY:
            context->tx.body.transfer_from_or_mint.royalties_qty =
                U2BE(msg->parameter, PARAMETER_LENGTH - 2);

            context->next_param = ROYALTIES_ADDRESS;
            if (context->tx.body.transfer_from_or_mint.royalties_qty == 0) {
                context->next_param = SIGNATURES_QTY;
            }
            break;
        case ROYALTIES_ADDRESS:
            context->next_param = ROYALTIES_VALUE;
            break;
        case ROYALTIES_VALUE:
            if (context->tx.body.transfer_from_or_mint.royalties_qty ==
                1) {  // Fix royalties values to only 1, the last one.
                context->tx.body.transfer_from_or_mint.royalties =
                    U2BE(msg->parameter, PARAMETER_LENGTH - 2);
            }

            context->tx.body.transfer_from_or_mint.royalties_qty--;
            if (context->tx.body.transfer_from_or_mint.royalties_qty != 0) {
                context->next_param = ROYALTIES_ADDRESS;
            } else {
                context->next_param = SIGNATURES_QTY;
            }
            break;

        // Signature logic is here, but it is not being used as we don't need to show them.
        case SIGNATURES_QTY:
            context->tx.body.transfer_from_or_mint.signatures_qty =
                U2BE(msg->parameter, PARAMETER_LENGTH - 2);
            context->tx.body.transfer_from_or_mint.signatures_counter =
                context->tx.body.transfer_from_or_mint.signatures_qty;

            context->next_param = SIGNATURE_OFFSET;
            if (context->tx.body.transfer_from_or_mint.signatures_qty == 0) {
                context->next_param = UNEXPECTED_PARAMETER;
            }
            break;
        case SIGNATURE_OFFSET:
            context->tx.body.transfer_from_or_mint.signatures_counter--;
            if (context->tx.body.transfer_from_or_mint.signatures_counter == 0) {
                context->next_param = SIGNATURE_LENGTH;
            }
            break;
        case SIGNATURE_LENGTH:
            context->tx.body.transfer_from_or_mint.signature_length =
                get_counter(U2BE(msg->parameter, PARAMETER_LENGTH - 2));

            context->next_param = SIGNATURE;
            break;
        case SIGNATURE:
            // Signature logic is here, but it is not being used. We won't copy anything
            // if(context->signature_length == 1){
            //     copy_parameter(context->signature, msg->parameter, sizeof(context->signature));
            // }

            context->tx.body.transfer_from_or_mint.signature_length--;
            if (context->tx.body.transfer_from_or_mint.signature_length == 0) {
                context->tx.body.transfer_from_or_mint.signatures_qty--;
                if (context->tx.body.transfer_from_or_mint.signatures_qty != 0) {
                    context->next_param = SIGNATURE_LENGTH;
                } else {
                    context->next_param = UNEXPECTED_PARAMETER;
                }
            }
            break;

        // Keep this
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_mint_and_transfer(ethPluginProvideParameter_t *msg, context_t *context) {
    switch (context->next_param) {
        case BODY_OFFSET:  // data offset
            context->next_param = BENEFICIARY;
            break;
        case BENEFICIARY:  // to
            copy_address(context->tx.body.mint_and_transfer.beneficiary.address,
                         msg->parameter,
                         sizeof(context->tx.body.mint_and_transfer.beneficiary));
            context->next_param = ID;
            break;
        case ID:  // token id
            copy_parameter(context->tx.body.mint_and_transfer.id.value,
                           msg->parameter,
                           sizeof(context->tx.body.mint_and_transfer.id));
            context->next_param = URI_OFFSET;
            break;
        case URI_OFFSET:  // uri offset
            context->next_param = CREATORS_OFFSET;
            break;
        case CREATORS_OFFSET:  // creators offset
            context->next_param = ROYALTIES_OFFSET;
            break;
        case ROYALTIES_OFFSET:  // royalties offset
            context->next_param = SIGNATURES_OFFSET;
            break;
        case SIGNATURES_OFFSET:  // signatures offset
            context->next_param = URI_CHUNK_OFFSET;
            break;
        case URI_CHUNK_OFFSET:  // uri length
            context->tx.body.mint_and_transfer.uri_length =
                    get_counter(U2BE(msg->parameter, PARAMETER_LENGTH - 2));
            context->next_param = WAIT_UNTIL_CREATORS;
            break;
        case WAIT_UNTIL_CREATORS:  // wait until reach next field
            context->tx.body.mint_and_transfer.uri_length--;
            if (context->tx.body.mint_and_transfer.uri_length == 0) {
                context->next_param = CREATORS_QTY;
            }
            break;
        case CREATORS_QTY:
            context->tx.body.mint_and_transfer.creators_qty =
                U2BE(msg->parameter, PARAMETER_LENGTH - 2);

            context->next_param = CREATORS_ADDRESS;
            if (context->tx.body.mint_and_transfer.creators_qty == 0) {
                context->next_param = ROYALTIES_QTY;
            }
            break;
        case CREATORS_ADDRESS:
            if (context->tx.body.mint_and_transfer.creators_qty ==
                1) {  // Fix creators addresses to only 1, the last one.
                copy_address(context->tx.body.mint_and_transfer.creator.address,
                             msg->parameter,
                             sizeof(context->tx.body.mint_and_transfer.creator));
            }

            context->next_param = CREATORS_VALUE;
            break;
        case CREATORS_VALUE:
            context->tx.body.mint_and_transfer.creators_qty--;
            if (context->tx.body.mint_and_transfer.creators_qty != 0) {
                context->next_param = CREATORS_ADDRESS;
            } else {
                context->next_param = ROYALTIES_QTY;
            }
            break;
        case ROYALTIES_QTY:
            context->tx.body.mint_and_transfer.royalties_qty =
                U2BE(msg->parameter, PARAMETER_LENGTH - 2);

            context->next_param = ROYALTIES_ADDRESS;
            if (context->tx.body.mint_and_transfer.royalties_qty == 0) {
                context->next_param = SIGNATURES_QTY;
            }
            break;
        case ROYALTIES_ADDRESS:
            context->next_param = ROYALTIES_VALUE;
            break;
        case ROYALTIES_VALUE:
            if (context->tx.body.mint_and_transfer.royalties_qty ==
                1) {  // Fix royalties values to only 1, the last one.
                context->tx.body.mint_and_transfer.royalties =
                    U2BE(msg->parameter, PARAMETER_LENGTH - 2);
            }

            context->tx.body.mint_and_transfer.royalties_qty--;
            if (context->tx.body.mint_and_transfer.royalties_qty != 0) {
                context->next_param = ROYALTIES_ADDRESS;
            } else {
                context->next_param = SIGNATURES_QTY;
            }
            break;

        // Signature logic is here, but it is not being used as we don't need to show them.
        case SIGNATURES_QTY:
            context->tx.body.mint_and_transfer.signatures_qty =
                U2BE(msg->parameter, PARAMETER_LENGTH - 2);
            context->tx.body.mint_and_transfer.signatures_counter =
                context->tx.body.mint_and_transfer.signatures_qty;

            context->next_param = SIGNATURE_OFFSET;
            if (context->tx.body.mint_and_transfer.signatures_qty == 0) {
                context->next_param = UNEXPECTED_PARAMETER;
            }
            break;
        case SIGNATURE_OFFSET:
            context->tx.body.mint_and_transfer.signatures_counter--;
            if (context->tx.body.mint_and_transfer.signatures_counter == 0) {
                context->next_param = SIGNATURE_LENGTH;
            }
            break;
        case SIGNATURE_LENGTH:
            context->tx.body.mint_and_transfer.signature_length =
                get_counter(U2BE(msg->parameter, PARAMETER_LENGTH - 2));

            context->next_param = SIGNATURE;
            break;
        case SIGNATURE:
            // Signature logic is here, but it is not being used. We won't copy anything
            // if(context->signature_length == 1){
            //     copy_parameter(context->signature, msg->parameter, sizeof(context->signature));
            // }

            context->tx.body.mint_and_transfer.signature_length--;
            if (context->tx.body.mint_and_transfer.signature_length == 0) {
                context->tx.body.mint_and_transfer.signatures_qty--;
                if (context->tx.body.mint_and_transfer.signatures_qty != 0) {
                    context->next_param = SIGNATURE_LENGTH;
                } else {
                    context->next_param = UNEXPECTED_PARAMETER;
                }
            }
            break;

        // Keep this
        default:
            PRINTF("Param not supported: %d\n", context->next_param);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}

static void handle_cancel(ethPluginProvideParameter_t *msg, context_t *context) {
    switch (context->next_param) {
        case BODY_OFFSET:
            context->next_param = MAKER;
            break;
        default:
            parse_order(msg, context, &(context->tx.body.cancel), ERROR_ORDER);
            break;
    }
}

static void handle_match_orders(ethPluginProvideParameter_t *msg, context_t *context) {
    if (context->tx.body.match_orders.order_side == NO_ORDER) {
        switch (context->next_param) {
            case LEFT_ORDER_OFFSET:
                context->next_param = LEFT_SIGNATURE_OFFSET;
                break;
            case LEFT_SIGNATURE_OFFSET:
                context->next_param = RIGHT_ORDER_OFFSET;
                break;
            case RIGHT_ORDER_OFFSET:
                context->next_param = RIGHT_SIGNATURE_OFFSET;
                break;
            case RIGHT_SIGNATURE_OFFSET:
                context->tx.body.match_orders.order_side = LEFT_ORDER;
                context->next_param = MAKER;
                break;
            default:
                PRINTF("Param not supported: %d\n", context->next_param);
                msg->result = ETH_PLUGIN_RESULT_ERROR;
                break;
        }
        return;
    }

    if (context->tx.body.match_orders.order_side == LEFT_ORDER) {
        parse_order(msg, context, &(context->tx.body.match_orders.left), RIGHT_ORDER);
        return;
    }

    if (context->tx.body.match_orders.order_side == RIGHT_ORDER) {
        parse_order(msg, context, &(context->tx.body.match_orders.right), ERROR_ORDER);
        return;
    }

    if (context->tx.body.match_orders.order_side == ERROR_ORDER) {
        PRINTF("Param not supported: %d\n", context->next_param);
        msg->result = ETH_PLUGIN_RESULT_ERROR;
    }
}

void handle_provide_parameter(void *parameters) {
    ethPluginProvideParameter_t *msg = (ethPluginProvideParameter_t *) parameters;
    context_t *context = (context_t *) msg->pluginContext;
    // We use `%.*H`: it's a utility function to print bytes. You first give
    // the number of bytes you wish to print (in this case, `PARAMETER_LENGTH`) and then
    // the address (here `msg->parameter`).
    PRINTF("plugin provide parameter: offset %d\nBytes: %.*H\n",
           msg->parameterOffset,
           PARAMETER_LENGTH,
           msg->parameter);

    msg->result = ETH_PLUGIN_RESULT_OK;

    switch (context->selectorIndex) {
        case MINT_AND_TRANSFER:
            handle_mint_and_transfer(msg, context);
            break;
        case CANCEL:
            handle_cancel(msg, context);
            break;
        case MATCH_ORDERS:
            handle_match_orders(msg, context);
            break;
        case TRANSFER_FROM_OR_MINT:
            handle_transfer_from_or_mint(msg, context);
            break;
        case SET_APPROVED_FOR_ALL:
            handle_set_approved_for_all(msg, context);
            break;
        case CREATE_TOKEN:
            handle_create_token(msg, context);
            break;
        case ERC721_RARIBLE_INIT:
            handle_erc721_rarible_init(msg, context);
            break;
        default:
            PRINTF("Selector Index not supported: %d\n", context->selectorIndex);
            msg->result = ETH_PLUGIN_RESULT_ERROR;
            break;
    }
}