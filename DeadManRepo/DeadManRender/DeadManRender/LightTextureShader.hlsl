Texture2D tex : register(t0);
//Texture2D normMap : register(t1);
SamplerState sam : register(s0);

struct PixelShaderInput
{
	float4 worldPos : WORLDPOS;
	float4 pos : SV_POSITION;
	float4 uv : UV;
	float4 color : COLOR;
	float4 norm : NORMAL;
};

struct genLight
{
	float4 pos;
	float4 dir;
	float4 color;
	float4 specColor;
	float4 camera;
	float4 coneRatio;
};

cbuffer Lights : register(b0)
{
	genLight lights[10];
}

float4 applySpecular(float4 _camPos, float _specularPower, float4 _surfacePos, float4 _lightDir, float4 _surfaceNorm, float4 _lightColor, float _specularHardness)
{
	_lightDir = -_lightDir;
	float3 viewDir = normalize(_camPos.xyz - _surfacePos.xyz);
	float NdotL = dot(normalize(_surfaceNorm.xyz), normalize(_lightDir.xyz));
	float intensity = saturate(NdotL);
	float3 H = normalize(normalize(_lightDir.xyz) + viewDir.xyz);
	float NdotH = dot(normalize(_surfaceNorm.xyz), H);
	intensity = pow(saturate(NdotH), _specularPower);
	return intensity * _lightColor * _specularHardness;
}

float calcAttenuation(float4 _position, float4 _worldPos, float _radius)
{
	float attenuation = 1 - saturate(length(_position.xyz - _worldPos.xyz) / _radius);
	attenuation = attenuation * attenuation;
	return attenuation;
}

float4 directionalLight(float4 _direction, float4 _surfaceNormal, float4 _lightColor)
{
	float ratio = saturate(dot(normalize(-_direction.xyz), _surfaceNormal.xyz) * _lightColor.w);
	return ratio * _lightColor;
}

float4 pointLight(float4 _position, float4 _worldPos, float4 _surfaceNormal, float4 _lightColor)
{
	float4 lightDir = float4(normalize(_position.xyz - _worldPos.xyz), 1);
	float ratio = saturate(dot(lightDir.xyz, _surfaceNormal.xyz) * _lightColor.w);
	return ratio * _lightColor;
}

float4 spotLight(float4 _position, float4 _worldPos, float4 _direction, float _coneRatio, float4 _surfaceNormal, float _attenuation, float4 _lightColor)
{
	float4 lightDir = float4(normalize(_position.xyz - _worldPos.xyz), 1);
	float surfaceRatio = saturate(dot(lightDir.xyz, normalize(_direction.xyz)));
	float spotFactor = saturate((surfaceRatio - _coneRatio) * _attenuation);
	float ratio = saturate(dot(normalize(_direction.xyz), _surfaceNormal.xyz) * _lightColor.w);
	float4 spotOut = spotFactor * ratio * _lightColor;
	return spotOut;
}

float4 main(PixelShaderInput input) : SV_TARGET
{

	float4 textureColor = tex.Sample(sam, input.uv.xy);

	//float4 normal = normMap.Sample(sam, input.uv.xy);
	//input.norm = float4(normalize(input.norm.xyz + normal.xyz), 1);
	float alpha = textureColor.w;

	float4 lightColor = float4(0, 0, 0, 0);
	bool end = false;
	for (int i = 0; !end && i < 10; i++)
	{
		switch (lights[i].pos.w)
		{
		case 0:
		{
			end = true;
			break;
		}
		case 1: // ambient
		{
			lightColor += float4(lights[i].color.xyz * lights[i].color.w, 1);
			break;
		}
		case 2: // directional
		{
			lightColor += directionalLight(lights[i].dir, input.norm, lights[i].color);
			lightColor += applySpecular(lights[i].camera, lights[i].camera.w, input.worldPos, float4(normalize(lights[i].dir.xyz), 1), input.norm, lights[i].specColor, lights[i].specColor.w);
			break;
		}
		case 3: // point
		{
			float attenuation = calcAttenuation(lights[i].pos, input.worldPos, lights[i].dir.w);
			float4 lightDir = float4(lights[i].pos.xyz - input.worldPos.xyz, 1);
			lightColor += attenuation * pointLight(lights[i].pos, input.worldPos, input.norm, lights[i].color);
			lightColor += attenuation * applySpecular(lights[i].camera, lights[i].camera.w, input.worldPos, lightDir, input.norm, lights[i].specColor, lights[i].specColor.w);
			break;
		}
		case 4: // spot
		{
			lightColor += spotLight(lights[i].pos, input.worldPos, lights[i].dir, lights[i].coneRatio.y, input.norm, lights[i].coneRatio.x, lights[i].color);
			lightColor += applySpecular(lights[i].camera, lights[i].camera.w, input.worldPos, float4(normalize(lights[i].dir.xyz), 1), input.norm, lights[i].specColor, lights[i].specColor.w);
			break;
		}
		}
	}

	float4 returnColor = lightColor * textureColor;
	return float4(returnColor.xyz, alpha);
}