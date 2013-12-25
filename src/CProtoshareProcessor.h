/*
 * CProtosharePoocessor.h
 *
 *  Created on: 20/12/2013
 *      Author: girino
 */

#ifndef CPROTOSHAREPOOCESSOR_H_
#define CPROTOSHAREPOOCESSOR_H_
#include "main_poolminer.h"

typedef void (*sha512_func_t)(unsigned char*, unsigned int, unsigned char*);

void sha512_func_avx(unsigned char* in, unsigned int size, unsigned char* out);
void sha512_func_sph(unsigned char* in, unsigned int size, unsigned char* out);

class CProtoshareProcessor {
public:
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

template<int COLLISION_TABLE_SIZE>
void protoshares_process(blockHeader_t* block,  CBlockProvider* bp, uint32_t* collisionIndices,
		sha512_func_t sha512_func, unsigned int thread_id);

#endif /* CPROTOSHAREPOOCESSOR_H_ */
