#pragma once

#include "eth_internals.h"
#include "eth_plugin_interface.h"
#include <string.h>

// Number of selectors defined in this plugin. Should match the enum `selector_t`.
#define NUM_SELECTORS 7

// Name of the plugin.
#define PLUGIN_NAME "Rarible"

// Used to parse match orders selector
#define LEFT_ORDER  -1
#define RIGHT_ORDER 1
#define NO_ORDER    0
#define ERROR_ORDER -3

// Used to parse match orders selector
#define IS_TAKER -1
#define IS_MAKER 1

#define NAME_MAX_LEN   PARAMETER_LENGTH
#define SYMBOL_MAX_LEN PARAMETER_LENGTH

#define ROYALTY_MAX_VALUE 10000

// Enumeration of the different selectors possible.
// Should follow the exact same order as the array declared in main.c
typedef enum {
    MINT_AND_TRANSFER = 0,
    CANCEL,
    MATCH_ORDERS,
    TRANSFER_FROM_OR_MINT,
    SET_APPROVED_FOR_ALL,
    CREATE_TOKEN,
    ERC721_RARIBLE_INIT,
} selector_t;

// Enumeration used to parse the smart contract data.
typedef enum {
    // mint and transfer
    BODY_OFFSET = 0,
    BENEFICIARY,
    SELLER,
    ID,
    URI_OFFSET,
    CREATORS_OFFSET,
    ROYALTIES_OFFSET,
    SIGNATURES_OFFSET,
    URI_CHUNK_OFFSET,
    WAIT_UNTIL_CREATORS,
    CREATORS_QTY,
    CREATORS_ADDRESS,
    CREATORS_VALUE,
    ROYALTIES_QTY,
    ROYALTIES_ADDRESS,
    ROYALTIES_VALUE,
    SIGNATURE,
    SIGNATURES_QTY,
    SIGNATURE_OFFSET,
    SIGNATURE_LENGTH,

    // orders (multi purpose)
    MAKER,
    MAKE_ASSET_OFFSET,
    TAKER,
    TAKE_ASSET_OFFSET,
    SALT,
    START,
    END,
    DATA_TYPE,
    DATA_OFFSET,
    DATA,
    DATA_LENGTH,

    MAKE_ASSET,
    TAKE_ASSET,

    ASSET_TYPE_OFFSET,
    ASSET_VALUE,
    ASSET_CLASS,
    ASSET_DATA_OFFSET,
    ASSET_DATA_LENGTH,
    ASSET_DATA,

    // match order
    LEFT_ORDER_OFFSET,
    LEFT_SIGNATURE_OFFSET,
    RIGHT_ORDER_OFFSET,
    RIGHT_SIGNATURE_OFFSET,

    // set approval for all
    TO,
    IS_APPROVED,

    // create token
    // erc721 rarible init
    NAME_OFFSET,
    SYMBOL_OFFSET,
    OPERATORS_OFFSET,
    BASE_URI_OFFSET,
    CONTRACT_URI_OFFSET,
    NAME_LENGTH,
    NAME,
    SYMBOL_LENGTH,
    SYMBOL,
    BASE_URI_LENGTH,
    BASE_URI,
    CONTRACT_URI_LENGTH,
    CONTRACT_URI,
    OPERATORS_QTY,
    OPERATORS_ADDRESS,

    TRANSFER_PROXY_ADDRESS,
    LAZY_TRANSFER_PROXY_ADDRESS,

    UNEXPECTED_PARAMETER,
} parameter;

extern const uint8_t *const RARIBLE_SELECTORS[NUM_SELECTORS];

typedef struct {
    uint8_t address[ADDRESS_LENGTH];
} address_t;

typedef struct {
    uint8_t value[INT256_LENGTH];
} token_id_t;

typedef struct {
    address_t maker;
    address_t taker;
} order_t;

typedef struct {
    u_int16_t len;
    u_int8_t text[NAME_MAX_LEN + 1];
} name_t;

typedef struct {
    u_int16_t len;
    u_int8_t text[SYMBOL_MAX_LEN + 1];
} symbol_t;

typedef struct {
    union {
        struct {
            // For display.
            token_id_t id;
            address_t beneficiary;
            bool creator_found;
            address_t creator;
            uint16_t royalties;
            // uint8_t signature[PARAMETER_LENGTH];

            // For parsing data.
            uint16_t uri_length;
            uint16_t creators_qty;
            uint16_t royalties_qty;
            uint16_t signatures_qty;
            uint16_t signatures_counter;
            uint16_t signature_length;

        } mint_and_transfer;

        struct {
            // For display.
            token_id_t id;
            address_t beneficiary;
            address_t seller;
            address_t creator;
            uint16_t royalties;
            // uint8_t signature[PARAMETER_LENGTH];

            // For parsing data.
            uint16_t uri_length;
            uint16_t creators_qty;
            uint16_t royalties_qty;
            uint16_t signatures_qty;
            uint16_t signatures_counter;
            uint16_t signature_length;

        } transfer_from_or_mint;

        order_t cancel;

        struct {
            // For display.
            order_t left;
            order_t right;

            // For parsing data.
            int8_t order_side;
        } match_orders;

        struct {
            // For display.
            address_t to;
            u_int8_t isApproved;

        } set_approval_for_all;

        struct {
            // For display.
            bool operator_found;
            address_t operator;
            name_t name;
            symbol_t symbol;

        } create_token;

        struct {
            // For display.
            address_t transfer_proxy;
            address_t lazy_transfer_proxy;
            name_t name;
            symbol_t symbol;

        } erc721_rarible_init;

    } body;
} rarible_tx_t;

// Shared global memory with Ethereum app. Must be at most 5 * 32 bytes.
typedef struct {
    // For both parsing and display.
    selector_t selectorIndex;

    // For parsing data.
    uint8_t next_param;  // Set to be the next param we expect to parse.
    uint8_t sub_param;   // Set to be the next param we expect to parse.
    uint16_t max_counter;
    uint16_t counter;

    // Tx related context
    rarible_tx_t tx;
} context_t;

// Piece of code that will check that the above structure is not bigger than 5 * 32. Do not remove
// this check.
_Static_assert(sizeof(context_t) <= 5 * 32, "Structure of parameters too big.");

void handle_provide_parameter(void *parameters);
void handle_query_contract_ui(void *parameters);
void handle_init_contract(void *parameters);
void handle_finalize(void *parameters);
void handle_provide_token(void *parameters);
void handle_query_contract_id(void *parameters);