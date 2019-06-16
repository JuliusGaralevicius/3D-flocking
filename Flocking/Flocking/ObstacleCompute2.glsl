#version 450 core
layout (local_size_x = 16) in;
struct Obstacle
{
	vec3 Pos;
	float radius;
	vec4 velocity;
};
layout (binding = 0) buffer inObstacles
{
	Obstacle oIN[];
};
vec3 clampVectorLength(vec3 vector, float max)
{
	if (length(vector)>max)
	{
		return normalize(vector)*max;
	}
	else
	{
		return vector;
	}
}
uniform float time = 0.0f;
uniform float dt = 0.0f;
float speed = 300.0f;
void main()
{
	uint myID = gl_GlobalInvocationID.x;
	Obstacle me = oIN[myID];
	
	float elasticity = 1.00f;
	
	vec3 positionAdjust = vec3(0.0f);
	vec3 velocityAdjust = vec3(0.0f);
	
	for (uint i = 0; i<16; i++)
	{

		if (i != myID)
		{
			Obstacle other = oIN[i];
			float distance = length(other.Pos - me.Pos);
			if (distance < me.radius+other.radius)
			{
				// fix position
				float penetration = (me.radius + other.radius) - distance;
				vec3 collisionNormal = normalize(other.Pos - me.Pos);
				positionAdjust = collisionNormal*penetration; // *penetration;
				
				// apply impulse
				float impulse = -(1+elasticity)*dot((me.velocity - other.velocity).xyz, collisionNormal) / (1/other.radius + 1/me.radius);
				
				velocityAdjust = (1.0f/me.radius)*collisionNormal*impulse;
				break;

			}
		}
	}
	
	barrier();
	memoryBarrier();
	
	
	me.velocity += vec4( velocityAdjust, 0.0f);
	me.Pos -= positionAdjust;
	me.Pos += me.velocity.xyz;
	

	



	
	if (me.Pos.x>512.0f)
		me.Pos.x=me.Pos.x-512.0f;
	else if (me.Pos.x<0)
		me.Pos.x=me.Pos.x + 512.0f;
	if (me.Pos.y>512.0f)
		me.Pos.y=me.Pos.y-512.0f;
	else if (me.Pos.y<0)
		me.Pos.y=me.Pos.y + 512.0f;
	if (me.Pos.z>512.0f)
		me.Pos.z=me.Pos.z-512.0f;
	else if (me.Pos.z<0)
		me.Pos.z=me.Pos.z + 512.0f;	
		
	oIN[myID]=me;
}