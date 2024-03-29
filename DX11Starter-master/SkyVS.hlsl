
cbuffer ExternalData : register(b0)
{
	matrix view;
	matrix projection;
}

struct VertexToPixel_Sky
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 position		:	SV_POSITION;	// XYZW position (System Value Position)
	float3 sampleDir	:	DIRECTION;

};

struct VertexShaderInput
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float3 position			: POSITION;     // XYZ position
	float3 normal			: NORMAL;
	float2 uv				: TEXCOORD;
	float3 tangent			: TANGENT;
};

VertexToPixel_Sky main(VertexShaderInput input)
{
	VertexToPixel_Sky output;
	matrix noTransform = view;
	noTransform._14 = 0;
	noTransform._24 = 0;
	noTransform._34 = 0;
	matrix viewProj = mul(projection, noTransform);

	output.position = mul(viewProj, float4(input.position,1));

	output.position.z = output.position.w;

	output.sampleDir = input.position;
	return output;
}