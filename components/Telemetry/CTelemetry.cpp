#include "CTelemetry.h"
#include "CTimer.h"
#include "Setup.h"
#include "CUSB.h"


std::vector<CTelemetry*> CTelemetry::pool;

CTelemetry* CTelemetry::Rent() {
  if (pool.empty()) {
    size_t capacity = max(pool.size(), initialCapacity);
    if (capacity < maxCapacity) {
      size_t newCapacity = std::min(capacity * 2, maxCapacity);
      for (size_t i = capacity; i < newCapacity; ++i)
        pool.push_back(new CTelemetry());

      capacity = newCapacity;
    } else {
      // At max capacity: allocate temporarily (not returned to pool)
      return new CTelemetry();
    }
  }

  // Reuse an existing one
  CTelemetry* item = pool.back();
  pool.pop_back();
  return item;
}

void CTelemetry::Return(CTelemetry* item) {
  if (pool.size() < maxCapacity) {
    item->reset(); 
    pool.push_back(item);
  } else {
    // Pool full → discard this one
    delete item;
  }
}

void CTelemetry::init() {
  pool.reserve(initialCapacity);
  for (size_t i = 0; i < initialCapacity; ++i)
    pool.push_back(new CTelemetry());
}

void CTelemetry::log(TeleGroup group, uint16_t ID, float value) {
    log(group, 0, ID, value);
}

void CTelemetry::log(TeleGroup group, uint8_t subGroup, uint16_t ID, float value) {
  double timestamp = CTimer::time() * CTimer::getSecondsPerTick();

  CTelemetry* telemetry = CTelemetry::Rent();
  telemetry->timeStamp = timestamp;
  telemetry->group = group;
  telemetry->subGroup = subGroup;
  telemetry->ID = ID;
  telemetry->value = value;

  USB.buffer(telemetry);
}

void CTelemetry::writeSerial(bool includeFrameMarkers) {
  if (includeFrameMarkers) USB.write(frameStart);
    
  USB.write(timeStamp);
  USB.write(static_cast<uint8_t>(group));
  USB.write(static_cast<uint8_t>(subGroup));
  USB.write(ID);
  USB.write(value);

  if (includeFrameMarkers) USB.write(frameEnd);
}

void CTelemetry::reset() {
  timeStamp = 0.0;
  group     = TeleGroup::PROGRAM;
  ID        = 0;
  value     = 0.0f;
}

std::deque<CTelemetry*>& CTelemetry::getAllTelemetries() {
  static std::deque<CTelemetry*> all;
  return all;
}

void CTelemetry::_register(CTelemetry* tele) {
  USB.printf("Registering %p (%s)\n", tele, tele->getName());
  getAllTelemetries().push_back(tele);
}


void CTelemetry::logAll() {
    for (CTelemetry* telemetry : getAllTelemetries()) {
        telemetry->value = telemetry->getValue();
        telemetry->writeSerial(true);
    }
}