#include "ShaderIncludes.hlsli"

Texture2D SurfaceTexture	:		register(t0); // "t" registers for textures
Texture2D CelSpecularTexture:		register(t1);
Texture2D CelTexture		:		register(t2);// "t" registers for textures
Texture2D NormalTexture		:		register(t3);
Texture2D RoughnessTexture	:		register(t4);
Texture2D MetalMap			:		register(t5);
SamplerState BasicSampler	:		register(s0); // "s" registers for samplers
SamplerState CelSampler		:		register(s1);

cbuffer ExternalData : register(b0)
{
	float4 colorTint;
	float roughness;
	float3 cameraPosition;
	float3 ambient;
	Light lights[5];
}



float4 main(VertexToPixel_NormalMap input) : SV_TARGET
{
	input.normal = normalize(input.normal);


	input.normal = NormalMapping(NormalTexture, BasicSampler, input.uv, input.normal, input.tangent);

	
	//return float4(input.normal, 1);
	// Adjust the variables below as necessary to work with your own code
	float3 surfaceColor = pow(SurfaceTexture.Sample(BasicSampler, input.uv).rgb, 2.2f);
	surfaceColor *= colorTint;
	float roughness = RoughnessTexture.Sample(BasicSampler, input.uv).r;
	float metalness = MetalMap.Sample(BasicSampler, input.uv).r;
	
	float3 specularColor = lerp(F0_NON_METAL.rrr, surfaceColor.rgb, metalness);


	//float3 finalLight = ambient * surfaceColor.rgb;
	//for (int i = 0; i < 5; i++) {

	//	Light currentLight = lights[i];
	//	currentLight.Direction = normalize(currentLight.Direction);
	//	float3 negDirectionNormal = float3(0, 0, 0);
	//	float attenuate = 1.0f;
	//	input.normal = NormalMapping(NormalTexture, BasicSampler, input.uv, input.normal, input.tangent);

	//	if (currentLight.Type == LIGHT_TYPE_DIRECTIONAL) {
	//		negDirectionNormal = normalize(-lights[i].Direction);
	//		
	//	}
	//	else if (currentLight.Type == LIGHT_TYPE_POINT) {
	//		negDirectionNormal = normalize(currentLight.Position - input.worldPosition);
	//		attenuate = Attenuate(currentLight, input.worldPosition);
	//	}
	//	float diffuse = (DiffusePBR(input.normal, negDirectionNormal));
	//	float spec = MicrofacetBRDF(input.normal, negDirectionNormal, normalize(cameraPosition - input.worldPosition), roughness, specularColor);
	//	

	//	//diffuse = CelTexture.Sample(CelSampler, float2(diffuse, 0)).r;
	//	//spec = CelSpecularTexture.Sample(CelSampler, float2(spec, 0)).r;
	//	float3 balancedDiff = DiffuseEnergyConserve(diffuse, spec, metalness);

	//	finalLight += (balancedDiff * surfaceColor.rgb + spec) * attenuate * currentLight.Intensity * currentLight.Color;
	//}

	float3 negDirectionNormal = normalize(-lights[0].Direction);

	float diffuse = (DiffusePBR(input.normal, negDirectionNormal));
	float3 spec = MicrofacetBRDF(input.normal, negDirectionNormal, normalize(cameraPosition - input.worldPosition), roughness, specularColor);
	
	diffuse = CelTexture.Sample(CelSampler, float2(diffuse, 0)).r;
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


