/*
* HW08: Particle Systems
* 
* Kelley Kelley
* CSCI 5239 Spring 2022
* 
* Key Bindings:
* 	arrows - change view
	0 - reset view
	esc - quit
	pgup/pgdn - zoom
*/

#include "x239lib/CSCIx239.h"
#include "perlin.h"
int zh = 0; // light position
float lh = 0; // light height
int th = 30, ph = 20; // view angles
int fov = 57; // field of view
int tex; // texture
float asp = 1; // aspect ratio
float dim = 7; // size of world
int perlin_shader; // perlin shader
int show_shader; // snow shader
int mode = 3; // mode

const int size = 512; // ground size
float ground_points[512 * 512 * 3]; // ground texture

int geyser_shader; // geyser shader

// geyser particles
// num particles
const int part_size = 10000;
float geyser_vertices[10000*3];
float geyser_colors[10000*3];
float geyser_velocities[10000*3];
float starttime[10000];
#define VELOCITY_ARRAY 4
#define START_TIME 5
static char* Name[] = { "","","","","Vel","Start",NULL};

// i need a special rand for openmp bc otherwise a bunch of threads are the same
// ie rand() unreliable
// http://www.science-and-fiction.org/rendering/noise.html
float mprand(int xcor, int ycor, float seed, float rng, float off) {
	float rand = (xcor + seed) * 12.9898 + (ycor * seed) * 78.233;
	rand = Sin(rand) * 43758.5453;
	rand = rand - floor(rand);
	rand = rand * rng + off;
	return rand;
}

// initialize particles
void initparticles() {
	// some base variables
	float middlex = 0;
	float middlez = 0;
	float basestart = 0;
	float seed = glfwGetTime();

	// create 10 groups of particles
	// base variables so that a set of particles
	// could be based in the same area
	// I want groups of 100 particles so divide total size by 100 for
	// how many groups we have
	// I'm not sure this works this way, but I kinda wanna try
	// hehe, it totally works this way
	#pragma omp parallel for
	for (int j = 0; j < part_size / 100; j++) {
		// set this groups center
		middlex = mprand(j, seed, 1, size / 2.0, -size / 4.0);
		middlez = mprand(seed, j, 2, size / 2.0, -size / 4.0);
		basestart = mprand(j, seed, 4, 2, 1);
		//middlex = frand(size / 4.0, -size / 8.0);
		//middlez = frand(size / 4.0, -size / 8.0);
		//basestart = frand(2, 1);
		// create a bunch of particles and populate the arrays
		#pragma omp parallel for
		// groups are 100 big so just iterate through 100 for this group
		for (int i = 0; i < 100; i++) {
			// our placement in the array
			int point = j * 100 * 3 + i * 3;
			// set vertices
			geyser_vertices[point] = middlex + mprand(i, j, seed, .5, -.25); // x
			geyser_vertices[point + 1] = mprand(i, j, seed+1, .5, -.25); // y
			geyser_vertices[point + 2] = middlez + mprand(i, j, seed+2, .5, -.25); // z
			// set colors
			geyser_colors[point] = mprand(i, j, seed+3, .2, .1); // r
			geyser_colors[point + 1] = 0; // g
			geyser_colors[point + 2] = mprand(i, j, seed+4, .6, .2); // b
			// set velocities
			geyser_velocities[point] = mprand(i, j, seed+5, 4, -2); // dx
			geyser_velocities[point + 1] = mprand(i, j, seed+6, 2, 9); // dy
			geyser_velocities[point + 2] = mprand(i, j, seed+7, 4, -2); // dz
			// set start time
			starttime[j * 100 + i] = basestart + mprand(i, j, seed+8, 1, 0);
		}
	}
}

// refresh display
void display(GLFWwindow* window) {
	// erase the window and depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// enable z-buffering
	glEnable(GL_DEPTH_TEST);
	// set view
	View(th, ph, fov, dim);
	// enable lighting
	// actually we don't need it commenting out for now
	//Lighting(3 * Cos(zh), 3 * Sin(zh), 0, 0.3, 0.5, 0.8);

	// select shader
	glUseProgram(perlin_shader);
	// pass time
	int id = glGetUniformLocation(perlin_shader, "time");
	glUniform1f(id, glfwGetTime());
	// draw ground
	glPointSize(2);
	// point vertex location to array
	glVertexPointer(3, GL_FLOAT, 0, ground_points);
	// enable vertex arrays
	glEnableClientState(GL_VERTEX_ARRAY);
	// draw it
	glDrawArrays(GL_POINTS, 0, size * size);
	// disable it
	glDisableClientState(GL_VERTEX_ARRAY);

	// time for geysers
	glUseProgram(geyser_shader);
	// every so often move our geysers
	if ((int)floor(glfwGetTime()) % 6 == 0)
		initparticles();
	// send time to shader
	id = glGetUniformLocation(geyser_shader, "time");
	glUniform1f(id, glfwGetTime());
	glPointSize(4);
	// point to vertices and colors
	glVertexPointer(3, GL_FLOAT, 0, geyser_vertices);
	glColorPointer(3, GL_FLOAT, 0, geyser_colors);
	// point attributes
	glVertexAttribPointer(VELOCITY_ARRAY, 3, GL_FLOAT, GL_FALSE, 0, geyser_velocities);
	glVertexAttribPointer(START_TIME, 1, GL_FLOAT, GL_FALSE, 0, starttime);
	// enable arrays
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableVertexAttribArray(VELOCITY_ARRAY);
	glEnableVertexAttribArray(START_TIME);
	// draw
	glDrawArrays(GL_POINTS, 0, part_size);
	// disable stuff
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableVertexAttribArray(VELOCITY_ARRAY);
	glDisableVertexAttribArray(START_TIME);

	// turn off shader
	glUseProgram(0);
	//  Display parameters
	glWindowPos2i(5, 5);
	Print("FPS=%d Dim=%.1f",
		FramesPerSecond(), dim);
	//  Render the scene and make it visible
	ErrCheck("display");
	glFlush();
	glfwSwapBuffers(window);
}

// key pressed
void key(GLFWwindow* window, int key, int scancode, int action, int mods) {
	// discard key releases
	if (action == GLFW_RELEASE) return;

	// exit on ESC
	switch (key) {
		// quit
	case GLFW_KEY_ESCAPE:
		glfwSetWindowShouldClose(window, 1);
		break;
		// reset view
	case GLFW_KEY_0:
		th = ph = 0;
		break;
		// increase/descrease asimuth
	case GLFW_KEY_RIGHT:
		th += 5;
		break;
	case GLFW_KEY_LEFT:
		th -= 5;
		break;
	case GLFW_KEY_UP:
		ph += 5;
		break;
	case GLFW_KEY_DOWN:
		ph -= 5;
		break;
	case GLFW_KEY_PAGE_DOWN:
		dim += 0.1;
		break;
	case GLFW_KEY_PAGE_UP:
		dim = dim > 1 ? dim - 0.1 : dim;
		break;
	}

	// wrap angles
	th %= 360;
	ph %= 360;
	// update projection
	Projection(fov, asp, dim);
}

// window resize callback
void reshape(GLFWwindow* window, int width, int height) {
	// get framebuffer dimensions
	glfwGetFramebufferSize(window, &width, &height);
	// aspect ratio
	asp = (height > 0) ? (double)width / height : 1;
	// set viewport to entire window
	glViewport(0, 0, width, height);
	// set projection
	Projection(fov, asp, dim);
}

// texture storing stuff is my new favorite thing
// so I'mma put the ground in a texture
// wait, I realized a fatal flaw,
// actually several, the main one being this makes no sense
// shift gears, weird ground time later
void perlinvert() {
	int i = 0;
	int j = 0;
	int point;
	float p;
	for (i = 0; i < size; i++) {
		for (j = 0; j < size; j++) {
			point = i * size * 3 + j * 3;
			// imma just go ahead and do all the frequencies in one color
			// bc adding them later in the shader seems a little silly
			p = perlin2d((float)i, (float)j, 2, 10);
			p += perlin2d((float)i, (float)j, 6, 4);
			p += perlin2d((float)i, (float)j, 10, 2);
			// -size/2 so it's in center of screen
			// then all divide by 2 so the points aren't so far apart
			ground_points[point] = ((float)i - size/2.0)/2.0;
			ground_points[point + 1] = p;
			ground_points[point + 2] = ((float)j - size/2.0)/2.0;
		}
	}
	
}

// main program
int main(int argc, char* argv[]) {
	// initialize GLFW
	GLFWwindow* window = InitWindow("Kelley Kelley HW08: Particle System", 1, 600, 600, &reshape, &key);

	// populate vertex array
	perlinvert();

	// initialize particles
	initparticles();

	// ground shader
	perlin_shader = CreateShaderProg("perlin.vert", NULL);

	// geyser shader
	geyser_shader = CreateShaderProgAttr("geyser.vert", NULL,Name);

	// stars?

	// clouds?

	// event loop
	ErrCheck("init");
	while (!glfwWindowShouldClose(window)) {
		// light animation
		zh = fmod(90 * glfwGetTime(), 360);
		// display
		display(window);
		// process any events
		glfwPollEvents();
	}
	// shut down GLFW
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}