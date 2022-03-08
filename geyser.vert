#version 120

attribute vec3 Vel;
uniform float time;
attribute float Start;

void main(void) {
	// particle location
	vec4 vert = gl_Vertex;

	// time 
	float t = mod(time, 6.0) - Start;

	// if it isn't time yet make it transparent
	if(t < 0.0) {
		gl_FrontColor = vec4(0,0,0,0);
	} else {
		// set particle color and move it
		gl_FrontColor = gl_Color;
		vert += t * vec4(Vel, 0);
		vert.y -= 4.9*t*t;
		// if it's roughly below the waves remove the particle
		if(vert.y < sin(time + gl_Vertex.x))
			gl_FrontColor = vec4(0,0,0,0);
		// trying to make it darker based on distance
		//gl_FrontColor /= length(gl_Vertex) / 100;
		// done
		gl_Position = vert;
	}
}