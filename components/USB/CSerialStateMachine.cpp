#include "CSerialStateMachine.h"
#include "PinHelpers.h"

SerialStateMachine& SerialStateMachine::create() {
    static std::vector<SerialStateMachine> instances;

    instances.emplace_back();
    
    return instances.back();
}


bool SerialStateMachine::handleDisconnection(bool connected, bool &handshakeComplete, CSerialWrapper::ModeType &mode)
{
    if (!connected) {
        Pins::flash(3);
        handshakeComplete = false;
        mode = CSerialWrapper::ModeType::UNSET;
        state = State::DISCONNECTED;
        return true;
    }
    return false;
}

void SerialStateMachine::tick(bool connected, bool hasData, bool testMode,
                              bool &handshakeComplete, CSerialWrapper::ModeType &mode,
                              std::function<void()> begin)
{
    switch (state)
        {
        case State::DISCONNECTED:
            if (connected) {
                handshakeComplete = false;
                mode = CSerialWrapper::ModeType::UNSET;
                state = State::CONNECTED_WAITING;
                firstCall = true;
            }
            break;

        case State::CONNECTED_WAITING:
            if (handleDisconnection(connected, handshakeComplete, mode)) break;

            if (hasData && (!testMode || firstCall)) {
                begin();
                state = State::READY;
            }
            break;

        case State::READY:
            if (handleDisconnection(connected, handshakeComplete, mode)) break;
            break;
    }
}
