#include "pdu_base.h"

PDUBase::PDUBase()
{
    terminal_token = 0;
    this->length = 0;
    this->seq_id = 0;
    this->command_id = 0 ;
}

