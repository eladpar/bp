/* 046267 Computer Architecture - Winter 20/21 - HW #1                  */
/* This file should hold your implementation of the predictor simulator */

#include "bp_api.h"
#include <math.h>
#include <stdlib.h>
#include <iostream> 
#define LSBSHARE 1
#define MIDSHARE 2

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
	// shura& shura::operator=(const shura& rhs) {
	// 	if (this != &rhs){
	// 	tag_=rhs.tag;
	// 	if (name_) delete [] name_;
	// 	name_=createNewCopy(rhs.name_);
	// 	}
	// 	return *this;
	// 	}
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
}

/* HAZAI */
bp hazai;

/* Global Functions */
uint32_t getTAG(uint32_t pc)
{
	uint32_t tagSize = hazai.tagSize;
	int log = (int)log2(hazai.btbSize);
	int left = 32 - (tagSize+log+2);
	int right = left + log + 2;
	uint32_t tag = pc << left;
	tag = tag >> right;
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
// void initshurut(shura *hazai.shurut, unsigned Size, unsigned tagSize, unsigned historySize)
// {
// 	for (unsigned i = 0; i < Size; i++)
// 	{
// 		hazai.shurut[i].tag = 0;
// 		hazai.shurut[i].history = 0;
// 		hazai.shurut[i].valid = false;
// 		hazai.shurut[i].target = 0;
// 		hazai.shurut[i].historySize = historySize;
// 		hazai.shurut[i].tagSize = tagSize;
// 	}
	
//}
int Mask(int number_of_bits)
{ 
	int mask = 0;
	while (number_of_bits > 0)
	{
		mask = (mask << 1) + 1;
		number_of_bits--;
	}
	return mask;
}
int getINDEX(uint32_t pc)
{
	uint32_t tagsize = hazai.tagSize;
	int log = (int)log2(hazai.btbSize);
	uint32_t index = (pc >> 2) & Mask(log);
	return (int)index;
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
				// hazai1 = new bp;
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
	// shura *hazai.shurut = hazai.shurut;
	if (isGlobalTable == 0 && isGlobalHist == 0) // local history , local fsm
	{
	for (unsigned i = 0; i < btbSize; i++)
		{
			hazai.shurut[i].tag = 0;

			hazai.shurut[i].history = new uint32_t;
			*hazai.shurut[i].history = 0;
			hazai.shurut[i].fsm = new uint32_t[(int)pow(2, historySize)];
			*hazai.shurut[i].fsm = fsmState;

			hazai.shurut[i].valid = false;
			hazai.shurut[i].target = 0;
			hazai.shurut[i].historySize = historySize;
			hazai.shurut[i].tagSize = tagSize;
		}
		return 0;
	}
	else if (isGlobalTable == 1 && isGlobalHist == 0)// local history global fsm
	{
		hazai.shurut[0].fsm = new uint32_t[(int)pow(2, historySize)];
		*hazai.shurut[0].fsm = fsmState;
		for (unsigned i = 0; i < btbSize; i++)
		{
			hazai.shurut[i].tag = 0;

			hazai.shurut[i].history = new uint32_t;
			*hazai.shurut[i].history = 0;
			hazai.shurut[i].fsm = hazai.shurut[0].fsm;

			hazai.shurut[i].valid = false;
			hazai.shurut[i].target = 0;
			hazai.shurut[i].historySize = historySize;
			hazai.shurut[i].tagSize = tagSize;
		}
		return 0;
	}
	else if (isGlobalTable == 0 && isGlobalHist == 1)// global history local fsm
	{
		hazai.shurut[0].history = new uint32_t;
		*hazai.shurut[0].history = 0;
		for (unsigned i = 0; i < btbSize; i++)
		{
			hazai.shurut[i].tag = 0;

			hazai.shurut[i].fsm = new uint32_t[(int)pow(2, historySize)];
			*hazai.shurut[i].fsm = fsmState;
			hazai.shurut[i].history = hazai.shurut[0].history;

			hazai.shurut[i].valid = false;
			hazai.shurut[i].target = 0;
			hazai.shurut[i].historySize = historySize;
			hazai.shurut[i].tagSize = tagSize;
		}
		return 0;
	}
	else if (isGlobalTable == 1 && isGlobalHist == 1)// global history global fsm
	{
		hazai.shurut[0].history = new uint32_t;
		*hazai.shurut[0].history = 0;
		hazai.shurut[0].fsm = new uint32_t[(int)pow(2, historySize)];
		*hazai.shurut[0].fsm = fsmState;
		for (unsigned i = 0; i < btbSize; i++)
		{
			hazai.shurut[i].tag = 0;

			hazai.shurut[i].fsm = hazai.shurut[0].fsm;
			hazai.shurut[i].history = hazai.shurut[0].history;

			hazai.shurut[i].valid = false;
			hazai.shurut[i].target = 0;
			hazai.shurut[i].historySize = historySize;
			hazai.shurut[i].tagSize = tagSize;
		}
		return 0;
	}
	return -1;
}
 
bool BP_predict(uint32_t pc, uint32_t *dst){
	// std::cout << "Error opening file";
	uint32_t pc_tag = getTAG(pc);
	// shura *hazai.shurut = hazai.shurut;
// std::cout << "Error opening file";
unsigned int a= hazai.btbSize;
	uint32_t index = getINDEX(pc); 
	// TODO <VALID BIT
	if (index > hazai.btbSize)
	{
		std::cout << "Error opening file";
		exit (1);
	}
	if (hazai.shurut[index].tag == pc_tag && hazai.shurut[index].valid == true)
	{
		bool res;
		if (hazai.Shared == LSBSHARE && hazai.isGlobalTable == true)
		{
			uint32_t new_tag = pc >> 2;
			uint32_t cut_tag = new_tag << (32-hazai.historySize);
			uint32_t new_cut_tag = cut_tag >> (32-hazai.historySize);
			res = predict(hazai.shurut[index].fsm[*hazai.shurut[index].history ^ new_cut_tag]);
		}
		// XOR PC HISTORY FROM BIT 2
		
		else if (hazai.Shared == MIDSHARE && hazai.isGlobalTable == true)
		{
			uint32_t new_tag = pc >> 16;
			uint32_t cut_tag = new_tag << (32-hazai.historySize);
			uint32_t new_cut_tag = cut_tag >> (32-hazai.historySize);
			res = predict(hazai.shurut[index].fsm[*hazai.shurut[index].history ^ new_cut_tag]);
		}
		// XOR PC HISTORY FROM BIT 16

		else
		{
			res = predict(hazai.shurut[index].fsm[*hazai.shurut[index].history]);
		}

		if (res)
		{
			*dst = hazai.shurut[index].target;
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
	// shura *hazai.shurut = hazai.shurut;
	uint32_t index = getINDEX(pc);
	uint32_t pc_tag = getTAG(pc);
	uint32_t reshuma_tag = hazai.shurut[index].tag;
	hazai.shurut[index].target = targetPc;

	if (reshuma_tag == pc_tag && hazai.shurut[index].valid)
	{
		if (hazai.Shared == LSBSHARE && hazai.isGlobalTable == true)
		{
			uint32_t new_tag = pc >> 16;
			uint32_t cut_tag = new_tag << (32-hazai.historySize);
			uint32_t new_cut_tag = cut_tag >> (32-hazai.historySize);
			hazai.shurut[index].fsm[*hazai.shurut[index].history ^ new_cut_tag] = nextFSM(hazai.shurut[index].fsm[*hazai.shurut[index].history ^ new_cut_tag], taken);
		}
		
		else if (hazai.Shared == MIDSHARE && hazai.isGlobalTable == true)
		{
			uint32_t new_tag = pc >> 16;
			uint32_t cut_tag = new_tag << (32-hazai.historySize);
			uint32_t new_cut_tag = cut_tag >> (32-hazai.historySize);
			hazai.shurut[index].fsm[*hazai.shurut[index].history ^ new_cut_tag] = nextFSM(hazai.shurut[index].fsm[*hazai.shurut[index].history ^ new_cut_tag], taken);
		}
		else
		{
			hazai.shurut[index].fsm[*hazai.shurut[index].history] = nextFSM(hazai.shurut[index].fsm[*hazai.shurut[index].history], taken);
		}
		*hazai.shurut[index].history = updateHISTORY(*hazai.shurut[index].history, hazai.historySize, taken);
	}
	else
	{
		hazai.shurut[index].valid = true;
		hazai.shurut[index].tag = pc_tag;
		hazai.shurut[index].tag = targetPc;
		if (hazai.Shared == LSBSHARE && hazai.isGlobalTable == true)
		{
			uint32_t new_tag = pc >> 16;
			uint32_t cut_tag = new_tag << (32-hazai.historySize);
			uint32_t new_cut_tag = cut_tag >> (32-hazai.historySize);
			hazai.shurut[index].fsm[*hazai.shurut[index].history ^ new_cut_tag] = nextFSM(hazai.shurut[index].fsm[*hazai.shurut[index].history ^ new_cut_tag], taken);
		}
		
		else if (hazai.Shared == MIDSHARE && hazai.isGlobalTable == true)
		{
			uint32_t new_tag = pc >> 16;
			uint32_t cut_tag = new_tag << (32-hazai.historySize);
			uint32_t new_cut_tag = cut_tag >> (32-hazai.historySize);
			hazai.shurut[index].fsm[*hazai.shurut[index].history ^ new_cut_tag] = nextFSM(hazai.shurut[index].fsm[*hazai.shurut[index].history ^ new_cut_tag], taken);
		}
		else
		{
			if (hazai.isGlobalTable == false)
				hazai.shurut[index].fsm[*hazai.shurut[index].history] = nextFSM(hazai.fsmState, taken);
			else
			{
				hazai.shurut[index].fsm[*hazai.shurut[index].history] = nextFSM(hazai.shurut[index].fsm[*hazai.shurut[index].history], taken);
			}
			
		}

		if (hazai.isGlobalHist ==false)
			*hazai.shurut[index].history = updateHISTORY(0, hazai.historySize, taken);
		else
			*hazai.shurut[index].history = updateHISTORY(*hazai.shurut[index].history, hazai.historySize, taken);
		
	}
	
	if ((targetPc != pred_dst) && (hazai.shurut[index].valid == true) && taken ) 
	{
		hazai.flush++;
	}
	else if ((pred_dst != pc+4) && !taken )
	{
		hazai.flush++;
	}
	else if ((hazai.shurut[index].valid == false) && taken )
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
		curStats->size = hazai.btbSize * (1+ hazai.tagSize + 30 + hazai.historySize + 2*pow(2, hazai.historySize));// TODO check target size
	}
	else if (isGlobalTable == 1 && isGlobalHist == 0)// local history global fsm
	{
		curStats->size = (hazai.btbSize * (1+ hazai.tagSize + 30+hazai.historySize)+ 2*pow(2, hazai.historySize));
	}
	else if (isGlobalTable == 0 && isGlobalHist == 1)// global history local fsm
	{
		curStats->size = (hazai.btbSize * (1+ hazai.tagSize + 30+2*pow(2, hazai.historySize))+ hazai.historySize);
	}
	else if (isGlobalTable == 1 && isGlobalHist == 1)// global history global fsm
	{
		curStats->size = (hazai.btbSize * (1+ hazai.tagSize + 30) + hazai.historySize + 2*pow(2, hazai.historySize));
	}
	// hazai.~bp();
		for (unsigned int i = 0; i < hazai.btbSize; i++)
	{
		// hazai.shurut[i].~shura();
		if (isGlobalTable == 0 && isGlobalHist == 0) // local history , local fsm
		{
			delete hazai.shurut[i].history;
			delete [] hazai.shurut[i].fsm;
		}
		else if (isGlobalTable == 1 && isGlobalHist == 0)// local history global fsm
		{
			delete hazai.shurut[i].history;
			if (i==0)
				delete [] hazai.shurut[i].fsm;
		}
		else if (isGlobalTable == 0 && isGlobalHist == 1)// global history local fsm
		{
			delete [] hazai.shurut[i].fsm;
			if (i==0)
				delete hazai.shurut[i].history;
		}
		else if (isGlobalTable == 1 && isGlobalHist == 1)// global history global fsm
		{
			if (i==0)
			{
				delete hazai.shurut[i].history;
				delete [] hazai.shurut[i].fsm;
			}
		}
	}
	
	delete [] hazai.shurut;
	// delete hazai1;
	return;
}

