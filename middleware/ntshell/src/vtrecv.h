/**
 * @file vtrecv.h
 * @brief VT100互換の受信用パーサの定義。
 * @details
 * An implementation of Paul Williams' DEC compatible state machine parser
 * This code is in the public domain.
 *
 * @author Joshua Haberman <joshua@reverberate.org>
 * @author Shinichiro Nakamura : Modified for Natural Tiny Shell (NT-Shell)
 */

#ifndef VTRECV_H
#define VTRECV_H

#define MAX_INTERMEDIATE_CHARS 2
#define ACTION(state_change) (vtrecv_action_t)(state_change & 0x0F)
#define STATE(state_change)  (vtrecv_state_t)(state_change >> 4)

typedef enum {
    VTRECV_STATE_ANYWHERE = 0,
    VTRECV_STATE_CSI_ENTRY = 1,
    VTRECV_STATE_CSI_IGNORE = 2,
    VTRECV_STATE_CSI_INTERMEDIATE = 3,
    VTRECV_STATE_CSI_PARAM = 4,
    VTRECV_STATE_DCS_ENTRY = 5,
    VTRECV_STATE_DCS_IGNORE = 6,
    VTRECV_STATE_DCS_INTERMEDIATE = 7,
    VTRECV_STATE_DCS_PARAM = 8,
    VTRECV_STATE_DCS_PASSTHROUGH = 9,
    VTRECV_STATE_ESCAPE = 10,
    VTRECV_STATE_ESCAPE_INTERMEDIATE = 11,
    VTRECV_STATE_GROUND = 12,
    VTRECV_STATE_OSC_STRING = 13,
    VTRECV_STATE_SOS_PM_APC_STRING = 14,
} vtrecv_state_t;

typedef enum {
    VTRECV_ACTION_CLEAR = 1,
    VTRECV_ACTION_COLLECT = 2,
    VTRECV_ACTION_CSI_DISPATCH = 3,
    VTRECV_ACTION_ESC_DISPATCH = 4,
    VTRECV_ACTION_EXECUTE = 5,
    VTRECV_ACTION_HOOK = 6,
    VTRECV_ACTION_IGNORE = 7,
    VTRECV_ACTION_OSC_END = 8,
    VTRECV_ACTION_OSC_PUT = 9,
    VTRECV_ACTION_OSC_START = 10,
    VTRECV_ACTION_PARAM = 11,
    VTRECV_ACTION_PRINT = 12,
    VTRECV_ACTION_PUT = 13,
    VTRECV_ACTION_UNHOOK = 14,
} vtrecv_action_t;

typedef unsigned char state_change_t;

struct vtrecv;

typedef void (*vtrecv_callback_t)(struct vtrecv*, vtrecv_action_t, unsigned char);

typedef struct vtrecv {
    vtrecv_state_t    state;
    vtrecv_callback_t cb;
    unsigned char      intermediate_chars[MAX_INTERMEDIATE_CHARS+1];
    char               ignore_flagged;
    int                params[16];
    int                num_params;
    void*              user_data;
} vtrecv_t;

#ifdef __cplusplus
extern "C" {
#endif

void vtrecv_init(vtrecv_t *parser, vtrecv_callback_t cb);
void vtrecv_execute(vtrecv_t *parser, unsigned char *data, int len);

#ifdef __cplusplus
}
#endif

#endif

