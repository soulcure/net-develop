/*****************************
 * author@xiesong
 * songtzu@126.com
 * 4.12.2016
 */

#ifndef PDUBASE_H
#define PDUBASE_H

#include <memory>   //shared_ptr




class PDUBase
{
public:
    PDUBase();

    /********************************************
     * index 0, [0,4)
     * start flag.
     */
    const static int startflag = 123456789 ;

    /********************************************
     * index 1, [4,8)
     * terminal_id is user_id, if user loged in.
     * if not login, given  him a random int.
     *
     */
    int terminal_token ;

    /*********************************************
     * index 2, [8,12)
     * this stand for command id.
     * meet with protobuf.
     */
    int command_id ;

    /*********************************************
     * index 3, [12,16)
     * seq_id, app (also in other connect in model,
     * such like php, used to route back to real source.)
     * in app, seq_id is used to dispatch the event to different source.
     */
    int seq_id;

    /*********************************************
     * index 4, [16,20)
     * protobuf length. in binary format.
     */
    int length ;

    /*********************************************
     * the buffer holder for protobuf.
     * this use shared_ptr to manage memory.
     */
    std::shared_ptr<char> body ;


};

#endif // PDUBASE_H
