/* 046267 Computer Architecture - Winter 20/21 - HW #1                  */
/* This file should hold your implementation of the predictor simulator */

#include "bp_api.h"

class shura
{
public:
	shura();
	uint32_t tag;
	uint32_t history;
	bool valid;
	uint32_t getTAG(uint32_t pc);
	uint32_t getHISTORY(uint32_t pc);
	uint32_t pred_pc;
	uint32_t tagSize;
	uint32_t historySize;
	~shura();
};

uint32_t shura::getTAG(uint32_t pc)
{
	tag = pc >> (32 -tagSize);
	return tag;
}
uint32_t shura::getHISTORY(uint32_t pc)
{
	history = (pc << tagSize) >> (32 -historySize);
	return history;
}
shura::shura()
{
	
}
shura::~shura()
{
}


class bp
{
private:
	
public:
	bp(unsigned btb, unsigned history, unsigned tag, unsigned fsm, bool globalhist, bool globaltable, int shared_);
	unsigned btbSize;
	unsigned historySize;
	unsigned tagSize;
	unsigned fsmState;
	bool isGlobalHist;
	shura *shurut;
	bool isGlobalTable;
	int Shared;
	~bp();
};

void initshurut(shura *arr, unsigned Size, unsigned tagSize, unsigned historySize)
{
	for (unsigned i = 0; i < Size; i++)
	{
		arr[i].tag = 0;
		arr[i].history = 0;
		arr[i].valid = false;
		arr[i].pred_pc = 0;
		arr[i].historySize = historySize;
		arr[i].tagSize = tagSize;
	}
	
}
bp::bp(unsigned btb, unsigned history, unsigned tag, unsigned fsm, bool globalhist, bool globaltable, int shared_) :
btbSize(btb), historySize(history), tagSize(tag), fsmState(fsm), isGlobalHist(globalhist), isGlobalTable(globaltable), Shared(shared_)
{
	shurut = new shura[btbSize];
	initshurut(shurut, btbSize, tagSize, historySize);
}

bp::~bp()
{
	delete [] shurut;
}


int BP_init(unsigned btbSize, unsigned historySize, unsigned tagSize, unsigned fsmState,
			bool isGlobalHist, bool isGlobalTable, int Shared){

bp hazai(unsigned btbSize, unsigned historySize, unsigned tagSize, unsigned fsmState,
			bool isGlobalHist, bool isGlobalTable, int Share);
/* 	shura arr[btbSize];
	arr[2] = shura;
	arr[2].tag;
	arr[2].history */;
	return -1;
}
 
bool BP_predict(uint32_t pc, uint32_t *dst){
	return false;
}

void BP_update(uint32_t pc, uint32_t targetPc, bool taken, uint32_t pred_dst){
	return;
}

void BP_GetStats(SIM_stats *curStats){
	return;
}

