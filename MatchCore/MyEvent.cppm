module;

#include <pvzclass/pvzclass/pvzevent.hpp>

export module MyEvent;

export class LoadEndEvent : public DLLEventTemplate<0x48E268, 7>
{
public:
	LoadEndEvent(int address) : DLLEventTemplate() { Init(address); };
};
