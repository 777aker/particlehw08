#version 150

uniform sampler2D circ;

in vec4 Color;
in vec2 Tex2D;
out vec4 FragColor;

void main() {
	vec4 texcolor = texture2D(circ, Tex2D);
	if(texcolor.y - texcolor.x >= .2)
		discard;
	else
		FragColor = texture2D(circ, Tex2D) * Color;
}