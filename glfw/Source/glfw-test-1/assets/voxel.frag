#version 150

uniform mat4 model;

struct Light{
	vec3 position;
	vec3 intensities;
};

struct Material{
	int type;
};

uniform mat4 M;
uniform mat4 VP;

uniform Light lights[5];
uniform int numLights;

uniform sampler2D textureSampler[5];
uniform int numTextures;

uniform sampler2D shadowMapSampler;

uniform Material materials[5];
uniform int numMaterials;

in vec3 fragVert;
in vec3 fragNormal;
in vec4 fragColor;
in vec2 fragUV;
in vec4 shadowCoord;

out vec4 outColor;

void main()
{
	vec4 fragColorTex = vec4(0,0,0,0);

	if(numTextures == 0){
		fragColorTex = fragColor;
	}

	for(int i = 0; i < numTextures; i++){
		if(i == 0){
			fragColorTex = texture(textureSampler[i], fragUV).rgba;
		}else{
			fragColorTex = mix(fragColorTex, texture(textureSampler[i], fragUV).rgba, 0.5);
		}
	}

    mat3 normalMatrix = transpose(inverse(mat3(model)));

	vec3 normal = normalize(normalMatrix * fragNormal);

	vec3 fragWorldPosition = vec3(model * vec4(fragVert, 1));

	float brightness = 0;
	vec3 outIntensities = vec3(0,0,0);

	for(int i = 0; i < numLights; i++){
		vec3 surfaceToLight = lights[i].position - fragWorldPosition;
		brightness += dot(normal, surfaceToLight) / (length(surfaceToLight) * length(normal));
		outIntensities += vec3(lights[i].intensities);
	}
	
	brightness = clamp(brightness, 0.1, 1);

 	float visibility = 1.0;

	vec3 ProjCoords = shadowCoord.xyz / shadowCoord.w;
    vec2 UVCoords;
    UVCoords.x = 0.5 * ProjCoords.x + 0.5;
    UVCoords.y = 0.5 * ProjCoords.y + 0.5;
    float z = 0.5 * ProjCoords.z + 0.5;
    float Depth = texture(shadowMapSampler, UVCoords).x;
    if (Depth < (z - 0.0005)){
        visibility = 0.5;
	}

	outColor =  vec4((brightness) * vec3(outIntensities), 1) * vec4(vec3(fragColorTex.xyz) * visibility, 1);
}