/** @file mainwindow.h
 *  @brief Class declaration for the mainwindow class. Declares a public slot which connects the start button
 *         to the createGameBoard() function.
 */


#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "asteroid.h"

#include <vector>

#include <QMainWindow>
#include <QPixmap>
#include <QLabel>
#include <QGridLayout>
#include <iostream>
#include <QWidget>
#include <QStackedWidget>
#include <QTimer>

namespace Ui {
class MainWindow;
}

/**
 * @brief The MainWindow class handles the creating of the welcome window, the gameboard window, and the gameover window.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT

public slots:
    /**
     * @brief startGame
     * This function starts the game. We put the gameboard on the top of the stacked widget
     * and make the game visible. We also resize the MainWindow so that the board is unobstructed.
     */
    void startGame() {
        stacked_widget->insertWidget(1, createGameBoard());
        stacked_widget->setCurrentIndex(1);
        setFixedSize(595,595);
    }

    /**
     * @brief moveAsteroids
     * This function handles the animation of the Asteroid images.
     */
    void moveAsteroids() {

        for(auto& astr : astVec) {
            astr.setX(astr.getX() + astr.getXdir());
            astr.setY(astr.getY() + astr.getYdir());

            if(astr.collided_with_ship()) {
                reset_gameboard();
                stacked_widget->insertWidget(2, create_gameover_screen());
                stacked_widget->setCurrentIndex(2);
                setFixedSize(475,300);
                return;
            }

            if (!astr.validPosition()) {
                astr.setState();
            }
        }
        repaint();
    }

    void return_to_main_menu() {
        stacked_widget->setCurrentIndex(0);
    }

    /**
     * @brief moveAttack
     * This function handles the animation of the attack image.
     */
    void moveAttack() {

        if(attackLabel == nullptr) {
            set_up_attack();
        }

        if(isAttacking && is_attack_in_valid_range()) {
            attackX = attackX + attackXDirection;
            attackY = attackY + attackYDirection;
            grid->addWidget(attackLabel, attackX, attackY, 1, 1, Qt::AlignCenter);
            for(auto& x : astVec) {

                /** If the coordinates of the attack image equal the coordinates of an asteroid,
                 *  we want to remove the attack image, restart the asteroid, and exit the function
                  */
                if(attackX == x.getX() && attackY == x.getY()) {
                    reset_attack();
                    x.setState();
                    num_asteroids_hit++;
                    return;
                }
            }
        } else if (isAttacking && !is_attack_in_valid_range()) {
            reset_attack();
        }
    }

    void increase_num_asteroids() {
        astVec.push_back(Asteroid());
        NUMASTEROIDS++;
    }

public:
    explicit MainWindow(QWidget *parent = 0);
    QWidget* createGameBoard();
    QWidget* create_gameover_screen();
    void paintEvent(QPaintEvent* e);
    void keyPressEvent(QKeyEvent* e);
    void rotateShip();
    bool collided_with_attack(const Asteroid& a) const;
    ~MainWindow();

    int GRIDWIDTH;
    int GRIDLENGTH;
    int BOARDWIDTH;
    int BOARDHEIGHT;

    int NUMASTEROIDS;
    int SIZE_OF_ASTEROID;
    int ASTEROID_SPEED;
    int RATE_OF_NUM_ASTR_INCREASE;

    int TOP_LEFT_XCORD_SHIP;
    int TOP_LEFT_YCORD_SHIP;
    int SIZE_OF_SHIP;
    int ATTACK_SPEED;

private:
    Ui::MainWindow *ui;

    QStackedWidget* stacked_widget;
    QWidget* central;

    bool shipDiagonal;
    size_t shipCurrentRotation;
    QImage shipImage;
    QImage shipImageRotated;
    QPixmap shipPixmap;
    QLabel* shipLabel;

    void set_up_attack();
    bool is_attack_in_valid_range();
    QLabel* attackLabel;
    bool isAttacking;
    int attackX;
    int attackY;
    int attackXDirection;
    int attackYDirection;

    QTimer* attackTimer;
    QTimer* asteroidTimer;
    QTimer* increase_number_of_asteroids_timer;

    QWidget* gameBoard;
    QLabel** gridLabels;
    QGridLayout* grid;

    std::vector<Asteroid> astVec;
    void set_up_asteroids();

    void reset_gameboard();
    void reset_attack();

    //gameover screen section
    QWidget* gameover_screen;
    QVBoxLayout* gameover_layout;
    QLabel* score;
    QLabel* gameover_banner;
    size_t num_asteroids_hit;
    size_t num_shots_fired;
};

#endif // MAINWINDOW_H
