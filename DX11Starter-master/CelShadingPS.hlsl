#include "ShaderIncludes.hlsli"

Texture2D SurfaceTexture	:		register(t0); // "t" registers for textures
Texture2D CelSpecularTexture:		register(t1);
Texture2D CelTexture		:		register(t2);// "t" registers for textures
Texture2D NormalTexture		:		register(t3);
Texture2D RoughnessTexture	:		register(t4);
SamplerState BasicSampler	:		register(s0); // "s" registers for samplers
SamplerState CelSampler		:		register(s1);

cbuffer ExternalData : register(b0)
{
	float4 colorTint;
	float roughness;
	float3 cameraPosition;
	float3 ambient;
	Light lights[5];

	// 0 = no cel shading
	// > 0 different types of cel shading 
	int celShadingType;
}



float4 main(VertexToPixel_NormalMap input) : SV_TARGET
{
	input.normal = normalize(input.normal);


	input.normal = NormalMapping(NormalTexture, BasicSampler, input.uv, input.normal, input.tangent);

	
	//return float4(input.normal, 1);
	// Adjust the variables below as necessary to work with your own code
	float4 surfaceColor = pow(SurfaceTexture.Sample(BasicSampler, input.uv), 2.2f);
	surfaceColor.rgb *= colorTint;


	float roughness = RoughnessTexture.Sample(BasicSampler, input.uv).r;


	float3 finalLight = ambient * surfaceColor.rgb;
	for (int i = 0; i < 5; i++) {

		Light currentLight = lights[i];
		currentLight.Direction = normalize(currentLight.Direction);
		float3 negDirectionNormal = float3(0, 0, 0);
		float attenuate = 1.0f;
		input.normal = NormalMapping(NormalTexture, BasicSampler, input.uv, input.normal, input.tangent);

		if (currentLight.Type == LIGHT_TYPE_DIRECTIONAL) {
			negDirectionNormal = normalize(-lights[i].Direction);
			
		}
		else if (currentLight.Type == LIGHT_TYPE_POINT) {
			negDirectionNormal = normalize(currentLight.Position - input.worldPosition);
			attenuate = Attenuate(currentLight, input.worldPosition);
		}
		float diffuse = (Diffuse(input.normal, negDirectionNormal));
		float phongLight = Phong(input.normal, input.worldPosition, MAX_SPECULAR_EXPONENT, roughness, cameraPosition, -negDirectionNormal);
		
			diffuse = CelTexture.Sample(CelSampler, float2(diffuse, 0)).r;

			phongLight = CelSpecularTexture.Sample(CelSampler, float2(phongLight, 0)).r;
		finalLight += (diffuse * surfaceColor.rgb + phongLight) * attenuate * currentLight.Intensity * currentLight.Color;
	}

	
	//return all lights with gamma correction
	return float4(pow(finalLight, 1.0f / 2.2f), 1);
}


