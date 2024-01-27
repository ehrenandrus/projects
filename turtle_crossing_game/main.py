from turtle import Screen
from player import Player
from car import Car
from scoreboard import Scoreboard
import time
import random

SC_WIDTH = 600
SC_HEIGHT = 600

screen = Screen()
screen.setup(width=SC_WIDTH, height=SC_HEIGHT)
screen.tracer(0)

player = Player()
cars = []
random_car_amount = 20
for _ in range(random_car_amount):
    cars.append(Car())
scoreboard = Scoreboard()


def move_player():
    player.move()


screen.onkey(lambda: move_player(), key="Up")
screen.listen()


def check_collision(current_car):
    y_max = player.ycor() + 20
    y_min = player.ycor() - 20
    print(f"current_car x,y ={current_car.xcor()}, {current_car.ycor()} | player x,y {player.pos()}")
    if current_car.distance(player) < 40 and y_min < current_car.ycor() < y_max:
        return True
    else:
        return False


def spawn_car():
    new_car = Car()
    end = (300, random.randint(-240, 280))
    new_car.goto(end)
    cars.append(new_car)


def check_finish_line():
    return player.ycor() > 280


def start_level():
    player.reset()
    for _ in range(scoreboard.level + 10):
        cars.append(Car())


def game_play():
    collision = False
    percent = 70
    while not collision:

        for car in cars:
            car.drive()
            collision = check_collision(car)
            if collision:
                scoreboard.game_over()
                break

        if check_finish_line():
            scoreboard.next_level()
            percent *= .9
            percent = int(percent)
            start_level()

        if random.randint(1, 100) > percent:
            spawn_car()
        time.sleep(.1)
        screen.update()


game_play()
screen.exitonclick()
