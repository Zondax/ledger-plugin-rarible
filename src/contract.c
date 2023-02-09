#include "rarible_plugin.h"

// List of selectors supported by this plugin.
static const uint8_t MINT_AND_TRANSFER_SELECTOR[SELECTOR_SIZE] = {0x22, 0xa7, 0x75, 0xb6};
static const uint8_t CANCEL_SELECTOR[SELECTOR_SIZE] = {0xe2, 0x86, 0x4f, 0xe3};
static const uint8_t MATCH_ORDERS_SELECTOR[SELECTOR_SIZE] = {0xe9, 0x9a, 0x3f, 0x80};
static const uint8_t TRANSFER_FROM_OR_MINT_SELECTOR[SELECTOR_SIZE] = {0x83, 0x2f, 0xbb, 0x29};
static const uint8_t SET_APPROVAL_FOR_ALL_SELECTOR[SELECTOR_SIZE] = {0xa2, 0x2c, 0xb4, 0x65};
static const uint8_t CREATE_TOKEN_SELECTOR[SELECTOR_SIZE] = {0x27, 0x05, 0x0d, 0x1f};
static const uint8_t ERC721_RARIBLE_INIT_SELECTOR[SELECTOR_SIZE] = {0x3d, 0xb3, 0x97, 0xc6};

// Array of all the different boilerplate selectors. Make sure this follows the same order as
// the enum defined in `rarible_plugin.h`
const uint8_t *const RARIBLE_SELECTORS[NUM_SELECTORS] = {
        MINT_AND_TRANSFER_SELECTOR,
        CANCEL_SELECTOR,
        MATCH_ORDERS_SELECTOR,
        TRANSFER_FROM_OR_MINT_SELECTOR,
        SET_APPROVAL_FOR_ALL_SELECTOR,
        CREATE_TOKEN_SELECTOR,
        ERC721_RARIBLE_INIT_SELECTOR,
};