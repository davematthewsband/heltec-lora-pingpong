#include "demo_protocol.h"
#include <cassert>
#include <cstdio>
#include <cstring>
using namespace demo;
static void discover(Node &a, Node &b) {
  a.receive(b.packet(Status), 100); b.receive(a.packet(Status), 100);
}
int main() {
  // Exact wire size, large IDs, truncation/oversize, malformed headers and values.
  Packet p; p.source = 0x123456789abcULL; p.target = 0xabcdef123456ULL;
  p.type = Toggle; p.session = 0xfedcba98; p.sequence = 0x87654321;
  p.millivolts = 3990; p.light = true;
  uint8_t data[256] = {}; encode(p, data);
  Packet decoded;
  for (size_t n = 0; n < sizeof(data); ++n) {
    bool ok = decode(data, n, decoded); assert(ok == (n == kPacketSize));
  }
  assert(decoded.source == p.source && decoded.target == p.target && decoded.session == p.session);
  assert(decoded.sequence == p.sequence && decoded.millivolts == p.millivolts && decoded.light);
  for (unsigned offset : {0u, 1u, 2u, 3u, 30u, 31u}) {
    uint8_t saved = data[offset]; data[offset] = 255;
    assert(!decode(data, kPacketSize, decoded)); data[offset] = saved;
  }
  Packet bad = p; bad.target = 0; encode(bad, data); assert(!decode(data, kPacketSize, decoded));
  assert(batteryPercent(0) == -1 && batteryPercent(5000) == -1);
  assert(batteryPercent(3000) == 0 && batteryPercent(3700) == 50 && batteryPercent(4300) == 100);

  // Wake/boot hold is consumed, debounce, short release and long without short.
  Button button;
  assert(button.update(true, 0) == None);
  assert(button.update(true, 3000) == None);
  assert(button.update(false, 3001) == None);
  assert(button.update(false, 3040) == None);
  assert(button.update(true, 3100) == None);
  assert(button.update(false, 3110) == None); // bounce
  assert(button.update(false, 3150) == None);
  button.update(true, 3200); button.update(true, 3240);
  button.update(false, 3300); assert(button.update(false, 3340) == Short);
  button.update(true, 3400); button.update(true, 3440);
  assert(button.update(true, 5440) == Long);
  assert(button.update(true, 7000) == None);
  button.update(false, 7010); assert(button.update(false, 7050) == None);

  Node a(0x111111111111, 123), b(0x222222222222, 456), stranger(333, 789);
  discover(a, b); assert(a.peer == b.id && b.peer == a.id);
  a.receive(stranger.packet(Status), 101); assert(a.peer == b.id);
  assert(a.request(Toggle, 200)); Packet command;
  assert(a.nextCommand(200, command)); b.receive(command, 210);
  assert(b.light && b.ackReady);
  // Drop acknowledgement: retry same ID, no second toggle.
  assert(!a.nextCommand(1000, command)); assert(a.nextCommand(2700, command));
  b.receive(command, 2710); assert(b.light);
  Packet wrong = b.ack; ++wrong.sequence; a.receive(wrong, 2720); assert(a.pending);
  wrong = b.ack; ++wrong.session; a.receive(wrong, 2721); assert(a.pending);
  wrong = b.ack; wrong.type = AckSleep; a.receive(wrong, 2722); assert(a.pending);
  a.receive(b.ack, 2730); b.ackReady = false;
  assert(!a.pending && a.confirmed && a.peerLight);
  // Commands work both directions. Older duplicate after a newer command is inert.
  assert(a.request(Toggle, 2800)); Packet newer;
  a.nextCommand(2800, newer); b.receive(newer, 2810); assert(!b.light);
  b.receive(command, 2820); assert(!b.light);
  a.receive(b.ack, 2830); assert(a.pending); // old command ACK cannot complete new command
  b.receive(newer, 2840); a.receive(b.ack, 2850); b.ackReady = false; assert(!a.pending);
  assert(b.request(Toggle, 2900)); b.nextCommand(2900, command);
  a.receive(command, 2910); assert(a.light); b.receive(a.ack, 2920); a.ackReady = false;
  assert(b.confirmed);

  // Sleep receiver waits for retries; requester waits for ACK and local button release.
  assert(a.request(Sleep, 3000)); a.nextCommand(3000, command); b.receive(command, 3010);
  assert(b.sleeping && !b.canSleep(3020, true, true));
  b.ackReady = false; // first ACK lost
  a.nextCommand(5500, command); b.receive(command, 5510);
  a.receive(b.ack, 5520); b.ackReady = false;
  assert(a.sleeping && a.confirmed);
  assert(!a.canSleep(7000, false, true)); assert(!a.canSleep(7000, true, false));
  assert(a.canSleep(7000, true, true)); assert(b.canSleep(13010, true, true));

  // All replies lost: bounded retries, local sleep with unconfirmed result.
  Node c(1, 11), d(2, 22); discover(c, d);
  c.request(Sleep, 0);
  for (uint32_t now : {0u, 2500u, 5000u}) assert(c.nextCommand(now, command));
  c.tick(7500); assert(c.failed && c.sleeping && !c.pending);
  assert(c.canSleep(9000, true, true));
  Node solo(4, 44); assert(!solo.request(Sleep, 0));
  assert(solo.failed && solo.canSleep(1500, true, true));

  // Simultaneous independent commands and simultaneous sleep.
  Node e(5, 55), f(6, 66); discover(e, f);
  e.request(Toggle, 0); f.request(Toggle, 0); Packet ec, fc;
  e.nextCommand(0, ec); f.nextCommand(0, fc);
  e.receive(fc, 100); f.receive(ec, 100);
  e.receive(f.ack, 200); f.receive(e.ack, 200);
  assert(e.light && f.light && !e.pending && !f.pending);
  e.ackReady = f.ackReady = false;
  e.request(Sleep, 300); f.request(Sleep, 300);
  e.nextCommand(300, ec); f.nextCommand(300, fc);
  e.receive(fc, 400); f.receive(ec, 400);
  assert(e.sleeping && f.sleeping && !e.pending && !f.pending);
  e.ackReady = f.ackReady = false;
  assert(e.canSleep(10400, true, true) && f.canSleep(10400, true, true));
  // Rollover-safe retry and button timing.
  Node g(7, 77), h(8, 88); discover(g, h);
  g.request(Toggle, 0xfffffff0u); assert(g.nextCommand(0xfffffff0u, command));
  assert(!g.nextCommand(100, command)); assert(g.nextCommand(2500, command));
  puts("Shared demo: codec, buttons, discovery, duplicate/lost commands, acknowledgements, sleep and rollover passed");
}
