from turtle import Turtle
STARTING_POS = (-280, 270)
FONT = ('Arial', 15, 'normal')
ALIGNMENT = 'left'

class Scoreboard(Turtle):

    def __init__(self):
        super().__init__()
        self.penup()
        self.color("black")
        self.hideturtle()
        self.goto(STARTING_POS)
        self.level = 1
        self.write_level()

    def next_level(self):
        self.level += 1
        self.clear()
        self.write_level()

    def write_level(self):
        self.write(arg=f"Level: {self.level}", align=ALIGNMENT, font=FONT)

    def game_over(self):
        self.goto(0, 0)
        self.write(arg=f"GAME OVER!", align="center", font=FONT)
