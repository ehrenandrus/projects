from turtle import Turtle


class Paddle(Turtle):

    def __init__(self, start_position):
        super().__init__()
        self.shape("square")
        self.penup()
        self.turtlesize(stretch_wid=1, stretch_len=5)
        self.goto(start_position)
        self.color("white")
        self.setheading(90)
        self.move_amount = 20

    def move_up(self):
        self.forward(self.move_amount)

    def move_down(self):
        self.backward(self.move_amount)

