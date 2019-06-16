#version 450 core
layout (location = 0) in vec4 position;
layout (location = 1) in vec4 colour;
layout (location = 2) in vec4 instancePos;
layout (location = 3) in vec4 instanceVel;
layout (location = 4) in float instanceType;

uniform mat4 mvp;
uniform vec4 col1;
uniform vec4 col2;

out vec4 outColour;
mat4 make_lookat(vec3 forward, vec3 up)
{
	vec3 side = cross(forward, up);
	vec3 u_frame = cross(side, forward);
	return mat4(vec4(side, 0.0),vec4(u_frame, 0.0),vec4(forward, 0.0),vec4(0.0, 0.0, 0.0, 1.0));
}
uint getCell(vec2 p)
{
	uint cellSize = 32;
	uint numCells = 16;
	return int(p.y/cellSize)*numCells + int(p.x/cellSize);
}
void main()
{
	mat4 lookat = make_lookat(normalize(instanceVel.xyz), vec3(0.0, 1.0, 0.0));
	vec4 pos = vec4(position.xyz, 1.0f)*lookat;
    gl_Position = mvp*vec4(pos +vec4(instancePos.xyz, 0.0));
	
	if (instanceType > 0.5f)
	{
		outColour = vec4(col1.x/255.0, col1.y/255.0, col1.z/255.0, col1.w);
	}
	else
	{
		outColour = vec4(col2.x/255.0, col2.y/255.0, col2.z/255.0, col2.w);
	}
	//uint group = getCell(instancePos.xy);
	//if (instancePos.x>=0 && instancePos.x<=512 && instancePos.y>=0 && instancePos.y<=512)
	//{
	//	float y = group/16;
	//	float x = group - (group/16)*16;
	//	
	//	outColour = vec4(x/16, y/16, y/32, 1.0);
    //
	//}
	//else {
	//	outColour = vec4(1.0, 1.0, 1.0, 1.0);
	//}

}