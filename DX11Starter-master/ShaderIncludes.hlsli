#ifndef __GGP_SHADER_INCLUDES__ // Each .hlsli file needs a unique identifier! 
#define __GGP_SHADER_INCLUDES__ 
#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2
#define MAX_SPECULAR_EXPONENT 256.0f
// ALL of your code pieces (structs, functions, etc.) go here! 


// Struct representing a single vertex worth of data
// - This should match the vertex definition in our C++ code
// - By "match", I mean the size, order and number of members
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexShaderInput
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float3 localPosition	: POSITION;     // XYZ position
	float3 normal			: NORMAL;
	float2 uv				: TEXCOORD;
};
struct Light {
	int Type;
	float3 Direction;
	float Range;
	float3 Position;
	float Intensity;
	float3 Color;
	float SpotFalloff;
	float3 Padding;
};
// Struct representing the data we're sending down the pipeline
// - Should match our pixel shader's input (hence the name: Vertex to Pixel)
// - At a minimum, we need a piece of data defined tagged as SV_POSITION
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 screenPosition	: SV_POSITION;	// XYZW position (System Value Position)
	float3 normal			: NORMAL;
	float3 worldPosition	: POSITION;
	float2 uv				: TEXCOORD;

};
float3 Diffuse(float3 normal, float3 dirToLight) 
{
	return saturate(dot(normal, dirToLight));
}
float Attenuate(Light light, float3 worldPos)
{
	float dist = distance(light.Position, worldPos);
	float att = saturate(1.0f - (dist * dist / (light.Range * light.Range)));
	return att * att;
}
float Phong(float3 normal, float3 pixelWorldPosition, float maxSpec, float roughness, float3 cameraPosition, float3 incomingLightDirection)
{
	float specExponent = (1 - roughness) * maxSpec;
	float3 V = normalize(cameraPosition - pixelWorldPosition);
	float3 R = reflect(incomingLightDirection, normal);
	float spec = pow(saturate(dot(R, V)), specExponent);
	return spec;
}
float random(float2 s)
{
	return frac(sin(dot(s, float2(12.9898, 78.233))) * 43758.5453123);
}

#endif