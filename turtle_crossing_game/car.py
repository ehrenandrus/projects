from turtle import Turtle
import random
COLORS = ['red', 'blue', 'yellow', 'purple', 'orange', 'green']
HEADING = 180

class Car(Turtle):

    def __init__(self):
        super().__init__()
        self.penup()
        self.shape("square")
        self.color(random.choice(COLORS))
        self.goto(random.randint(-300, 300), random.randint(-240, 280))
        self.setheading(HEADING)
        self.shapesize(stretch_len=2)

    def drive(self):
        self.forward(10)
