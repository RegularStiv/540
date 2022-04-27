#include "ShaderIncludes.hlsli"

Texture2D SurfaceTexture	:		register(t0); // "t" registers for textures
Texture2D SpecularTexture	:		register(t1);
Texture2D NormalTexture		:		register(t2);// "t" registers for textures
Texture2D MetalTexture		:		register(t3);
Texture2D RoughnessTexture	:		register(t4);
SamplerState BasicSampler	:		register(s0); // "s" registers for samplers

cbuffer ExternalData : register(b0)
{
	float4 colorTint;
	float roughness;
	float3 cameraPosition;
	float3 ambient;
	Light lights[5];
}
// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel_NormalMap input) : SV_TARGET
{

	input.normal = normalize(input.normal);
	

	float3 unpackedNormal = NormalTexture.Sample(BasicSampler, input.uv).rgb * 2 - 1;

	// Feel free to adjust/simplify this code to fit with your existing shader(s)
	// Simplifications include not re-normalizing the same vector more than once!
	float3 N = normalize(input.normal); // Must be normalized here or before
	float3 T = normalize(input.tangent); // Must be normalized here or before
	T = normalize(T - N * dot(T, N)); // Gram-Schmidt assumes T&N are normalized!
	float3 B = cross(T, N);
	float3x3 TBN = float3x3(T, B, N);

	// Assumes that input.normal is used later in the shader
	input.normal = normalize(mul(unpackedNormal, TBN)); // Note multiplication order!
	//return float4(input.normal, 1);
	// Adjust the variables below as necessary to work with your own code
	float3 surfaceColor = pow(SurfaceTexture.Sample(BasicSampler, input.uv).rgb, 2.2f);
	surfaceColor *= colorTint;
	float specularScale = SpecularTexture.Sample(BasicSampler, input.uv).r;

	float roughness = RoughnessTexture.Sample(BasicSampler, input.uv).r;
	float metalness = MetalTexture.Sample(BasicSampler, input.uv).r;
	// Specular color determination -----------------
// Assume albedo texture is actually holding specular color where metalness == 1
//
// Note the use of lerp here - metal is generally 0 or 1, but might be in between
// because of linear texture sampling, so we lerp the specular color to match
	float3 specularColor = lerp(F0_NON_METAL.rrr, surfaceColor.rgb, metalness);



	//directional lights
	float3 negDirectionNormal = normalize(-lights[0].Direction);

	float diffuse =  (DiffusePBR(input.normal, negDirectionNormal));
	float3 spec = MicrofacetBRDF(input.normal, negDirectionNormal, normalize(cameraPosition - input.worldPosition), roughness, specularColor);
	float3 balancedDiff = DiffuseEnergyConserve(diffuse, spec, metalness);

	float3 finalColor = (balancedDiff * surfaceColor + spec) * lights[0].Intensity * lights[0].Color;

	negDirectionNormal = normalize(-lights[1].Direction);
	diffuse = (DiffusePBR(input.normal, negDirectionNormal));
	spec = MicrofacetBRDF(input.normal, negDirectionNormal, normalize(cameraPosition - input.worldPosition), roughness, specularColor);
	float3 finalColor2 = (balancedDiff * surfaceColor + spec) * lights[1].Intensity * lights[1].Color;

	negDirectionNormal = normalize(-lights[2].Direction);
	diffuse = (DiffusePBR(input.normal, negDirectionNormal));
	spec = MicrofacetBRDF(input.normal, negDirectionNormal, normalize(cameraPosition - input.worldPosition), roughness, specularColor);
	float3 finalColor3 = (balancedDiff * surfaceColor + spec) * lights[2].Intensity * lights[2].Color;


	//point lights
	float3 direction = normalize(lights[3].Position - input.worldPosition);
	diffuse = (DiffusePBR(input.normal, direction)) * Attenuate(lights[3], input.worldPosition);
	spec = MicrofacetBRDF(input.normal, direction, normalize(cameraPosition - input.worldPosition), roughness, specularColor);
	float3 finalColor4 = (balancedDiff * surfaceColor + spec) * lights[3].Intensity * lights[3].Color;

	direction = normalize(lights[4].Position - input.worldPosition);
	diffuse = (DiffusePBR(input.normal, direction)) * Attenuate(lights[4], input.worldPosition);
	spec = MicrofacetBRDF(input.normal, direction, normalize(cameraPosition - input.worldPosition), roughness, specularColor);
	float3 finalColor5 = (balancedDiff * surfaceColor + spec) * lights[4].Intensity * lights[4].Color;

	//add all lights
	float3 totalColor = finalColor + finalColor2 + finalColor3 + finalColor4 + finalColor5;

	//return all lights with gamma correction
	return float4(pow(totalColor, 1.0f / 2.2f), 1);

}