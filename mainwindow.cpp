/** @file mainwindow.cpp
 *  @brief Creates a mainwindow which is the users access point to the game.
 *         Contains a welcome banner, start button, how to play button,
 *         and a list of high scores.
 */

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "asteroid.h"

#include <iostream>
#include <vector>
#include <sstream>

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QGridLayout>
#include <QPixmap>
#include <QSize>
#include <QImage>
#include <QTransform>
#include <QKeyEvent>
#include <QTimer>
#include <QString>

const int NORTH = 0;
const int NORTHEAST = 45;
const int EAST = 90;
const int SOUTHEAST = 135;
const int SOUTH = 180;
const int SOUTHWEST = 225;
const int WEST = 270;
const int NORTHWEST = 315;

/**
 * @brief MainWindow::MainWindow is the constructor for the entire application. MainWindow contains a stacked widget
 * which manages the welcome screen and the gameboard.
 * @param parent is a default parameter
 */
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);

    GRIDWIDTH = 23;
    GRIDLENGTH = 23;
    BOARDWIDTH = 575;
    BOARDHEIGHT = 575;

    NUMASTEROIDS = 3;
    SIZE_OF_ASTEROID = 1;
    ASTEROID_SPEED = 600;
    RATE_OF_NUM_ASTR_INCREASE = 30*1000;

    TOP_LEFT_XCORD_SHIP = 11;
    TOP_LEFT_YCORD_SHIP = 11;
    SIZE_OF_SHIP = 1;
    ATTACK_SPEED = 50;

    // for stacked_widget
    stacked_widget = new QStackedWidget;
    QVBoxLayout* stacked_layout = new QVBoxLayout;
    central = new QWidget;

    shipDiagonal = false;
    shipCurrentRotation = 0;
    shipLabel = new QLabel;
    shipImage = QImage(":/images/spaceship.png");
    shipImageRotated = QImage(":/images/spaceshipRotated.png");
    shipPixmap.convertFromImage(shipImage);

    asteroidTimer = new QTimer;
    connect(asteroidTimer, SIGNAL(timeout()), this, SLOT(moveAsteroids()));

    attackTimer = new QTimer;
    connect(attackTimer, SIGNAL(timeout()), this, SLOT(moveAttack()));

    increase_number_of_asteroids_timer = new QTimer;
    connect(increase_number_of_asteroids_timer, SIGNAL(timeout()), this, SLOT(increase_num_asteroids()));

    QWidget* base = new QWidget;
    QVBoxLayout* bottomLayout = new QVBoxLayout(base);

    QWidget* welcomeWid = new QWidget;
    QWidget* bottomWid = new QWidget;

    QHBoxLayout* topHbox = new QHBoxLayout(welcomeWid);
    QLabel* welcome = new QLabel("Asteroids!");
    welcome->setStyleSheet("QLabel { font-size: 30px }");
    welcome->setAlignment(Qt::AlignCenter);
    topHbox->addWidget(welcome);

    QHBoxLayout* botHBox = new QHBoxLayout(bottomWid);
    QWidget* leftbottom = new QWidget;
    QWidget* rightbottom = new QWidget;
    QVBoxLayout* startAndHow = new QVBoxLayout(leftbottom);
    QVBoxLayout* highscore = new QVBoxLayout(rightbottom);

    QPushButton* startGame = new QPushButton;

    QObject::connect(startGame, SIGNAL(clicked(bool)), this, SLOT(startGame()) );

    startGame->setText("Start Game");
    startAndHow->addWidget(startGame);
    leftbottom->setLayout(startAndHow);

    QTextEdit* highScores = new QTextEdit;
    highScores->setText("<h2><center>How To Play: </center></h2> <p>Use the spacebar to attack.</p> <p>Use the left arrow key to rotate the ship to its left.</p> <p>Use the right arrow key to rotate the ship to its right.</p> <p>The number of asteroids increases the longer you survive.</p>");
    highScores->setReadOnly(true);
    //highScores->setMinimumHeight(400);
    highscore->addWidget(highScores);
    rightbottom->setLayout(highscore);

    botHBox->addWidget(leftbottom);
    botHBox->addWidget(rightbottom);
    bottomWid->setLayout(botHBox);
    welcomeWid->setLayout(topHbox);
    bottomLayout->addWidget(welcomeWid);
    bottomLayout->addWidget(bottomWid);
    base->setLayout(bottomLayout);

    stacked_widget->insertWidget(0, base);

    stacked_layout->addWidget(stacked_widget);
    central->setLayout(stacked_layout);

    setFixedSize(475,300);
    setCentralWidget(central);
}

/**
 * @brief MainWindow::createGameBoard creates our gameboard. Instantiates the grid tiles, adds the ship images,
 * and sets up the asteroid and attack images.
 * @return a pointer to our gameboard
 */
QWidget* MainWindow::createGameBoard() {

    QSize tileSize(BOARDWIDTH/GRIDWIDTH,BOARDHEIGHT/GRIDLENGTH);

    gameBoard = new QWidget;
    gameBoard->setFixedSize(BOARDWIDTH, BOARDHEIGHT);
    grid = new QGridLayout(gameBoard);

    gridLabels = new QLabel*[GRIDWIDTH * GRIDLENGTH];

    for (size_t i = 0, size = GRIDLENGTH*GRIDWIDTH; i < size; ++i ) {
        gridLabels[i] = new QLabel;
        gridLabels[i]->setStyleSheet("QLabel { background-color: black }; ");
        gridLabels[i]->setFixedSize(tileSize);
    }

    for (size_t i = 0; i < GRIDLENGTH; ++i) {
        for (size_t j = 0; j < GRIDWIDTH; ++j) {
            grid->addWidget(gridLabels[i * GRIDLENGTH + j], i, j);
        }
    }

    grid->addWidget(shipLabel, TOP_LEFT_XCORD_SHIP, TOP_LEFT_YCORD_SHIP, SIZE_OF_SHIP, SIZE_OF_SHIP, Qt::AlignCenter);

    for(size_t i = 0; i < NUMASTEROIDS; ++i) {
        Asteroid a;
        astVec.push_back(a);
    }

    set_up_attack();
    isAttacking = false;

    set_up_asteroids();

    num_shots_fired = 0;
    num_asteroids_hit = 0;

    asteroidTimer->start(ASTEROID_SPEED);
    attackTimer->start(ATTACK_SPEED);
    increase_number_of_asteroids_timer->start(RATE_OF_NUM_ASTR_INCREASE);

    grid->setSpacing(0);
    gameBoard->setLayout(grid);

    return gameBoard;
}

/**
 * @brief MainWindow::keyPressEvent handles the user's input from the keyboard.
 * If the user presses the right key, the ship turns to its right.
 * If the user presses the left key, the ship turns to its left.
 * If the user presses the spacebar, the ship fires its attack.
 * If any other key is pressed, nothing happens.
 * @param e is the default parameter for the function
 */
void MainWindow::keyPressEvent(QKeyEvent *e) {
    switch(e->key()) {
    case (Qt::Key_Left): {
        shipCurrentRotation += 315;
        rotateShip();
        break;
    }
    case (Qt::Key_Right): {
        shipCurrentRotation += 45;
        rotateShip();
        break;
    }
    case (Qt::Key_Space): {

        if (!isAttacking) {
            isAttacking = true;
            num_shots_fired++;

            attackX = TOP_LEFT_XCORD_SHIP;
            attackY = TOP_LEFT_YCORD_SHIP;

            switch(shipCurrentRotation % 360) {
            case(NORTH):
                attackXDirection = -1;
                attackYDirection = 0;
                break;
            case(NORTHEAST):
                attackXDirection = -1;
                attackYDirection = 1;
                break;
            case(EAST):
                attackXDirection = 0;
                attackYDirection = 1;
                break;
            case(SOUTHEAST):
                attackXDirection = 1;
                attackYDirection = 1;
                break;
            case(SOUTH):
                attackXDirection = 1;
                attackYDirection = 0;
                break;
            case(SOUTHWEST):
                attackXDirection = 1;
                attackYDirection = -1;
                break;
            case(WEST):
                attackXDirection = 0;
                attackYDirection = -1;
                break;
            case(NORTHWEST):
                attackXDirection = -1;
                attackYDirection = -1;
                break;
            default:
                break;
            }
        }
        break;
    }
    default:
        QWidget::keyPressEvent(e);
    }
}

/**
 * @brief MainWindow::rotateShip
 * This function rotates the ship image. The function takes advantage of the fact that only eight orientations
 * of the ship are possible. As a result, we can only have the ship facing a cardinal direction (N, S, E, W) or not
 * (NW, SW, SE, NE).
 */
void MainWindow::rotateShip() {

    QTransform trans;
    QImage tempShip;

    if(shipDiagonal)
        shipDiagonal = false;
    else
        shipDiagonal = true;

    if(shipDiagonal) {
        trans.rotate(shipCurrentRotation - 45);
        tempShip = shipImageRotated.transformed(trans);
        shipPixmap.convertFromImage(tempShip);
    }
    else {
        trans.rotate(shipCurrentRotation);
        tempShip = shipImage.transformed(trans);
        shipPixmap.convertFromImage(tempShip);
    }

    repaint();
}

/**
 * @brief MainWindow::set_up_attack
 * This function contructs the attack image.
 */
void MainWindow::set_up_attack() {
    attackLabel = new QLabel;
    attackLabel->setPixmap(QPixmap(":/images/attack.png"));
    attackLabel->setScaledContents(true);

}

/**
 * @brief MainWindow::set_up_asteroids
 * This function iterates through the vector of Asteroid objects and adds the Asteroids
 * to the gridlayout.
 */
void MainWindow::set_up_asteroids() {

    for(auto& astr : astVec) {
        grid->addWidget(astr.getLabel(), astr.getX(), astr.getY(), SIZE_OF_ASTEROID, SIZE_OF_ASTEROID, Qt::AlignCenter);
    }
}

/**
 * @brief MainWindow::is_attack_in_valid_range
 * This function checks that there is at least one grid between the attack image
 * and the border of the grid.
 * @return true is there is at least one grid, false otherwise.
 */
bool MainWindow::is_attack_in_valid_range() {
    return attackX > 0 && attackX < GRIDLENGTH - 1 && attackY > 0 && attackY < GRIDWIDTH - 1;
}

/**
 * @brief MainWindow::paintEvent
 * @param e is the default paramater for the paintEvent function
 * This function redraws the ship image (which may have rotated) and
 * draws the asteroids on the grid.
 */
void MainWindow::paintEvent(QPaintEvent* e) {
    shipLabel->setPixmap(shipPixmap);
    shipLabel->setScaledContents(true);

    set_up_asteroids();
}

/**
 * @brief MainWindow::reset_gameboard is used to return the gameboard widget to its default statement.
 * This is the state in which it existed before creation. The function is called when the game ends.
 */
void MainWindow::reset_gameboard() {

    attackTimer->stop();
    asteroidTimer->stop();
    increase_number_of_asteroids_timer->stop();
    NUMASTEROIDS = 3;
    astVec.clear();
}

/**
 * @brief MainWindow::collided_with_attack checks if the ship's attack has collided with an asteroid.
 * @param a is the asteroid that we are checking against for a collision.
 * @return true if a collision occured, false otherwise
 */
bool MainWindow::collided_with_attack(const Asteroid &a) const {
    return a.getX() == attackX && a.getY() == attackY;
}

/**
 * @brief MainWindow::reset_attack removes the attack image from the gameboard.
 * This function is called if the attack image leaves the screen or if it collides
 * with an asteroid.
 */
void MainWindow::reset_attack() {
    grid->removeWidget(attackLabel);
    delete attackLabel;
    attackLabel = nullptr;
    isAttacking = false;
    attackX = TOP_LEFT_XCORD_SHIP;
    attackY = TOP_LEFT_YCORD_SHIP;
}

/**
 * @brief MainWindow::create_gameover_screen handles the creation of the gameover window that appears once an asteroid collides with a ship.
 * The window displays the number of asteroids hit by the user and the accuracy with which the user shot during his play.
 * @return a pointer to the gameover window widget.
 */
QWidget* MainWindow::create_gameover_screen() {

    std::stringstream asteroids_hit;
    asteroids_hit << num_asteroids_hit;

    std::stringstream accuracy;
    std::string accuracy_string;

    if(num_shots_fired != 0) {
        accuracy << (num_asteroids_hit * 1.0/num_shots_fired*100);
        accuracy_string = "     Accuracy: " + (accuracy.str()).substr(0, 5) + "%";
    } else {
        accuracy_string = "     Accuracy: No shots were fired!";
    }

    QString q = QString::fromStdString("Number of Asteroids Destroyed: " + asteroids_hit.str() + accuracy_string);

    gameover_screen = new QWidget;
    gameover_layout = new QVBoxLayout(gameover_screen);
    gameover_banner = new QLabel;

    gameover_banner->setText("Game Over");
    gameover_banner->setStyleSheet("QLabel { font-size: 30px; } ");
    gameover_banner->setAlignment(Qt::AlignHCenter);
    gameover_layout->addWidget(gameover_banner);

    score = new QLabel;
    score->setText(q);
    score->setAlignment(Qt::AlignHCenter);
    gameover_layout->addWidget(score);

    QPushButton* return_home = new QPushButton;
    return_home->setText("Return to Main Menu");
    QObject::connect(return_home, SIGNAL(clicked(bool)), this, SLOT(return_to_main_menu()));
    gameover_layout->addWidget(return_home);
    gameover_screen->setLayout(gameover_layout);

    return gameover_screen;
}

/**
 * @brief MainWindow::~MainWindow
 * Destructor for MainWindow class.
 */
MainWindow::~MainWindow()
{
    delete ui;
}
