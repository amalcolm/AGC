#include "HWforState.h"
#include <deque>

struct DBGflags {
  StateType state;

  bool started = false;

  double startTime = -1.0;
  double markTime = -1.0;
  bool toggle = false;
};
DBGflags& getDBGflags(StateType state);

bool setKnownConfig(int cfg) {
  static constexpr std::tuple<int, int, int, int, int> knownConfigs[] = {
    {61, 59, 190, 128, 5},    {60, 58,  83, 128, 5}, 
    {40, 55, 128, 128, 0},
  };
  static constexpr int numKnownConfigs = sizeof(knownConfigs) / sizeof(knownConfigs[0]);

  if (cfg < 0 || cfg >= numKnownConfigs) return false;

  auto [top, bot, mid, offset, gain] = knownConfigs[cfg];

    HW->Stage1.top     .setLevel(top   );
    HW->Stage1.bot     .setLevel(bot   );
    HW->Stage1.mid     .setLevel(mid   );
    HW->OpAmp.offsetPot.setLevel(offset);
    HW->OpAmp.gainPot  .setLevel(gain  );
    delayMicroseconds(10);
    return true;
}



void HWforState::HWflags::dbg() {

  auto& [_, started, startTime, markTime, toggle] = getDBGflags(HW->state);

  if (startTime < 0.0) { // i.e. not set yet
    if (HW->OpAmp.inZone) 
      startTime = Timer.getConnectTime();
    else
      return;
  }

  double now = Timer.getConnectTime() - startTime;

  if (now < 1.0) return; // only start toggling after a second to allow settling



  if (!started) { started = true; markTime = now;

    setKnownConfig(0);

    offsetsChanged = true;
    holdStage1 = true;
    holdStage2 = true;

    delayMicroseconds(10);
  }

  if (now - markTime >= 2.0) { // toggle every 2 seconds
    markTime = now;
    toggle = !toggle;

    if (toggle) 
      setKnownConfig(1);
    else 
      setKnownConfig(0);
  }
}

  



// DBGflags management

std::deque<DBGflags> s_dbgFlags;

DBGflags& getDBGflags(StateType state) {
  for (auto& flags : s_dbgFlags) {
    if (flags.state == state) return flags;
  }
  s_dbgFlags.push_back({state});
  return s_dbgFlags.back();
}