TextureCube	CubeTexture		:		register(t0); // "t" registers for textures
SamplerState BasicSampler	:		register(s0); // "s" registers for samplers

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



float4 main(VertexToPixel_Sky input) : SV_TARGET
{
	return CubeTexture.Sample(BasicSampler, input.sampleDir);
}