#version 450 core
layout (local_size_x = 256) in;
struct Boid
{
	vec4 Pos;
	vec3 Vel;
	float type;
};
layout (binding = 0) uniform atomic_uint count[4096];
layout (binding = 1) buffer inBoids
{
	Boid bIN[];
};


uniform uint gridDimensions = 512;
uniform uint cellCount = 16; // cell count in one dimension
uint cellSize = 512/16;

uint getCell(vec3 pos)
{
	return uint(pos.z/cellSize)*cellCount*cellCount + uint(pos.y/cellSize)*cellCount + uint(pos.x/cellSize);
}
void main()
{
	uint cellID = getCell(bIN[gl_GlobalInvocationID.x].Pos.xyz);
	if (cellID>=cellCount*cellCount*cellCount)
	{
	atomicCounterIncrement(count[0]);
	}
	else 
	{
		atomicCounterIncrement(count[cellID]);
	}
	
}