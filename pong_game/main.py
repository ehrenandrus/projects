from turtle import Screen
from paddle import Paddle
from ball import Ball
from scoreboard import Scoreboard
import time

#set up screen
screen = Screen()
sc_width = 800
sc_height = 600
screen.setup(width=sc_width, height=sc_height)
screen.bgcolor("black")
screen.title("Pong")
screen.tracer(0)

#define boundaries of the board
upper_bound = int(sc_height / 2) - 20
lower_bound = -upper_bound
right_bound = int(sc_width / 2) - 20
left_bound = -right_bound

#Start positions at the edge of the screen
l_paddle_start_pos = (-350, 0)
r_paddle_start_pos = (350, 0)

#create paddle, ball, and scoreboard objects
l_paddle = Paddle(l_paddle_start_pos)
r_paddle = Paddle(r_paddle_start_pos)
ball = Ball()
scoreboard = Scoreboard()

#bind keys to movement functions for paddles.
screen.onkey(lambda: r_paddle.move_up(), key="Up")
screen.onkey(lambda: r_paddle.move_down(), key="Down")
screen.onkey(lambda: l_paddle.move_up(), key="w")
screen.onkey(lambda: l_paddle.move_down(), key="s")

# #loop control variable
# game_is_on = True

#winning score
winning_score = 10

screen.update()


def reset_screen():
    scoreboard.reset()
    ball.goto(0, 0)
    l_paddle.goto(l_paddle_start_pos)
    r_paddle.goto(r_paddle_start_pos)
    ball.reset()
    time.sleep(4)
    scoreboard.update_score("n")


def play_game():
    game_is_on = True
    while game_is_on:
        screen.update()

        screen.listen()
        ball.move()
        time.sleep(ball.sleep)

        #check if ball is hit by paddle
        if ball.distance(l_paddle) < 50 and ball.xcor() < l_paddle_start_pos[0] + 20:
            ball.hit()
        elif ball.distance(r_paddle) < 50 and ball.xcor() > r_paddle_start_pos[0] - 20:
            ball.hit()

        # check if ball hits the top boundary and bounce
        if ball.ycor() > upper_bound:
            ball.bounce()
        elif ball.ycor() < lower_bound:
            ball.bounce()

        #update score if ball goes beyond the x boundaries
        if ball.xcor() < left_bound:
            scoreboard.update_score("r")
            reset_screen()
        elif ball.xcor() > right_bound:
            scoreboard.update_score("l")
            reset_screen()

        #end game when someone reaches 10
        if scoreboard.leftp_score == winning_score or scoreboard.rightp_score == winning_score:
            scoreboard.game_over()
            game_is_on = False


play_game()
screen.exitonclick()
