#version 450 core
layout (local_size_x = 1024) in;

layout (binding = 0) uniform atomic_uint count[4096];
layout (binding = 1) buffer bufferName
{
	uint sum[4096];
};
layout (binding = 2) buffer maxBuffer
{
	uint max[4];
};

shared uint shared0[1024];
shared uint shared1[1024];

void main()
{
	int myID = int(gl_GlobalInvocationID.x);
	int localid = int(gl_LocalInvocationID.x);
	shared0[localid] = atomicCounter(count[myID]);
	
	int neighbour;
	for (uint i = 0; i<=log2(1024); i++)
	{
		barrier();
		neighbour = int(pow(2, i));
		if (mod(i, 2) == 0)
		{
			if (localid-neighbour >= 0)
				shared1[localid] = shared0[localid] + shared0[localid-neighbour];
			else 
				shared1[localid] = shared0[localid];
		}
		else
		{
			if (localid-neighbour >= 0)
				shared0[localid] = shared1[localid] + shared1[localid-neighbour];
			else 
				shared0[localid] = shared1[localid];
		}
	}
	
	//memoryBarrier();
	//barrier();
	//memoryBarrierShared();

	sum[myID+1] = shared1[localid];
	if (mod(myID+1, 1024) == 0)
	{
		uint maxID = (myID+1)/1024;
		max[maxID] = shared1[localid];
	}
}