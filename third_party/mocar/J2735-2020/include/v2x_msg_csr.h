#ifndef __V2X_MSG_CSR_H
#define __V2X_MSG_CSR_H

#include <v2x_msg_common.h>


/*************************************************************************/
/**************************** CommonSafetyRequest **************************/
/*************************************************************************/


typedef long mde_requestedItem_t;

typedef struct v2x_msg_csr
{
    /*
     *  element: MsgCount 
     *  desc: Provide a sequence number within a stream of messages with the same V2XmsgID and from the same sender.
     *  range: 0...127
     */
    uint8_t                msg_count;

    /*
     *  element: TemporaryID 
     *  desc: This is the 4 octet random device identifier, called the TemporaryID.
     *  size: 4 byte
     */    
    char                    temp_id[4];

    /*
     *  element: MinuteOfTheYear 
     *  desc: Time, unit:minute
     *  range: 0..527040
     */
    uint16_t                time_stamp;

    int						n_requestedItem;
	mde_requestedItem_t		requestedItem[32];
	

} v2x_msg_csr_t;

#endif
