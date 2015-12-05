/** @file asteroid.cpp
 *  @brief This file contains the definition of the Asteroid class.
 */

#include "asteroid.h"
#include <chrono>
#include <random>
#include <cmath>

unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
std::default_random_engine generator (seed);
std::uniform_int_distribution<int> startingSide(1,4);
std::uniform_int_distribution<int> startingDirection(-1,1);
std::normal_distribution<double> startingTile(11, 6);

/**
 * @brief Asteroid::Asteroid is the default constructor for the Asteroid class.
 * Sets the image on an asteriod and sets its initial location and direction of motion.
 */
Asteroid::Asteroid() {
    astLabel = new QLabel;
    astLabel->setPixmap(QPixmap(":/images/asteroid.png"));
    astLabel->setScaledContents(true);
    setState();
}

/**
 * @brief Asteroid::setState sets the initial location and direction of
 * motion for an asteroid. This function is called when an asteroid is created
 * and when its location needs to be reset (after colliding with an attack).
 */
void Asteroid::setState() {
        int side = startingSide(generator);
        switch(side) {
        case(1): {
            // top of grid
            x = 0;
            y = std::abs((int)startingTile(generator)) % 23;

            xDir = 1;
            yDir = startingDirection(generator);

            break;
        }
        case(2): {
            // left of grid
            x = std::abs((int)startingTile(generator)) % 23;
            y = 0;

            xDir = startingDirection(generator);
            yDir = 1;

            break;
        }
        case(3): {
            // bottom of grid
            x = 22;
            y = std::abs((int)startingTile(generator)) % 23;

            xDir = -1;
            yDir = startingDirection(generator);

            break;
        }
        case(4): {
            // right of grid
            x = std::abs((int)startingTile(generator)) % 23;
            y = 22;

            xDir = startingDirection(generator);
            yDir = -1;

            break;
        }
    }
}

/**
 * @brief Asteroid::validPosition checks that the asteroid has not left the gameboard
 * @return true if the asteroid has not left the gameboard, false otherwise
 */
bool Asteroid::validPosition() const {

    // we need the topleft corner of our asteroid image to be on the grid
    return x > -1 && x < 22 && y > -1 && y < 22;
}

/**
 * @brief Asteroid::getX gets the x coordinate of the asteroid
 * @return tbe x coordinate
 */
int Asteroid::getX() const {
    return x;
}

/**
 * @brief Asteroid::getY gets the y coordinate of the asteroid
 * @return the y coordinate
 */
int Asteroid::getY() const {
    return y;
}

/**
 * @brief Asteroid::setX sets the x coordinate of the asteroid
 * @param x is the value to which we are setting the x coordinate
 */
void Asteroid::setX(int x) {
    this->x = x;
}

/**
 * @brief Asteroid::setY sets the y coordinate of the asteriod
 * @param y is the value to which we are setting the y coordinate
 */
void Asteroid::setY(int y) {
    this->y = y;
}

/**
 * @brief Asteroid::getXdir gets the direction in the x direction in which
 * the asteroid is moving
 * @return the asteroid's motion with respect to the x-axis
 */
int Asteroid::getXdir() const {
    return xDir;
}

/**
 * @brief Asteroid::getYdir gets the direction in the y direction in which
 * the asteroid is moving
 * @return the asteroid's motion with respect to the y-axis
 */
int Asteroid::getYdir() const {
    return yDir;
}

/**
 * @brief Asteroid::getLabel gets the label associated with the asteroid
 * @return a pointer to the label
 */
QLabel* Asteroid::getLabel() const {
    return astLabel;
}

/**
 * @brief Asteroid::collided_with_ship checks if the asteroid has collided with the ship
 * @return true if a collision has occured, false otherwise
 */
bool Asteroid::collided_with_ship() const {
    return x == 11 && y == 11;
}
