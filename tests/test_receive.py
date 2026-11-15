"""Exercise the actual receive callback with host stubs and memory sanitizers."""
from pathlib import Path
import subprocess
import tempfile

source = (Path(__file__).resolve().parents[1] / 'archive/pingpong-oled-baseline.cpp').read_text()
start = source.index('void OnRxDone( uint8_t *payload', source.index('void OnTxTimeout( void ){'))
callback = source[start:source.index('\nvoid lora_init', start)]
harness = r'''
#include <cassert>
#include <cstdint>
#include <cstring>
struct { template<class... T> void printf(const char*, T...) {} void println(const char*) {} } Serial;
struct { void Sleep() {} } Radio;
char rxpacket[30];
int rxNumber=0, Rssi=0, rxSize=0;
bool receiveflag=false;
enum { STATE_RX, STATE_TX };
int state=STATE_RX;
'''
harness += callback
harness += r'''
int main() {
  uint8_t payload[256]; memset(payload, 'x', sizeof(payload));
  for (unsigned size : {0u, 1u, 28u, 29u, 30u, 31u, 255u, 256u}) {
    memset(rxpacket, 'z', sizeof(rxpacket));
    rxNumber=0; rxSize=0; receiveflag=false; state=STATE_TX;
    OnRxDone(payload, size, -75, 3);
    if (size < sizeof(rxpacket)) {
      assert(rxNumber == 1 && rxSize == int(size));
      assert(rxpacket[size] == '\0');
      assert(memcmp(rxpacket, payload, size) == 0);
      assert(receiveflag && state == STATE_TX);
    } else {
      assert(rxNumber == 0 && !receiveflag && state == STATE_RX);
      for (char c : rxpacket) assert(c == 'z');
    }
  }
}
'''
harness = '#include <initializer_list>\n' + harness
with tempfile.TemporaryDirectory() as tmp:
    cpp = Path(tmp) / 'receive.cpp'
    binary = Path(tmp) / 'receive'
    cpp.write_text(harness)
    subprocess.run(['c++', '-std=c++11', '-fsanitize=address,undefined', str(cpp), '-o', str(binary)], check=True)
    subprocess.run([str(binary)], check=True)
print('Receive callback: 8 boundary cases passed with address/undefined sanitizers')
