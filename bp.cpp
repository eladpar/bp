/* 046267 Computer Architecture - Winter 20/21 - HW #1                  */
/* This file should hold your implementation of the predictor simulator */

#include "bp_api.h"
#include <math.h>
#include <stdlib.h>
#include <iostream>  

enum State {SNT =0, WNT =1, WT=2, ST=3};

class shura
{
public:
	shura();
	uint32_t tag;
	uint32_t  *history;
	uint32_t *fsm;
	bool valid;
	// uint32_t getTAG(uint32_t pc);
	uint32_t getHISTORY(uint32_t pc);
	uint32_t target;
	uint32_t tagSize;
	uint32_t historySize;
	~shura();
};

// uint32_t getTAG(uint32_t pc)
// {
// 	tag = pc >> (32 -tagSize);
// 	return tag;
// }
uint32_t shura::getHISTORY(uint32_t his)
{
	// *history = his >> (32 -historySize); // TODO
	return *history;
}
shura::shura()
{
	
}
shura::~shura()
{
	delete history;
	delete [] fsm;
}


class bp
{
private:
	
public:
	bp();
	////bp(unsigned btb, unsigned history, unsigned tag, unsigned fsm, bool globalhist, bool globaltable, int shared_);
	unsigned btbSize;
	unsigned historySize;
	unsigned tagSize;
	unsigned fsmState;
	bool isGlobalHist;
	int flush;
	int branch_counter;
	shura *shurut;
	bool isGlobalTable;
	int Shared;
	~bp();
};

bp::bp()
{}

//// bp::bp(unsigned btb, unsigned history, unsigned tag, unsigned fsm, bool globalhist, bool globaltable, int shared_) :
//// btbSize(btb), historySize(history), tagSize(tag), fsmState(fsm), isGlobalHist(globalhist), isGlobalTable(globaltable), Shared(shared_)
//// {
// 	// shurut = new shura[btbSize];//TODO dismantle this ^
// 	// initshurut(shurut, btbSize, tagSize, historySize);
//// }

bp::~bp()
{
	for (unsigned int i = 0; i < btbSize; i++)
	{
		shurut[i].~shura();
	}
	
	delete [] shurut;
}

/* HAZAI */
bp hazai; 

/* Global Functions */
uint32_t getTAG(uint32_t pc, uint32_t tagSize)
{
	uint32_t tag = pc >> (32 -tagSize);
	return tag;
}
bool predict(uint32_t fsm)
{
	bool res;
	switch (fsm)
	{
	case SNT :
		res = false;
		break;
	case WNT :
		res = false;
		break;
	case WT :
		res = true;
		break;
	case ST :
		res = true;
		break;
	
	default:
		res =false;
		break;
	}
	return res;
}
void initshurut(shura *arr, unsigned Size, unsigned tagSize, unsigned historySize)
{
	for (unsigned i = 0; i < Size; i++)
	{
		arr[i].tag = 0;
		arr[i].history = 0;
		arr[i].valid = false;
		arr[i].target = 0;
		arr[i].historySize = historySize;
		arr[i].tagSize = tagSize;
	}
	
}
int getINDEX(uint32_t pc)
{
	return (int) pc << hazai.tagSize >> (32- (int)log2(hazai.btbSize));
}

uint32_t nextFSM(uint32_t fsm, bool taken)
{
	if (fsm == SNT  && taken ==false)
	{
		return SNT;
	}
	else if (fsm == SNT && taken == true)
	{
		return WNT;
	}
	else if (fsm == WNT && taken == false)
	{
		return SNT;
	}
	else if (fsm == WNT && taken == true)
	{
		return WT;
	}
	else if (fsm == WT && taken == false)
	{
		return WNT;
	}
	else if (fsm == WT && taken == true)
	{
		return ST;
	}
	else if (fsm == ST && taken == false)
	{
		return WT;
	}
	else if (fsm == ST && taken == true)
	{
		return ST;
	}
	else return hazai.fsmState;
}

uint32_t updateHISTORY(uint32_t history, unsigned int historySize ,bool taken)
{
	uint32_t new_history = (history << (32 - historySize +1)) >> (32 - historySize);
	if (taken)
	{
		return new_history++;
	}
	else
	{
		return new_history;
	}
	
}

int BP_init(unsigned btbSize, unsigned historySize, unsigned tagSize, unsigned fsmState,
			bool isGlobalHist, bool isGlobalTable, int Shared){

//// bp hazai(unsigned btbSize, unsigned historySize, unsigned tagSize, unsigned fsmState,
// 	//		bool isGlobalHist, bool isGlobalTable, int Share);
			// TODO update all the fields
			//TODO new shurut[] and initshurut
			/* Initialize hazai according to input - overrides deafult sattes */
	hazai.btbSize = btbSize;
	hazai.historySize = historySize;
	hazai.tagSize =tagSize;
	hazai.fsmState = fsmState;
	hazai.isGlobalHist = isGlobalHist;
	hazai.isGlobalTable = isGlobalTable;
	hazai.Shared = Shared;
	hazai.flush = 0;
	hazai.branch_counter = 0;

	hazai.shurut = new shura[btbSize];
	shura *arr = hazai.shurut;
	if (isGlobalTable == 0 && isGlobalHist == 0) // local history , local fsm
	{
	for (unsigned i = 0; i < btbSize; i++)
		{
			arr[i].tag = 0;

			arr[i].history = new uint32_t;
			*arr[i].history = 0;
			arr[i].fsm = new uint32_t[(int)pow(2, historySize)];
			*arr[i].fsm = fsmState;

			arr[i].valid = false;
			arr[i].target = 0;
			arr[i].historySize = historySize;
			arr[i].tagSize = tagSize;
		}
		return 0;
	}
	else if (isGlobalTable == 1 && isGlobalHist == 0)// local history global fsm
	{
		arr[0].fsm = new uint32_t[(int)pow(2, historySize)];
		*arr[0].fsm = fsmState;
		for (unsigned i = 0; i < btbSize; i++)
		{
			arr[i].tag = 0;

			arr[i].history = new uint32_t;
			*arr[i].history = 0;
			arr[i].fsm = arr[0].fsm;

			arr[i].valid = false;
			arr[i].target = 0;
			arr[i].historySize = historySize;
			arr[i].tagSize = tagSize;
		}
		return 0;
	}
	else if (isGlobalTable == 0 && isGlobalHist == 1)// global history local fsm
	{
		arr[0].history = new uint32_t;
		*arr[0].history = 0;
		for (unsigned i = 0; i < btbSize; i++)
		{
			arr[i].tag = 0;

			arr[i].fsm = new uint32_t[(int)pow(2, historySize)];
			*arr[i].fsm = fsmState;
			arr[i].history = arr[0].history;

			arr[i].valid = false;
			arr[i].target = 0;
			arr[i].historySize = historySize;
			arr[i].tagSize = tagSize;
		}
		return 0;
	}
	else if (isGlobalTable == 1 && isGlobalHist == 1)// global history global fsm
	{
		arr[0].history = new uint32_t;
		*arr[0].history = 0;
		arr[0].fsm = new uint32_t[(int)pow(2, historySize)];
		*arr[0].fsm = fsmState;
		for (unsigned i = 0; i < btbSize; i++)
		{
			arr[i].tag = 0;

			arr[i].fsm = arr[0].fsm;
			arr[i].history = arr[0].history;

			arr[i].valid = false;
			arr[i].target = 0;
			arr[i].historySize = historySize;
			arr[i].tagSize = tagSize;
		}
		return 0;
	}
	return -1;
}
 
bool BP_predict(uint32_t pc, uint32_t *dst){
	std::cout << "Error opening file";
	uint32_t pc_tag = getTAG(pc, hazai.tagSize);
	shura *arr = hazai.shurut;
std::cout << "Error opening file";
	uint32_t index = getINDEX(pc);
	// TODO <VALID BIT
	if (index < hazai.btbSize)
	{
		std::cout << "Error opening file";
		exit (1);
	}
	if (arr[index].tag == pc_tag)
	{
		bool res = predict(*arr[index].fsm);
		if (res)
		{
			*dst = arr[index].target;
		}	
		else
		{
			*dst = pc +4;
		}
		return res;
	}
	else
	{
		*dst = pc +4;
		return false;
	}
}

void BP_update(uint32_t pc, uint32_t targetPc, bool taken, uint32_t pred_dst){
	hazai.branch_counter++;
	shura *arr = hazai.shurut;
	uint32_t index = getINDEX(pc);
	uint32_t pc_tag = getTAG(pc, hazai.tagSize);
	uint32_t reshuma_tag = arr[index].tag;

	if (reshuma_tag == pc_tag && arr[index].valid)
	{
	arr[index].fsm[*arr[index].history] = nextFSM(arr[index].fsm[*arr[index].history], taken);
	*arr[index].history = updateHISTORY(*arr[index].history, hazai.historySize, taken);
	}
	else
	{
		arr[index].valid = true;
		arr[index].fsm[*arr[index].history] = nextFSM(hazai.fsmState, taken);
		*arr[index].history = updateHISTORY(0, hazai.historySize, taken);
	}
	
	
	if (targetPc != pred_dst)
	{
		hazai.flush++;
	}
	
	return;
}

void BP_GetStats(SIM_stats *curStats){
	curStats->br_num = hazai.branch_counter;
	curStats->flush_num = hazai.flush;
	bool isGlobalTable = hazai.isGlobalTable;
	bool isGlobalHist = hazai.isGlobalHist;

	if (isGlobalTable == 0 && isGlobalHist == 0) // local history , local fsm
	{
		curStats->size = hazai.btbSize * (1+ hazai.tagSize + 32 + hazai.historySize + 2*pow(2, hazai.historySize));// TODO check target size
	}
	else if (isGlobalTable == 1 && isGlobalHist == 0)// local history global fsm
	{
		curStats->size = (hazai.btbSize * (1+ hazai.tagSize + 32+hazai.historySize)+ 2*pow(2, hazai.historySize));
	}
	else if (isGlobalTable == 0 && isGlobalHist == 1)// global history local fsm
	{
		curStats->size = (hazai.btbSize * (1+ hazai.tagSize + 32+2*pow(2, hazai.historySize))+ hazai.historySize);
	}
	else if (isGlobalTable == 1 && isGlobalHist == 1)// global history global fsm
	{
		curStats->size = (hazai.btbSize * (1+ hazai.tagSize + 32) + hazai.historySize + 2*pow(2, hazai.historySize));
	}
	hazai.~bp();
	return;
}

