#ifndef __GGP_SHADER_INCLUDES__ // Each .hlsli file needs a unique identifier! 
#define __GGP_SHADER_INCLUDES__ 

// ALL of your code pieces (structs, functions, etc.) go here! 

#endif
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
	//float4 color			: COLOR;        // RGBA color
	float3 normal			: NORMAL;
	float2 uv				: TEXCOORD;
};
float random(float2 s)
{
	return frac(sin(dot(s, float2(12.9898, 78.233))) * 43758.5453123);
}