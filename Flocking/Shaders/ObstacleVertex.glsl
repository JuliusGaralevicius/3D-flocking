#version 450 core
layout (location = 0) in vec4 position;
layout (location = 1) in vec3 instancePos;
layout (location = 2) in float radius;

uniform mat4 mvp;
uniform mat4 m;
uniform mat4 vp;

mat4 scale(float x, float y, float z){
    return mat4(
        vec4(x,   0.0, 0.0, 0.0),
        vec4(0.0, y,   0.0, 0.0),
        vec4(0.0, 0.0, z,   0.0),
        vec4(0.0, 0.0, 0.0, 1.0)
    );
}

void main()
{
	// create scaling vector based on radius
	vec4 pos = scale(radius, radius, radius)*vec4(position.xyz, 1.0);
    gl_Position = vp*vec4(pos.xyz+instancePos.xyz, 1.0);
}
