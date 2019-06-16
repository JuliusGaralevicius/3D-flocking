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
void main()
{
	uint myID = gl_GlobalInvocationID.x;
	Obstacle me = oIN[myID];
	
	bool collided = false;
	vec3 resultVelocity = me.velocity.xyz;
	for (uint i = 0; i<16; i++)
	{
		if (i!= myID)
		{
			Obstacle other = oIN[i];
			float distance = length(other.Pos-me.Pos);
			if (distance<=(me.radius + other.radius))
			{
				vec2 result = resultVelocity.xy;
				float m1 = me.radius*me.radius;
				float m2 = other.radius*other.radius;
				vec2 v1 = me.velocity.xy;
				vec2 v2 = other.velocity.xy;
				vec2 x1 = me.Pos.xy;
				vec2 x2 = other.Pos.xy;
				
				result -= 2*m2 * dot(v1-v2, x1-x2) *   (x1-x2)     /    ((m1 + m2) * (length(x1-x2)*length(x1-x2)))   ;
				resultVelocity = vec3(result.xy, 1.0);
			}
		}

	}

	
	barrier();
	resultVelocity = vec3(clampVectorLength(resultVelocity.xyz, 0.4));
	me.Pos += resultVelocity;
	
	me.velocity = vec4(resultVelocity.xyz, 0.0);
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