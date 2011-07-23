#ifndef _GLOBALDATA_H
#define _GLOBALDATA_H_

#include "AccessManipulation.h"
#include "ConfigHandler.h"
#include "LogHandler.h"
#include "PacketHandler.h"
#include "BNCSHandler.h"
#include "QueueHandler.h"
#include "CommandHandler.h"
#include "ListHandler.h"

class GlobalData {
public:
	AccessManipulation accessManip;
	ConfigHandler configManip;
	LogHandler logManip;
	PacketHandler packetManip;
	BNCSHandler bncsManip;
	QueueHandler queueManip;
	CommandHandler commandManip;
	ListHandler listManip;
};

extern GlobalData *global;

#endif