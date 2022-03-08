// nothing really yet just draws ground
#version 120

uniform float time;
out vec4 Color;

void main(void) {
	// base how bright it is based on y position so if it's deep dark shallow light makes it look much
	// better and can actually see the depth
	Color = vec4(0,0,.8+sin(time+gl_Vertex.x)/5,1);
	// simply draw our ocean and make it wavey
	gl_Position = gl_ModelViewProjectionMatrix*(gl_Vertex + vec4(0,sin(time + gl_Vertex.x),0,0));
}