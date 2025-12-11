#pragma once
#include "CSerialWrapper.h"
#include <functional>

class SerialStateMachine {
public:
    enum class State
    {
        DISCONNECTED,
        CONNECTED_WAITING,
        READY
    };

    SerialStateMachine() : state(State::DISCONNECTED), firstCall(true) {}

    void update(bool connected, bool hasData, bool testMode,
              bool &handshakeComplete, CSerialWrapper::ModeType &mode,
              std::function<void()> beginFn);

    State getState() const { return state; }
    bool getFirstCall() const { return firstCall; }
    void setFirstCall(bool value) { firstCall = value; }


    static SerialStateMachine& create();

private:
    State state;
    bool firstCall;

    // Handles disconnect if needed; returns true if a disconnect was processed
    bool handleDisconnection(bool connected, bool &handshakeComplete, CSerialWrapper::ModeType &mode);
};
