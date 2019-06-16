#version 450 core
layout (location = 0) in vec4 position;
layout (binding = 1) uniform atomic_uint count[4096];

uniform mat4 mvp;
out vec4 colour;



mat4 translate(float x, float y, float z)
{
    return mat4(
        vec4(1.0, 0.0, 0.0, 0.0),
        vec4(0.0, 1.0, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(x,   y,   z,   1.0)
    );
}

void main()
{
	float padding = 1.025;
	int z = gl_InstanceID/(16*16);
	int x = int(mod(gl_InstanceID, 16));
	int y = (gl_InstanceID/16)-z*16;
	
    gl_Position = mvp*translate(float(x*padding)*32.0, float(y*padding)*32.0, float(z*padding)*32.0)*position;
	
	float numBoids = float(atomicCounter(count[gl_InstanceID]));
	colour = vec4(0.0f, 1.0f-numBoids/100.1f, numBoids/100.1f, 1.0f);
}