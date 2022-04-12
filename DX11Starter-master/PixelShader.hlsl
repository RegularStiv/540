#include "ShaderIncludes.hlsli"

Texture2D SurfaceTexture	:		register(t0); // "t" registers for textures
Texture2D SpecularTexture	:		register(t1);
Texture2D NormalTexture		:		register(t2);// "t" registers for textures
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
	// Adjust the variables below as necessary to work with your own code
	float3 surfaceColor = SurfaceTexture.Sample(BasicSampler, input.uv).rgb;
	surfaceColor *= colorTint;
	float specularScale = SpecularTexture.Sample(BasicSampler, input.uv).r;

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
	//return float4(specularScale, 0, 0, 1);
	float3 negDirectionNormal = normalize(-lights[0].Direction);
	float3 diffuse =  (Diffuse(input.normal, negDirectionNormal));
	float light = Phong(input.normal, input.worldPosition, MAX_SPECULAR_EXPONENT, roughness, cameraPosition, -negDirectionNormal) * specularScale;
	float3 finalColor = float3(((diffuse  * lights[0].Color * surfaceColor) + (ambient * surfaceColor))) + light;

	negDirectionNormal = normalize(-lights[1].Direction);
	diffuse = (Diffuse(input.normal, negDirectionNormal));
	light = Phong(input.normal, input.worldPosition, MAX_SPECULAR_EXPONENT, roughness, cameraPosition, -negDirectionNormal) * specularScale;
	float3 finalColor2 = float3(((diffuse * lights[1].Color * surfaceColor) + (ambient * surfaceColor))) + light;

	negDirectionNormal = normalize(-lights[2].Direction);
	diffuse = (Diffuse(input.normal, negDirectionNormal));
	light = Phong(input.normal, input.worldPosition, MAX_SPECULAR_EXPONENT, roughness, cameraPosition, -negDirectionNormal) * specularScale;
	float3 finalColor3 = float3(((diffuse * lights[2].Color * surfaceColor) + (ambient * surfaceColor))) + light;

	float3 direction = normalize(lights[3].Position - input.worldPosition);
	diffuse = (Diffuse(input.normal, direction)) * Attenuate(lights[3], input.worldPosition);
	light = Phong(input.normal, input.worldPosition, MAX_SPECULAR_EXPONENT, roughness, cameraPosition, -direction) * Attenuate(lights[3], input.worldPosition) * specularScale;
	float3 finalColor4 = float3(((diffuse * lights[3].Color * surfaceColor) + (ambient * surfaceColor))) + light;

	direction = normalize(lights[4].Position - input.worldPosition);
	diffuse = (Diffuse(input.normal, direction)) * Attenuate(lights[4], input.worldPosition);
	light = Phong(input.normal, input.worldPosition, MAX_SPECULAR_EXPONENT, roughness, cameraPosition, -direction) * Attenuate(lights[4], input.worldPosition) * specularScale;
	float3 finalColor5 = float3(((diffuse * lights[4].Color * surfaceColor) + (ambient * surfaceColor))) + light;

	return float4(finalColor + finalColor2 + finalColor3 + finalColor4 + finalColor5, 1);

}