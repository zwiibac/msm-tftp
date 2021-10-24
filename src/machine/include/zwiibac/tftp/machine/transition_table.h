#pragma once

// back-end
#include <boost/msm/back/state_machine.hpp>
//front-end
#include <boost/msm/front/state_machine_def.hpp>
// functors
#include <boost/msm/front/functor_row.hpp>
#include <boost/msm/front/euml/common.hpp>


#include <zwiibac/tftp/action/form_data_packet.h>
#include <zwiibac/tftp/action/init_serve_read_request.h>
#include <zwiibac/tftp/action/time_out.h>
#include <zwiibac/tftp/action/reset.h>
#include <zwiibac/tftp/action/reset_time_out.h>
#include <zwiibac/tftp/action/form_error_packet.h>
#include <zwiibac/tftp/action/form_protocol_error_packet.h>
#include <zwiibac/tftp/action/init_serve_write_request.h>
#include <zwiibac/tftp/action/form_ack_packet.h>
#include <zwiibac/tftp/action/write_data.h>
#include <zwiibac/tftp/action/form_oack_packet.h>

#include <zwiibac/tftp/event/accepting_failed.h>
#include <zwiibac/tftp/event/packet_received.h>
#include <zwiibac/tftp/event/packet_sent.h>
#include <zwiibac/tftp/event/request_received.h>
#include <zwiibac/tftp/event/timed_out.h>
#include <zwiibac/tftp/event/restart.h>

#include <zwiibac/tftp/guard/can_retry.h>
#include <zwiibac/tftp/guard/has_ack_received.h>
#include <zwiibac/tftp/guard/has_error.h>
#include <zwiibac/tftp/guard/has_more_data_to_send.h>
#include <zwiibac/tftp/guard/has_read_request_received.h>
#include <zwiibac/tftp/guard/is_old_ack.h>
#include <zwiibac/tftp/guard/logical_operators.h>
#include <zwiibac/tftp/guard/from_agreed_endpoint.h>
#include <zwiibac/tftp/guard/has_write_request_received.h>
#include <zwiibac/tftp/guard/has_data_received.h>
#include <zwiibac/tftp/guard/is_old_data.h>
#include <zwiibac/tftp/guard/has_options.h>
#include <zwiibac/tftp/guard/has_err_received.h>

#include <zwiibac/tftp/state/accepting.h>
#include <zwiibac/tftp/state/read_request_received.h>
#include <zwiibac/tftp/state/sending_data.h>
#include <zwiibac/tftp/state/resetted.h>
#include <zwiibac/tftp/state/data_read.h>
#include <zwiibac/tftp/state/sending_error.h>
#include <zwiibac/tftp/state/waiting_for_ack.h>
#include <zwiibac/tftp/state/write_request_received.h>
#include <zwiibac/tftp/state/sending_ack.h>
#include <zwiibac/tftp/state/waiting_for_data.h>
#include <zwiibac/tftp/state/data_written.h>

namespace zwiibac {
namespace tftp {

class CommonTransitionTable 
{
private:
    using none = boost::msm::front::none;

    template<class Source,class Event,class Target,class Action, class Guard>
    using Row = boost::msm::front::Row<Source, Event, Target, Action, Guard>;
public:
    using type = boost::mpl::vector<
        Row<SendingError,         PacketSent,      Resetted,             Reset,                   none>,
        Row<Resetted,             none,            Accepting,            none,                    none>,
        Row<Accepting,            AcceptingFailed, Resetted,             Reset,                   HasError>
    >;
};

using CommonTransitionTable_t = CommonTransitionTable::type;

class WriteRequestTransitionTable 
{
private:
    using none = boost::msm::front::none;

    template<class Source,class Event,class Target,class Action, class Guard>
    using Row = boost::msm::front::Row<Source, Event, Target, Action, Guard>;
public:
    using type = boost::mpl::vector<
        Row<Accepting,            RequestReceived, WriteRequestReceived, InitServeWriteRequest,   And<Not<HasError>, HasWriteRequestReceived>>,
        Row<WriteRequestReceived, none,            SendingAck,           FormAckPacket,           Not<HasError>>,
        Row<WriteRequestReceived, none,            SendingError,         FormErrorPacket,         HasError>,
        Row<SendingAck,           PacketSent,      WaitingForData,       none,                    none>,
        Row<WaitingForData,       TimedOut,        SendingAck,           TimeOut,                 CanRetry>,
        Row<WaitingForData,       TimedOut,        Resetted,             Reset,                   Not<CanRetry>>,        
        Row<WaitingForData,       PacketReceived,  SendingError,         FormProtocolErrorPacket, And<FromAgreedEndpoint, Not<HasDataReceived>>>,
        Row<WaitingForData,       PacketReceived,  WaitingForAck,        none,                    Or<Not<FromAgreedEndpoint>, And<HasDataReceived, IsOldData>>>,
        Row<WaitingForData,       PacketReceived,  DataWritten,          WriteData,               And<FromAgreedEndpoint, HasDataReceived, Not<IsOldData>>>,
        Row<DataWritten,          none,            SendingAck,           FormAckPacket,           Not<HasError>>,
        Row<DataWritten,          none,            SendingError,         FormErrorPacket,         HasError>  
    >;
};

using WriteRequestTransitionTable_t = WriteRequestTransitionTable::type;

class ReadRequestTransitionTable 
{
private:
    using none = boost::msm::front::none;

    template<class Source,class Event,class Target,class Action, class Guard>
    using Row = boost::msm::front::Row<Source, Event, Target, Action, Guard>;

    using HasFinalAckReceived = And<HasAckReceived, Not<IsOldAck>, Not<HasMoreToSend>>;
    using HasIntermediateAckReceived = And<HasAckReceived, Not<IsOldAck>, HasMoreToSend>;
    using HasOldAckReceived = And<HasAckReceived, IsOldAck>;
public:
    using type = boost::mpl::vector<
        Row<Accepting,           RequestReceived, ReadRequestReceived,  InitServeReadRequest,    And<Not<HasError>, HasReadRequestReceived>>,
        Row<ReadRequestReceived, none,            DataRead,             FormDataPacket,          Not<Or<HasError, HasOptions>>>,
        Row<ReadRequestReceived, none,            DataRead,             FormOackPacket,          HasOptions>,
        Row<ReadRequestReceived, none,            SendingError,         FormErrorPacket,         HasError>,
        Row<DataRead,            none,            SendingData,          ResetTimeOut,            Not<HasError>>,
        Row<DataRead,            none,            SendingError,         FormErrorPacket,         HasError>,
        Row<SendingData,         PacketSent,      WaitingForAck,        none,                    none>,
        Row<WaitingForAck,       TimedOut,        SendingData,          TimeOut,                 CanRetry>,
        Row<WaitingForAck,       TimedOut,        Resetted,             Reset,                   Not<CanRetry>>,        
        Row<WaitingForAck,       PacketReceived,  SendingError,         FormProtocolErrorPacket, And<FromAgreedEndpoint, Not<HasAckReceived>>>,
        Row<WaitingForAck,       PacketReceived,  WaitingForAck,        none,                    Or<Not<FromAgreedEndpoint>, HasOldAckReceived>>,
        Row<WaitingForAck,       PacketReceived,  DataRead,             FormDataPacket,          And<FromAgreedEndpoint, HasIntermediateAckReceived>>,
        Row<WaitingForAck,       PacketReceived,  Resetted,             Reset,                   Or<HasErrReceived, And<FromAgreedEndpoint, HasFinalAckReceived>>> 
    >;
};

using ReadRequestTransitionTable_t = ReadRequestTransitionTable::type;


} // end namespace tftp
} // end namespace zwiibac