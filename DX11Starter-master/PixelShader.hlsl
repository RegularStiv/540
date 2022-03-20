#include "ShaderIncludes.hlsli"


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
float4 main(VertexToPixel input) : SV_TARGET
{

	input.normal = normalize(input.normal);

	float3 negDirectionNormal = normalize(-lights[0].Direction);
	float3 diffuse =  (Diffuse(input.normal, negDirectionNormal));
	float light = Phong(input.normal, input.worldPosition, MAX_SPECULAR_EXPONENT, roughness, cameraPosition, -negDirectionNormal);
	float3 finalColor = float3(((diffuse  * lights[0].Color * colorTint) + (ambient * colorTint))) + light;

	negDirectionNormal = normalize(-lights[1].Direction);
	diffuse = (Diffuse(input.normal, negDirectionNormal));
	light = Phong(input.normal, input.worldPosition, MAX_SPECULAR_EXPONENT, roughness, cameraPosition, -negDirectionNormal);
	float3 finalColor2 = float3(((diffuse * lights[1].Color * colorTint) + (ambient * colorTint))) + light;

	negDirectionNormal = normalize(-lights[2].Direction);
	diffuse = (Diffuse(input.normal, negDirectionNormal));
	light = Phong(input.normal, input.worldPosition, MAX_SPECULAR_EXPONENT, roughness, cameraPosition, -negDirectionNormal);
	float3 finalColor3 = float3(((diffuse * lights[2].Color * colorTint) + (ambient * colorTint))) + light;

	float3 direction = normalize(lights[3].Position - input.worldPosition);
	diffuse = (Diffuse(input.normal, direction)) * Attenuate(lights[3], input.worldPosition);
	light = Phong(input.normal, input.worldPosition, MAX_SPECULAR_EXPONENT, roughness, cameraPosition, -direction) * Attenuate(lights[3], input.worldPosition);
	float3 finalColor4 = float3(((diffuse * lights[3].Color * colorTint) + (ambient * colorTint))) + light;

	direction = normalize(lights[4].Position - input.worldPosition);
	diffuse = (Diffuse(input.normal, direction)) * Attenuate(lights[4], input.worldPosition);
	light = Phong(input.normal, input.worldPosition, MAX_SPECULAR_EXPONENT, roughness, cameraPosition, -direction) * Attenuate(lights[4], input.worldPosition);
	float3 finalColor5 = float3(((diffuse * lights[4].Color * colorTint) + (ambient * colorTint))) + light;

	return float4(finalColor + finalColor2 + finalColor3 + finalColor4 + finalColor5, 1);

}