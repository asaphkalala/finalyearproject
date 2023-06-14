#version 460 core
#define MAX_VERTICES_PER_STRAND 50
#define FTL 0
#define FILL_VOLUMES 1
#define COLLISIONS 2

#define ELLIPSOID_COUNT 7
#define VOLUME_UPPER_LIMIT 10

layout (local_size_x = 128) in;

layout (std430, binding = 0) buffer HairPosition {
	float positions[][3];
};

layout (std430, binding = 1) buffer HairVelocity {
	float velocities[][3];
};

layout (std430, binding = 2) buffer volumeDensity {
	int volumeDensities[11][11][11];
};

layout (std430, binding = 3) buffer volumeVelocity {
	int volumeVelocities[11][11][11][3];
};

struct HairData {
	uint particlesPerStrand;
	uint strandCount;
	float particleMass;
	float segmentLength;
};


uniform mat4 ellipsoids[ELLIPSOID_COUNT]; // 7 ellipsoids
uniform float ellipsoidRadius; 
uniform mat4 model;
uniform float curlRadius = 0.05f; 
uniform uint state; // 0: FTL, 1: Fill volumes, 2: Collisions
uniform Force force; // gravity
uniform HairData hairData;
uniform float deltaTime;
uniform float runningTime;
uniform float vdampCoeffic = 0.90;
uniform float frictionCoeffic = 0.0;

vec3 followTheLeader(in vec3 leaderParticlePosition, in vec3 proposedParticlePosition, out vec3 positionCorrectionVector) // Follow The Leader Algorithm
{
	const vec3 direction = normalize(proposedParticlePosition - leaderParticlePosition);
	vec3 fixedPosition = leaderParticlePosition + (direction * hairData.segmentLength);
	positionCorrectionVector = fixedPosition - proposedParticlePosition;
	return fixedPosition;
}
{
	const vec3 direction = normalize(proposedParticlePosition - leaderParticlePosition);
	vec3 fixedPosition = leaderParticlePosition + (direction * hairData.segmentLength);
	positionCorrectionVector = fixedPosition - proposedParticlePosition;
	return fixedPosition;
}

vec3 generateGravityForce() // Gravity
{
	return hairData.particleMass * vec3(0.0, force.gravity, 0.0); // F = mg 
}

vec3 integrateExplicitEuler(in vec3 forces, in vec3 particlePosition, in vec3 particleVelocity) // Explicit Euler
{
	const vec3 acceleration = forces / hairData.particleMass;
	return (particlePosition + (particleVelocity * deltaTime) + (acceleration * deltaTime * deltaTime));
}
{
	const vec3 acceleration = forces / hairData.particleMass; // a = F/m 
	return (particlePosition + (particleVelocity * deltaTime) + (acceleration * deltaTime * deltaTime)); // x(t+1) = x(t) + v(t)dt + 1/2a(t)dt^2
}

vec3 integrateHeun(in vec3 forces, in vec3 particlePosition, in vec3 particleVelocity) 
{
	const vec3 acceleration = forces / hairData.particleMass;

	const vec3 firstVelocity = particleVelocity + deltaTime * acceleration;
	const vec3 firstPosition = particlePosition + deltaTime * firstVelocity;

	return (particlePosition + deltaTime * ((firstVelocity + secondVelocity) / 2)); 
}

vec3 updateVelocity(in vec3 oldPosition, in vec3 newPosition) /
{
	return ((newPosition - oldPosition) / deltaTime);
}

vec3 updatePosition(in vec3 oldPosition, in vec3 newPosition) 
{
	return newPosition;
}
vec3 interpolateVelocity(in vec3 particlePosition)
{
	particlePosition += (VOLUME_UPPER_LIMIT / 2);
	ivec3 flooredCoords = ivec3(floor(particlePosition));

	// Upper limit of the regular voxel grid, flooring to 9 to avoid out of bounds access
	if (flooredCoords.x >= VOLUME_UPPER_LIMIT) flooredCoords.x = VOLUME_UPPER_LIMIT - 1;
	if (flooredCoords.y >= VOLUME_UPPER_LIMIT) flooredCoords.y = VOLUME_UPPER_LIMIT - 1;
	if (flooredCoords.z >= VOLUME_UPPER_LIMIT) flooredCoords.z = VOLUME_UPPER_LIMIT - 1;

	vec3 voxelVertexVelocities[2][2][2]; // 8 vertices of the voxel cube
	for (uint i = 0; i < 2; ++i)
	{
		for (uint j = 0; j < 2; ++j)
		{
			for (uint k = 0; k < 2; ++k)
			{
				voxelVertexVelocities[i][j][k].x = volumeVelocities[flooredCoords.x + i][flooredCoords.y + j][flooredCoords.z + k][0];
				voxelVertexVelocities[i][j][k].y = volumeVelocities[flooredCoords.x + i][flooredCoords.y + j][flooredCoords.z + k][1];
				voxelVertexVelocities[i][j][k].z = volumeVelocities[flooredCoords.x + i][flooredCoords.y + j][flooredCoords.z + k][2];
				if (volumeDensities[flooredCoords.x + i][flooredCoords.y + j][flooredCoords.z + k] != 0)
					voxelVertexVelocities[i][j][k] /= float(volumeDensities[flooredCoords.x + i][flooredCoords.y + j][flooredCoords.z + k]);
			}
		}
	}

	// Trilinear interpolation to obtain velocity at particle position
	
	const float tx = abs(particlePosition.x - flooredCoords.x);
	const float ty = abs(particlePosition.y - flooredCoords.y);
	const float tz = abs(particlePosition.z - flooredCoords.z);

	// 4 linear interpolations on x-axis 
	const vec3 xPoint1 = tx * voxelVertexVelocities[0][0][0] + (1 - tx) * voxelVertexVelocities[1][0][0];
	const vec3 xPoint2 = tx * voxelVertexVelocities[0][0][1] + (1 - tx) * voxelVertexVelocities[1][0][1];
	const vec3 xPoint3 = tx * voxelVertexVelocities[0][1][0] + (1 - tx) * voxelVertexVelocities[1][1][0];
	const vec3 xPoint4 = tx * voxelVertexVelocities[0][1][1] + (1 - tx) * voxelVertexVelocities[1][1][1];

	// 2 linear interpolations on y-axis
	const vec3 yPoint1 = ty * xPoint1 + (1 - ty) * xPoint3;
	const vec3 yPoint2 = ty * xPoint2 + (1 - ty) * xPoint4;

	// 1 linear interpolation on z-axis
	const vec3 zPoint = tz * yPoint1 + (1 - tz) * yPoint2;

	return zPoint;
}

vec3 correctFtlVelocity(in vec3 currentParticleVelocity, in vec3 nextParticleCorrectionVector) 
{
	const vec3 correctedVelocity = currentParticleVelocity + vdampCoeffic * (-nextParticleCorrectionVector / deltaTime);
	return correctedVelocity;
}

void addHairFriction() // Hair friction
{
	if (gl_GlobalInvocationID.x > hairData.strandCount * hairData.particlesPerStrand) // Out of bounds
		return;

	vec3 particlePosition = vec3(positions[gl_GlobalInvocationID.x][0], positions[gl_GlobalInvocationID.x][1], positions[gl_GlobalInvocationID.x][2]); 
	vec3 particleVelocity = vec3(velocities[gl_GlobalInvocationID.x][0], velocities[gl_GlobalInvocationID.x][1], velocities[gl_GlobalInvocationID.x][2]); 
	particleVelocity = (1.0 - frictionCoeffic) * particleVelocity + frictionCoeffic * interpolateVelocity(particlePosition);
	velocities[gl_GlobalInvocationID.x][0] = particleVelocity.x;
	velocities[gl_GlobalInvocationID.x][1] = particleVelocity.y;
	velocities[gl_GlobalInvocationID.x][2] = particleVelocity.z;
} // End of hair friction

void fillVolumes() 
{
	if (gl_GlobalInvocationID.x > hairData.strandCount * hairData.particlesPerStrand) // Out of bounds
		return;

	// Adding 5 to linear map [-5,5] range to [0,10] range
	const vec3 particlePosition = vec3(positions[gl_GlobalInvocationID.x][0], positions[gl_GlobalInvocationID.x][1], positions[gl_GlobalInvocationID.x][2]) + (VOLUME_UPPER_LIMIT / 2); 
	const vec3 particleVelocity = vec3(velocities[gl_GlobalInvocationID.x][0], velocities[gl_GlobalInvocationID.x][1], velocities[gl_GlobalInvocationID.x][2]); 
	ivec3 flooredCoords = ivec3(floor(particlePosition));
	if (flooredCoords.x >= VOLUME_UPPER_LIMIT) flooredCoords.x = VOLUME_UPPER_LIMIT - 1;
	if (flooredCoords.y >= VOLUME_UPPER_LIMIT) flooredCoords.y = VOLUME_UPPER_LIMIT - 1;
	if (flooredCoords.z >= VOLUME_UPPER_LIMIT) flooredCoords.z = VOLUME_UPPER_LIMIT - 1;

	for (uint i = 0; i < 2; ++i) // 2x2x2 voxel cube
	{
		for (uint j = 0; j < 2; ++j)
		{
			for (uint k = 0; k < 2; ++k)
			{
				float densityW = (1.0 - abs(particlePosition.x - flooredCoords.x - i)) * (1.0 - abs(particlePosition.y - flooredCoords.y - j)) * (1.0 - abs(particlePosition.z - flooredCoords.z - k)) * 1000.f;
				int densityWeight = int(densityW);
				atomicAdd(volumeDensities[flooredCoords.x + i][flooredCoords.y + j][flooredCoords.z + k], densityWeight);
				atomicAdd(volumeVelocities[flooredCoords.x + i][flooredCoords.y + j][flooredCoords.z + k][0], int(densityWeight * particleVelocity.x));
				atomicAdd(volumeVelocities[flooredCoords.x + i][flooredCoords.y + j][flooredCoords.z + k][1], int(densityWeight * particleVelocity.y));
				atomicAdd(volumeVelocities[flooredCoords.x + i][flooredCoords.y + j][flooredCoords.z + k][2], int(densityWeight * particleVelocity.z));
			}
		}
	}
}

void resolveBodyCollision(inout vec3 particlePosition) 
{
	for (uint i = 0; i < ELLIPSOID_COUNT; ++i) // Ellipsoid collision so that hair doesn't go through body
	{
		vec3 transformedPosition = vec3(inverse(ellipsoids[i]) * vec4(particlePosition, 1.f));
		if (length(transformedPosition) < ellipsoidRadius) 
		{
			transformedPosition = normalize(transformedPosition) * (ellipsoidRadius + curlRadius);
			particlePosition = vec3(ellipsoids[i] * vec4(transformedPosition, 1.f));
		}
	}
}

void moveParticles()
{
	if (gl_GlobalInvocationID.x > hairData.strandCount) // Out of bounds because of 1 strand per work group
		return; 

	vec3 particlePositions[MAX_VERTICES_PER_STRAND]; // Positions of all particles in a strand
	vec3 particleVelocities[MAX_VERTICES_PER_STRAND]; // Velocities of all particles in a strand

	uint offset = gl_GlobalInvocationID.x * hairData.particlesPerStrand;

	for (uint i = 0; i < hairData.particlesPerStrand; ++i) // Filling arrays with data from global memory
	{
		const uint particleOffset = offset + i;
		// Positions of all particles in a strand
		particlePositions[i].x = positions[particleOffset][0];
		particlePositions[i].y = positions[particleOffset][1];
		particlePositions[i].z = positions[particleOffset][2];

		// Velocities of all particles in a strand
		particleVelocities[i].x = velocities[particleOffset][0];
		particleVelocities[i].y = velocities[particleOffset][1];
		particleVelocities[i].z = velocities[particleOffset][2];
	}

	particlePositions[0] = vec3(model * vec4(particlePositions[0], 1.f));

	vec3 forces, proposedPosition;
	vec3 positionCorrectionVector[MAX_VERTICES_PER_STRAND];
	for (uint i = 1; i < hairData.particlesPerStrand; ++i) 
	{
		forces = vec3(0.f);
		forces += generateGravityForce();
		proposedPosition = integrateExplicitEuler(forces, particlePositions[i], particleVelocities[i]); // Explicit Euler
		proposedPosition = followTheLeader(particlePositions[i - 1], proposedPosition, positionCorrectionVector[i]); // Follow the leader
		resolveBodyCollision(proposedPosition); // Ellipsoid collision
		particleVelocities[i] = updateVelocity(particlePositions[i], proposedPosition); // Update velocity
		particlePositions[i] = proposedPosition;
	}

	for (uint i = 1; i < hairData.particlesPerStrand - 1; ++i)
	{
		particleVelocities[i] = correctFtlVelocity(particleVelocities[i], positionCorrectionVector[i + 1]); // Correct velocity
	}

	for (uint i = 1; i < hairData.particlesPerStrand; ++i) // Filling arrays with data from global memory
	{
		const uint particleOffset = offset + i;
		positions[particleOffset][0] = particlePositions[i].x;
		positions[particleOffset][1] = particlePositions[i].y;
		positions[particleOffset][2] = particlePositions[i].z;

		velocities[particleOffset][0] = particleVelocities[i].x;
		velocities[particleOffset][1] = particleVelocities[i].y;
		velocities[particleOffset][2] = particleVelocities[i].z;
	}
	{
		const uint particleOffset = offset + i; // Last particle
		positions[particleOffset][0] = particlePositions[i].x;
		positions[particleOffset][1] = particlePositions[i].y;
		positions[particleOffset][2] = particlePositions[i].z;

		velocities[particleOffset][0] = particleVelocities[i].x;
		velocities[particleOffset][1] = particleVelocities[i].y;
		velocities[particleOffset][2] = particleVelocities[i].z;
	}
}

void main(void)
{
	switch (state)
	{
		case FTL:
			moveParticles();
			break;

		case FILL_VOLUMES:
			fillVolumes();
			break;

		case COLLISIONS:
			addHairFriction();
			break;
	}
}
