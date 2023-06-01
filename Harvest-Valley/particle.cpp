#include "particle.h"

particle::particle(float x, float y, float z,float lifetime, float size_v, float speed_v, float red_v, float green_v, float blue_v, float acceleration_v, bool type_v) {
	lifespan = lifetime* (float(rand()) / RAND_MAX - 0.5f) * 2 * 2.0f;
	max_lifetime = lifespan;
	size = size_v;
	pos.x = x;
	pos.y = y;
	pos.z = z;
	speed = speed_v + (float(rand()) / RAND_MAX - 0.5f) * 2 * 8.0f;
	speedx= (float(rand()) / RAND_MAX - 0.5f) * 2 * 15.0f;
	speedz= (float(rand()) / RAND_MAX - 0.5f) * 2 * 15.0f;
	angle = (float(rand()) / RAND_MAX - 0.5f) * 2 * 180.0f * 3.14159 / 180.0f;
	red = red_v;
	green = green_v;
	blue = blue_v;
	acceleration = acceleration_v;
	type = type_v;
	dead = false;
}

void particle::draw() {
	if (!dead) {
		glColor3f(red, green, blue);
		glBegin(GL_POLYGON);
		glVertex3f(pos.x - (size/2) * sin(angle), pos.y - size / 2, pos.z - (size / 2) * cos(angle));
		glVertex3f(pos.x + (size / 2) * sin(angle), pos.y - size / 2, pos.z + (size / 2) * cos(angle));
		glVertex3f(pos.x + (size / 2) * sin(angle), pos.y + size / 2, pos.z + (size / 2) * cos(angle));
		glVertex3f(pos.x - (size / 2) * sin(angle), pos.y + size / 2, pos.z - (size / 2) * cos(angle));
		glEnd();
	}
}
void particle::update(float time_elapsed) {
	if (type && speed + acceleration * time_elapsed > 0)
		speed += acceleration * time_elapsed;
	//speedx -= 0.1 * time_elapsed*pow(2,max_lifetime-lifespan);
	//speedz -= 0.1 * time_elapsed * pow(2, max_lifetime - lifespan);
	speedx -= 0.1 * time_elapsed*speedx*2;
	speedz -= 0.1 * time_elapsed* speedz*2;
	pos.y += speed * time_elapsed;
	pos.x += speedx * time_elapsed;
	pos.z += speedz * time_elapsed;
	lifespan -= time_elapsed;
	if (lifespan <= 0)
		dead = true;
}