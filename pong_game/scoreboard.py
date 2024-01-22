from turtle import Turtle
ALIGNMENT = "center"
FONT = ("Ariel", 40, "normal")
START_POS = (0, 230)


class Scoreboard(Turtle):

    def __init__(self):
        super().__init__()
        self.penup()
        self.hideturtle()
        self.color("white")
        self.leftp_score = 0
        self.rightp_score = 0
        self.goto(START_POS)
        self.write(arg=f"{self.leftp_score} {self.rightp_score}", align=ALIGNMENT, font=FONT)

    def update_score(self, scorer):
        self.clear()
        self.goto(START_POS)
        if scorer == "l":
            self.leftp_score += 1
        elif scorer == "r":
            self.rightp_score += 1
        self.write(arg=f"{self.leftp_score} {self.rightp_score}", align=ALIGNMENT, font=FONT)

    def game_over(self):
        self.goto(0, 0)
        if self.leftp_score == self.rightp_score:
            winner = "DRAW"
        elif self.leftp_score > self.rightp_score:
            winner = "Winner is Left Player"
        elif self.rightp_score > self.leftp_score:
            winner = "Winner is Right Player"
        self.write(arg=f"Game Over!\n{winner}", align=ALIGNMENT, font=FONT)

    def reset(self):
        self.goto(0, 0)
        self.write(arg="Next round will start in 5 seconds.", align=ALIGNMENT, font=FONT)
        # self.goto(START_POS)

