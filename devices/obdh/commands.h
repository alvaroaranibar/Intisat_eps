#ifndef COMMANDS_H_
#define COMMANDS_H_

/**
 * \brief INTISAT Protocol command types.
 */
typedef enum
{
    REQ_FULL_TLM     = 0x10,    /* Requests the complete EPS data structure. */
    REQ_BEACON_TLM   = 0x11,    /* Requests basic data for the Beacon. */
    REQ_SINGLE_PARAM = 0x12,    /* Reads a specific ID from the table. */
    WR_SINGLE_PARAM  = 0x20,    /* Writes a value to an ID. */
    ANOMALY     = 0x40          /* Reads/sends the emergency code */
} obc_command_e;

#endif /* COMMANDS_H_ */
