#include <cstdarg>
#include <cstdint>
#include <cstdlib>
#include <ostream>
#include <new>

class UdpSocket{};

struct NetClient {
  UdpSocket socket;
};

extern "C" {

const NetClient *init();

void foo(const NetClient *a);

}  // extern "C"
