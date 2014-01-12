/*
 * CProtosharePoocessor.h
 *
 *  Created on: 20/12/2013
 *      Author: girino
 */

#ifndef CPROTOSHAREPOOCESSOR_H_
#define CPROTOSHAREPOOCESSOR_H_
#include "main_poolminer.h"
#include "AbstractMomentum.h"

typedef void (*sha512_func_t)(unsigned char*, unsigned int, unsigned char*);

void sha512_func_avx(unsigned char* in, unsigned int size, unsigned char* out);
void sha512_func_sph(unsigned char* in, unsigned int size, unsigned char* out);

class CProtoshareProcessor {
public:
	CProtoshareProcessor(); // for compatibility
	CProtoshareProcessor(SHAMODE _shamode,
			unsigned int _collisionTableBits,
			unsigned int _thread_id);
	virtual ~CProtoshareProcessor();
	virtual void protoshares_process(blockHeader_t* block,  CBlockProvider* bp);

	uint32_t *collisionIndices;
	unsigned int collisionTableBits;
	unsigned int thread_id;
	SHAMODE shamode;
};

class CProtoshareProcessorGPU : public CProtoshareProcessor {
public:
	CProtoshareProcessorGPU(SHAMODE _shamode,
			unsigned int _collisionTableBits,
			unsigned int _thread_id);
	virtual ~CProtoshareProcessorGPU();
	virtual void protoshares_process(blockHeader_t* block,  CBlockProvider* bp);
private:
	unsigned int collisionTableBits;
	unsigned int thread_id;
	SHAMODE shamode;
	AbstractMomentum * M1;
};

#endif /* CPROTOSHAREPOOCESSOR_H_ */
