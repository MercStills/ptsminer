//
//  OpenCLMomentum2.cpp
//  momentumCL
//
//  Created by Girino Vey on 02/01/14.
//
//

#include "OpenCLMomentum2.h"
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <assert.h>
#ifdef __APPLE__
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif
#include "global.h"
#include "sha_utils.h"

OpenCLMomentum2::OpenCLMomentum2(int _HASH_BITS) {
	max_threads = 1<<30; // very big
	HASH_BITS = _HASH_BITS;

	// compiles
	fprintf(stdout, "Device: %s\n", main.getPlatform(0)->getDevice(0)->getName().c_str());
	cl_ulong maxWorkGroupSize = main.getPlatform(0)->getDevice(0)->getMaxWorkGroupSize();
	fprintf(stdout, "Max work group size: %llu\n", maxWorkGroupSize);

	if (maxWorkGroupSize < max_threads) max_threads = maxWorkGroupSize;

	OpenCLContext *context = main.getPlatform(0)->getContext();
	std::vector<std::string> program_filenames;
	program_filenames.push_back("opencl/opencl_cryptsha512.h");
	program_filenames.push_back("opencl/cryptsha512_kernel.cl");
	program_filenames.push_back("opencl/OpenCLMomentum2.cl");
	OpenCLProgram *program = context->loadProgramFromFiles(program_filenames);

	size_t BLOCKSIZE = max_threads;
	// allocate internal structure
	printf("here\n");
	cl_message = context->createBuffer(sizeof(uint8_t)*32, CL_MEM_READ_ONLY, NULL);
	internal_hash_table = context->createBuffer(sizeof(uint32_t)*(1<<HASH_BITS), CL_MEM_READ_WRITE, NULL);
	temp_collisions1 = context->createBuffer(sizeof(uint32_t)*20, CL_MEM_WRITE_ONLY, NULL);
	temp_collisions2 = context->createBuffer(sizeof(uint32_t)*20, CL_MEM_WRITE_ONLY, NULL);
	temp_collisions_count = context->createBuffer(sizeof(size_t), CL_MEM_READ_WRITE, NULL);
}

OpenCLMomentum2::~OpenCLMomentum2() {
	// destroy
	delete internal_hash_table;
	delete (temp_collisions2);
	delete (temp_collisions1);
	delete (cl_message);
}

void OpenCLMomentum2::find_collisions(uint8_t* message, uint32_t* collisions1, uint32_t* collisions2, size_t* collision_count) {


	// temp storage
	*collision_count = 0;

	OpenCLContext *context = main.getPlatform(0)->getContext();
	OpenCLProgram *program = context->getProgram(0);

	OpenCLKernel *kernel = program->getKernel("kernel_sha512");

	assert(kernel != NULL);

	//size_t BLOCKSIZE = main.getPlatform(0)->getDevice(0)->getMaxWorkGroupSize();
	size_t BLOCKSIZE = kernel->getWorkGroupSize(main.getPlatform(0)->getDevice(0));

	//printf("BLOCKSIZE = %lld\n", BLOCKSIZE);

	kernel->resetArgs();
	kernel->addGlobalArg(cl_message);
	kernel->addGlobalArg(internal_hash_table);
	uint32_t ht_size = 1<<HASH_BITS;
	kernel->addScalarUInt(ht_size);
	kernel->addGlobalArg(temp_collisions1);
	kernel->addGlobalArg(temp_collisions2);
	kernel->addGlobalArg(temp_collisions_count);

	OpenCLCommandQueue *queue = context->createCommandQueue(0);
	cl_event eventw1 = queue->enqueueWriteBuffer(cl_message, message, sizeof(uint8_t)*32, NULL, 0);
	cl_event eventw2 = queue->enqueueWriteBuffer(temp_collisions_count, collision_count, sizeof(uint32_t), &eventw1, 1);

//	cl_event eventk = queue->enqueueKernel1D(kernel, MAX_MOMENTUM_NONCE, worksize, &eventw, 1);
	cl_event eventk = queue->enqueueKernel1D(kernel, MAX_MOMENTUM_NONCE/8, BLOCKSIZE, &eventw2, 1);
	cl_event eventr1 = queue->enqueueReadBuffer(temp_collisions_count, collision_count, sizeof(size_t), &eventk, 1);
	queue->enqueueReadBuffer(temp_collisions1, collisions1, sizeof(uint32_t)*20, &eventr1, 1);
	queue->enqueueReadBuffer(temp_collisions2, collisions2, sizeof(uint32_t)*20, &eventr1, 1);
	queue->finish();
}
