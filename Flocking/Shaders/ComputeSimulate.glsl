#version 450 core
layout (local_size_x = 512) in;
struct Boid
{
	vec4 Pos;
	vec3 Vel;
	float type;
};
struct Obstacle
{
	vec3 position;
	float radius;
	vec4 velocity;
};
layout (binding = 0) buffer inBoids
{
	Boid bIN[];
};
layout (binding = 1) buffer inIndexes
{
	uint sortedIndexes[];
};
layout (binding = 2) buffer inObstacles
{
	Obstacle oIN[];
};

uniform uint dimension = 16;
uniform uint nbhdRange = 4096;
uniform float dt = 0.0f;
uniform vec4 goal1 = vec4(0.0f);
uniform vec4 goal2 = vec4(0.0f);
uniform float speed = 150.0f;


uniform float groupRadius = 6;
uniform float avoidRadius = 4;
uniform float otherAvoidRadius = 5;

uniform float homing = 1.0f;
uniform float avoidObstacles = 0.0f;

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
uint getCell(vec3 pos)
{
	uint cellSize = 32;
	uint cellCount = 16;
	return uint(pos.z/cellSize)*cellCount*cellCount + uint(pos.y/cellSize)*cellCount + uint(pos.x/cellSize);
}
bool inrange(uint value)
{
	return (value<nbhdRange);
}

void main()
{
	uint myID = gl_GlobalInvocationID.x;
	Boid me = bIN[myID];
	
	// will return invalid if out of bounds
	uint myCell = getCell(me.Pos.xyz);

	vec3 avoid = vec3(0.0);
	vec3 group = vec3(0.0);
	vec3 align = vec3(0.0);
	vec3 otherAvoid = vec3(0.0);
	vec3 obstacleAvoid = vec3(0.0);
	
	vec3 home;
	if (me.type>0.5)
	{
		home = (goal1-me.Pos).xyz;
	}
	else
	{
		home = (goal2-me.Pos).xyz;
	}
	home = dot(home,home)*home*homing;
	
	int avoided = 0;
	int otherAvoided = 0;
	int obstacleAvoided = 0;
	int grouped = 0;
	int aligned = 0;
	
	// table which shows which neighbourhood cells are present and which are not
	uint offsetFlags[27] = {1, 1, 1,			// far z
						   1, 1, 1,
						   1, 1, 1,
						   
						   1, 1, 1,				// middle z
						   1, 1, 1,
						   1, 1, 1,
						   
						   1, 1, 1,				// close z
						   1, 1, 1,
						   1, 1, 1
						   };
						   
	// table where each neighbourhood entry has start index of sorted array					   
	
	if (mod(myCell, dimension) == 0)
	{
		// leftmost cell
		for (int i = 0; i<3; i++)
		{
			offsetFlags[i*9] = nbhdRange;
			offsetFlags[i*9 + 3] = nbhdRange;
			offsetFlags[i*9 + 6] = nbhdRange;
		}

	}
	if (mod(myCell, dimension) == dimension-1)
	{
		// rightmost cell
		for (int i = 0; i<3; i++)
		{
			offsetFlags[i*9 + 2] = nbhdRange;
			offsetFlags[i*9 + 5] = nbhdRange;
			offsetFlags[i*9 + 8] = nbhdRange;
		}
	}
	if (mod(myCell, dimension*dimension) < dimension)
	{
	   // topmost cell
	   for (int i = 0; i<3; i++)
	   {
			offsetFlags[i*9] = nbhdRange;
			offsetFlags[i*9 + 1] = nbhdRange;
			offsetFlags[i*9 + 2] = nbhdRange;
	   }

	}
	if (mod(myCell, dimension*dimension) >= dimension*(dimension - 1))
	{
	    // bottom cell
		for (int i = 0; i<3; i++)
	    {
			offsetFlags[i*9 + 6] = nbhdRange;
			offsetFlags[i*9 + 7] = nbhdRange;
			offsetFlags[i*9 + 8] = nbhdRange;
		}
	}
	if (myCell / (dimension*dimension) == 0)
	{	
		// nearest cell [0 ; dimension*dimension - 1]
		for (int i = 18; i< 27; i++)
		{
			offsetFlags[i] = nbhdRange;
		}
	}
	if (myCell / (dimension*dimension*(dimension-1)) > 0)
	{
		// furthest cell [dimension*dimension*(dimension-1) ; dimension^3 -1]
		for (int i = 0; i<9; i++)
		{
			offsetFlags[i] = nbhdRange;
		}
	}


	
	// fill in with cellID of neighbour or invalid
	for (int k = 0; k<3; k++)
	{
		for (int i = 0; i<3; i++)
		{
			for (int j = 0; j<3; j++)
			{
				
				if (inrange(offsetFlags[k*9 + i*3 + j]))
				{
					uint checkCell = myCell + (1-k)*(dimension*dimension) + 
									  (i-1)* dimension + 
									  (j-1);
					uint start = sortedIndexes[checkCell];
					uint end;
					
					if (inrange(checkCell+1))
					{
						end = sortedIndexes[checkCell+1];
					}
					else 
					{
						end = gl_WorkGroupSize.x*gl_NumWorkGroups.x;
					}
					for (uint q = start; q<end; q++)
					{
						Boid other = bIN[q];
						// avoid
						vec4 distance = other.Pos - me.Pos;
						if (dot(distance, distance)<avoidRadius*avoidRadius)
						{
							avoid= avoid - distance.xyz;
							avoided++;
						}
						// group
						if (other.type == me.type && dot(distance, distance) < groupRadius*groupRadius)
						{
							group+=distance.xyz;
							grouped++;
							align+=other.Vel;
							aligned++;
						}
						// avoid other flock
						if (other.type != me.type && dot(distance, distance)<otherAvoidRadius*otherAvoidRadius)
						{
							otherAvoid = otherAvoid - distance.xyz;
							otherAvoided++;
						}
						

					}
					if (avoidObstacles>0.5)
					{
						for (uint i = 0; i<16; i++)
						{
							Obstacle ob = oIN[i];
							vec3 distance = ob.position - me.Pos.xyz;
							if (length(distance)<ob.radius)
							{
								obstacleAvoid = obstacleAvoid - distance.xyz;
								obstacleAvoided++;
							}
						}
					}

					
				}
			}
		}
	
	}
	
	if (avoided!=0)
	avoid = avoid/avoided;
	
	if (otherAvoided!=0)
	otherAvoid = otherAvoid/otherAvoided;
	
	if (obstacleAvoided!=0)
	obstacleAvoid = obstacleAvoid/obstacleAvoided;
	
	group = group/grouped*0.005;
	align = align/aligned;
	
	float inertia = 0.3;
	Boid rez;
	if (me.type > 0.5)
	{
		rez.Vel = clampVectorLength(me.Vel, inertia) + clampVectorLength(avoid+group+align+home+obstacleAvoid*10+otherAvoid, 1 - inertia);
	}
	else
	{
		rez.Vel = clampVectorLength(me.Vel, inertia) + clampVectorLength(avoid+group+align+home+obstacleAvoid*10+otherAvoid, 1 - inertia);
	}
	rez.Pos = vec4(rez.Vel.xyz*dt*speed, 0.0) + me.Pos;
	rez.type = me.type;
	
	if (rez.Pos.x>512.0f)
		rez.Pos.x=rez.Pos.x-512.0f;
	else if (rez.Pos.x<0)
		rez.Pos.x=rez.Pos.x + 512.0f;
	if (rez.Pos.y>512.0f)
		rez.Pos.y=rez.Pos.y-512.0f;
	else if (rez.Pos.y<0)
		rez.Pos.y=rez.Pos.y + 512.0f;
	if (rez.Pos.z>512.0f)
		rez.Pos.z=rez.Pos.z-512.0f;
	else if (rez.Pos.z<0)
		rez.Pos.z=rez.Pos.z + 512.0f;
	bIN[myID] = rez;
}