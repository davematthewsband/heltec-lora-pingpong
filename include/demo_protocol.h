#pragma once
#include <stddef.h>
#include <stdint.h>

namespace demo {
inline bool due(uint32_t now, uint32_t deadline) { return int32_t(now - deadline) >= 0; }
constexpr size_t kPacketSize = 32;
enum Type : uint8_t { Status = 1, Toggle = 2, Sleep = 3, AckToggle = 4, AckSleep = 5 };
struct Packet {
  Type type = Status;
  uint64_t source = 0, target = 0;
  uint32_t session = 0, sequence = 0;
  uint16_t millivolts = 0;
  bool light = false;
};
inline void put(uint8_t *p, uint64_t value, unsigned bytes) {
  for (unsigned i = 0; i < bytes; ++i) p[i] = uint8_t(value >> (8 * i));
}
inline uint64_t get(const uint8_t *p, unsigned bytes) {
  uint64_t value = 0;
  for (unsigned i = 0; i < bytes; ++i) value |= uint64_t(p[i]) << (8 * i);
  return value;
}
inline void encode(const Packet &p, uint8_t *out) {
  out[0] = 'F'; out[1] = 'D'; out[2] = 1; out[3] = p.type;
  put(out + 4, p.source, 8); put(out + 12, p.target, 8);
  put(out + 20, p.session, 4); put(out + 24, p.sequence, 4);
  put(out + 28, p.millivolts, 2); out[30] = p.light; out[31] = 0;
}
inline bool decode(const uint8_t *in, size_t size, Packet &p) {
  if (size != kPacketSize || in[0] != 'F' || in[1] != 'D' || in[2] != 1 ||
      in[3] < Status || in[3] > AckSleep || in[30] > 1 || in[31] != 0) return false;
  p.type = Type(in[3]); p.source = get(in + 4, 8); p.target = get(in + 12, 8);
  p.session = uint32_t(get(in + 20, 4)); p.sequence = uint32_t(get(in + 24, 4));
  p.millivolts = uint16_t(get(in + 28, 2)); p.light = in[30];
  return p.source != 0 && p.source <= 0xffffffffffffULL &&
         p.target <= 0xffffffffffffULL && (p.type == Status || p.target != 0);
}
inline int batteryPercent(uint16_t mv) {
  if (mv < 2500 || mv > 4500) return -1; // unavailable, not a battery detector
  if (mv <= 3200) return 0;
  if (mv >= 4200) return 100;
  return (mv - 3200) / 10;
}

enum Press { None, Short, Long };
class Button {
  bool raw_ = false, stable_ = false, armed_ = false, long_ = false;
  uint32_t changed_ = 0, pressed_ = 0;
 public:
  // Suppress boot/wake press until the button has been released and debounced.
  Press update(bool down, uint32_t now) {
    if (down != raw_) { raw_ = down; changed_ = now; }
    if (uint32_t(now - changed_) < 35) return None;
    if (!armed_) { if (!down) { armed_ = true; stable_ = false; } return None; }
    if (down != stable_) {
      stable_ = down;
      if (down) { pressed_ = now; long_ = false; }
      else if (!long_) return Short;
    }
    if (stable_ && !long_ && uint32_t(now - pressed_) >= 2000) {
      long_ = true; return Long;
    }
    return None;
  }
};

// Hardware-independent two-device command state. Transport sends pending commands
// at most three times; a receiver remains awake to acknowledge duplicate sleeps.
class Node {
 public:
  uint64_t id, peer = 0;
  uint32_t session, sequence = 0, lastSeen = 0;
  uint16_t battery = 0, peerBattery = 0;
  bool light = false, peerLight = false, pending = false, ackReady = false;
  bool sleeping = false, confirmed = false, failed = false;
  uint32_t sleepAt = 0, retryAt = 0;
  unsigned attempts = 0;
  Packet command, ack;
 private:
  bool haveCommand_ = false;
  uint32_t remoteSession_ = 0, remoteSequence_ = 0;
 public:
  Node(uint64_t identity, uint32_t boot) : id(identity), session(boot) {}
  Packet packet(Type type) const {
    Packet p; p.type = type; p.source = id; p.target = peer;
    p.session = session; p.millivolts = battery; p.light = light; return p;
  }
  bool request(Type type, uint32_t now) {
    if (sleeping || (pending && type != Sleep)) return false;
    if (!peer) {
      failed = true;
      if (type == Sleep) { sleeping = true; light = false; sleepAt = now + 1500; }
      return false;
    }
    command = packet(type); command.sequence = ++sequence;
    pending = true; attempts = 0; retryAt = now;
    confirmed = failed = false; return true;
  }
  void receive(const Packet &p, uint32_t now) {
    if (!p.source || p.source == id || (p.target && p.target != id)) return;
    if (peer && peer != p.source) return;
    // Only discovery/status can introduce a peer. No broadcast commands.
    if (!peer && p.type != Status) return;
    peer = p.source; peerBattery = p.millivolts; peerLight = p.light; lastSeen = now;
    if (p.type == Status) return;
    if (p.type == AckToggle || p.type == AckSleep) {
      if (pending && p.session == session && p.sequence == command.sequence &&
          ((command.type == Toggle && p.type == AckToggle) ||
           (command.type == Sleep && p.type == AckSleep))) {
        pending = false; confirmed = true;
        if (command.type == Sleep) { sleeping = true; light = false; sleepAt = now + 1000; }
      }
      return;
    }
    if (p.type != Toggle && p.type != Sleep) return;
    // A delayed/duplicate command never toggles twice. A new boot has a new nonce.
    bool fresh = !haveCommand_ || p.session != remoteSession_ ||
                 int32_t(p.sequence - remoteSequence_) > 0;
    if (fresh) {
      haveCommand_ = true; remoteSession_ = p.session; remoteSequence_ = p.sequence;
      if (p.type == Toggle && !sleeping) light = !light;
      if (p.type == Sleep && !sleeping) {
        sleeping = true; light = false; sleepAt = now + 10000;
        pending = false;
      }
    }
    ack = packet(p.type == Sleep ? AckSleep : AckToggle);
    ack.session = p.session; ack.sequence = p.sequence; ackReady = true;
  }
  void tick(uint32_t now) {
    if (pending && attempts >= 3 && due(now, retryAt)) {
      pending = false; failed = true;
      if (command.type == Sleep) { sleeping = true; light = false; sleepAt = now + 1500; }
    }
  }
  bool nextCommand(uint32_t now, Packet &p) {
    if (!pending || attempts >= 3 || !due(now, retryAt)) return false;
    p = command; ++attempts; retryAt = now + 2500; return true;
  }
  bool canSleep(uint32_t now, bool buttonReleased, bool radioIdle) const {
    return sleeping && due(now, sleepAt) && buttonReleased && radioIdle && !ackReady;
  }
};
} // namespace demo
