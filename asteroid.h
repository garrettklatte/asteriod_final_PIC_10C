/** @file mainwindow.h
 *  @brief Class declaration for the Asteroid class.
 */

#ifndef ASTEROID_H
#define ASTEROID_H

#include <QPoint>
#include <QLabel>

namespace Ui {
class Asteroid;
}

/**
 * @brief The Asteroid class handles all of the necessary functionality of the asteroid objects.
 */
class Asteroid {
public:
    Asteroid();

    int getX() const;
    int getY() const;
    void setX(int x);
    void setY(int y);
    int getXdir() const;
    int getYdir() const;
    QLabel* getLabel() const;
    void setState();
    bool validPosition() const;
    bool collided_with_ship() const;

private:
    int x;
    int y;
    QLabel* astLabel;
    int xDir;
    int yDir;
};

#endif // ASTEROID_H

