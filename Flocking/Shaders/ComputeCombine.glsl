#version 450 core
layout (local_size_x = 512) in;
struct Boid
{
	vec4 Pos;
	vec3 Vel;
	float type;
};
layout (binding = 0) buffer prefix
{
	uint pfx[4096];
};
layout (binding = 1) buffer prefixOG
{
	uint pfxOG[4096];
};
layout (binding = 2) buffer max
{
	uint m[4];
};

void main()
{
	int myID = int(gl_GlobalInvocationID.x);
	uint increment = 0;
	for (int i = 0; i <= (myID-1)/1024; i++)
	{
		increment+= m[i];
	}
	uint final = pfx[myID] + increment;
	pfx[myID] = final;
	pfxOG[myID] = final;
}