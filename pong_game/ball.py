from turtle import Turtle
from random import randint, choice

start_headings = [

    {
        "low": 150,
        "high": 240
    }
]

quadrants = [[0, 90], [91, 180], [181, 270], [271, 359]]

class Ball(Turtle):

    def __init__(self):
        super().__init__()
        self.shape("circle")
        self.penup()
        self.color("white")
        self.reset()
        self.x_move = 10
        self.y_move = 10
        self.sleep = .1

    def move(self):
        new_x = self.xcor() + self.x_move
        new_y = self.ycor() + self.y_move
        self.goto(new_x, new_y)

    def bounce(self):
        # current_heading = self.heading()
        # if quadrants[0][1] > current_heading > quadrants[0][0]:
        #     new_heading = randint(quadrants[1][0], quadrants[1][1])
        # elif quadrants[1][1] > current_heading > quadrants[1][0]:
        #     new_heading = randint(quadrants[0][0], quadrants[0][1])
        # elif quadrants[2][1] > current_heading > quadrants[2][0]:
        #     new_heading = randint(quadrants[3][0], quadrants[3][1])
        # elif quadrants[3][1] > current_heading > quadrants[3][0]:
        #     new_heading = randint(quadrants[2][0], quadrants[2][1])
        self.y_move *= -1

        # new_heading = (current_heading + choice([90, -90]))
        # print(f"current heading {current_heading}")
        # print(f"new heading {new_heading}")
        # self.setheading(new_heading)

    def hit(self):
        self.x_move *= -1
        self.sleep *= .9

    def reset(self):
        index = randint(0, 1)
        if index == 1:
            new_heading = choice([randint(0, 30), randint(330, 359)])
            self.setheading(new_heading)
        else:
            self.setheading(randint(start_headings[index]["low"], start_headings[index]["high"]))
        self.sleep = .1