from turtle import Turtle
ALIGNMENT = "center"
FONT = ("Ariel", 18, "normal")
class Scoreboard(Turtle):

    def __init__(self):
        super().__init__()
        self.color("white")
        self.score = 0
        self.penup()
        self.hideturtle()
        self.goto(x=0, y=270)

    def print_score(self):
        self.write(arg=f"Score: {self.score}", align=ALIGNMENT, font=FONT)

    def update_score(self):
        self.clear()
        self.score += 1

    def game_over(self):
        self.goto(0, 0)
        self.write(arg=f"GAME OVER", align=ALIGNMENT, font=FONT)