#version 150

in vec2				TexCoord0;
out vec4			oColor;
uniform float keepColorInRange = 0.05;
uniform float radius = 0.5;
uniform sampler2D	uTex0;

vec4 checker(vec4 Color, vec2 uv)
{
    float x = uv.x - radius;
    float y = uv.y - radius;
    float v = x*x + y*y;
    if ( v <= keepColorInRange*keepColorInRange) {
        return Color;
    }

    v = sqrt(v);
    vec4 end = vec4(0, 0, 0, Color.a);
    vec4 u = end - Color;
    float k = (v - keepColorInRange) / (radius - keepColorInRange);
    if (k > 1.0) {
        return end;
    }
    return Color + u * (k * k);
}

void main(void) {
    vec4 texColor = texture( uTex0, TexCoord0 );
    oColor = checker(texColor, TexCoord0);
}