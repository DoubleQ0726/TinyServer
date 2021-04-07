#pragma once
#include <ucontext.h>
#include <memory>

namespace TinyServer
{

class Fiber : public std::enable_shared_from_this<Fiber>
{

};


}