#version 330 core

out vec4 FragColor;

uniform vec4 lightColor;



float near = 0.05f;
float far = 100.0f;

float linearizeDepth(float depth)
{
    return (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));
}

float logisticDepth(float depth, float steepness = 0.5f, float offset = 5.0f)
{
    float zVal = linearizeDepth(depth);
    return (1 / (1 + exp(-steepness * (zVal - offset))));
}

void main()
{
    float depth = logisticDepth(gl_FragCoord.z);
	FragColor = lightColor * (1.0f - depth) + vec4(depth * vec3(0.07f, 0.13f, 0.17f), 1.0f);
}