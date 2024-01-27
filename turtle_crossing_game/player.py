from turtle import Turtle
STARTING_POS = (0, -280)
HEADING = 90


class Player(Turtle):

    def __init__(self):
        super().__init__()
        self.shape("turtle")
        self.penup()
        self.goto(STARTING_POS)
        self.color("black")
        self.setheading(HEADING)

    def move(self):
        self.forward(10)

    def reset(self):
        self.goto(STARTING_POS)
