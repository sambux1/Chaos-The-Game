/*
Projectile class file

Chaos The Game
*/

#include "projectile.h"

#include <cmath>

using namespace std;


// given information about the shooter, form a projecile and assign its initial qualities
Projectile::Projectile(double shooterX, double shooterY, int shooterRot, int shooterHeight) {
	// **** possibly change so it shoots from slightly away from the edge of the shooter
	posX = (int) (shooterX + (((shooterHeight / 2)) * sin(shooterRot * (M_PI / 180))) + 0.5);
	posY = (int) (shooterY - (((shooterHeight / 2)) * cos(shooterRot * (M_PI / 180))) + 0.5);
	
	velX = PROJECTILE_SPEED * sin(shooterRot * (M_PI / 180));
	velY = - PROJECTILE_SPEED * cos(shooterRot * (M_PI / 180));
	
	tick_count = 0;
}

Projectile::~Projectile() {
	// nothing to do here
}
