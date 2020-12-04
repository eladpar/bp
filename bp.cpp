/* 046267 Computer Architecture - Winter 20/21 - HW #1                  */
/* This file should hold your implementation of the predictor simulator */

#include "bp_api.h"
#include <math.h>
#include <stdlib.h>
#include <iostream>  

#define NOSHARE  0
#define LSBSHARE 1
#define MIDSHARE 2

enum State {SNT =0, WNT =1, WT=2, ST=3};
int fsm_index(uint32_t row_index, uint32_t pc);

class shura
{
public:
	shura();
	uint32_t tag;
	uint32_t  *history;
	uint32_t *fsm;
	bool valid;
	uint32_t target;
	~shura();
};


class bp
{
private:
	
public:
	bp();
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


bp::~bp()
{
}


bp hazai;

/* Global Functions */
int ANDvec(int bits)
{
	int vec =0;
	while(bits>0)
	{
		vec = vec << 1;
		vec += 1;
		bits--;
	}
	return vec;
}

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

int getINDEX(uint32_t pc)
{
	uint32_t tagsize = hazai.tagSize;
	int log = (int)log2(hazai.btbSize);
	uint32_t index = (pc >> 2) & ANDvec(log);
	return (int)index;
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
	//init BTB
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

	//init shurut
	if (isGlobalTable == 0 && isGlobalHist == 0) // local history , local fsm
	{
	for (unsigned i = 0; i < btbSize; i++)
		{
			hazai.shurut[i].tag = 0;

			hazai.shurut[i].history = new uint32_t;
			*hazai.shurut[i].history = 0;

			hazai.shurut[i].fsm = new uint32_t[(int)pow(2, historySize)];
			for(int j=0; j< (int)pow(2, historySize); j++)
			{
				hazai.shurut[i].fsm[j] = fsmState;
			}

			hazai.shurut[i].valid = false;
			hazai.shurut[i].target = 0;
		}
		return 0;
	}
	else if (isGlobalTable == 1 && isGlobalHist == 0)// local history global fsm
	{
		hazai.shurut[0].fsm = new uint32_t[(int)pow(2, historySize)];
		for(int j=0; j< (int)pow(2, historySize); j++)
		{
			hazai.shurut[0].fsm[j] = fsmState;
		}

		for (unsigned i = 0; i < btbSize; i++)
		{
			hazai.shurut[i].tag = 0;

			hazai.shurut[i].history = new uint32_t;
			*hazai.shurut[i].history = 0;
			hazai.shurut[i].fsm = hazai.shurut[0].fsm;

			hazai.shurut[i].valid = false;
			hazai.shurut[i].target = 0;
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
			for(int j=0; j< (int)pow(2, historySize); j++)
			{
				hazai.shurut[i].fsm[j] = fsmState;
			}

			hazai.shurut[i].history = hazai.shurut[0].history;

			hazai.shurut[i].valid = false;
			hazai.shurut[i].target = 0;
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
		}
		return 0;
	}
	return -1;
}
 
bool BP_predict(uint32_t pc, uint32_t *dst){
	
	uint32_t pc_tag = getTAG(pc);
	uint32_t index = getINDEX(pc); 
	
	if (hazai.shurut[index].tag == pc_tag && hazai.shurut[index].valid==true)
	{
		int fsm_i = fsm_index(index, pc); 
		bool res = predict(hazai.shurut[index].fsm[fsm_i]);
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

int fsm_index(uint32_t row_index, uint32_t pc)
{	if(hazai.isGlobalTable == false)
	{
		return *hazai.shurut[row_index].history;
	}

	//share is relevant only if the fsm table is global
	else
	{
		if(hazai.Shared == NOSHARE)
		{
			return *hazai.shurut[row_index].history;
		}

		else if(hazai.Shared == LSBSHARE)
		{
			int i = pc >> 2;
			i = i & ANDvec(hazai.historySize);
			i = i ^ *hazai.shurut[row_index].history;
			return i;
		}
		else if(hazai.Shared == MIDSHARE)
		{
			int i = pc >> 16;
			i = i & ANDvec(hazai.historySize);
			i = i ^ *hazai.shurut[row_index].history;
			return i;
		}
	}
}

void BP_update(uint32_t pc, uint32_t targetPc, bool taken, uint32_t pred_dst){
	hazai.branch_counter++;

	uint32_t index = getINDEX(pc);
	uint32_t pc_tag = getTAG(pc);
	int fsm_i = fsm_index(index, pc); 
	uint32_t reshuma_tag = hazai.shurut[index].tag;

	hazai.shurut[index].tag = pc_tag;
	hazai.shurut[index].target = targetPc;

	if (reshuma_tag == pc_tag && hazai.shurut[index].valid)
	{
	hazai.shurut[index].fsm[fsm_i] = nextFSM(hazai.shurut[index].fsm[fsm_i], taken);
	*hazai.shurut[index].history = updateHISTORY(*hazai.shurut[index].history, hazai.historySize, taken);
	}
	else
	{
		hazai.shurut[index].valid = true;
		if(hazai.isGlobalHist == true)
		{
			*hazai.shurut[index].history = updateHISTORY(*hazai.shurut[index].history, hazai.historySize, taken);
		}
		else 
		{
			*hazai.shurut[index].history = updateHISTORY(0, hazai.historySize, taken);
		}


		if(hazai.isGlobalTable == true)
		{
			hazai.shurut[index].fsm[fsm_i] = nextFSM(hazai.shurut[index].fsm[fsm_i] , taken);
		}
		else 
		{
			for(int i=0; i< (int)pow(2, hazai.historySize); i++)
			{
				hazai.shurut[index].fsm[i] = hazai.fsmState;
			}
			hazai.shurut[index].fsm[fsm_i] = nextFSM(hazai.fsmState, taken);
		}
		
		
	}
	


	if (targetPc != pred_dst && taken == true)
	{
		hazai.flush++;
	}
	else if((pred_dst != pc + 4) && taken == false)
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
	


	for (unsigned int i = 0; i < hazai.btbSize; i++)
	{
		
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
	return;
}

