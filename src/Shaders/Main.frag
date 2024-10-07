// Example shader used when no shader path is set.

float fmod(float x, float y)
{
	return x - y * trunc(x/y);
}

void mainImage(out vec4 fragColor, in vec2 fragCoord)
{
	float time = iTime / 5f;
	float r = fmod(time, 0.5f);
	float g = fmod(time, 1.0f);
	float b = fmod(time, 2.0f);

	fragColor = vec4(r, g, b, 1.0f);
}