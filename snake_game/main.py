from turtle import Screen
import time
from snake import Snake
from food import Food
from scoreboard import Scoreboard

screen = Screen()
screen.setup(width=600, height=600)
screen.bgcolor("black")
screen.title("Snake Game")
screen.tracer(0)

snake = Snake()
food = Food()
scoreboard = Scoreboard()

screen.onkey(lambda: snake.left(), key="Left")
screen.onkey(lambda: snake.right(), key="Right")
screen.onkey(lambda: snake.up(), key="Up")
screen.onkey(lambda: snake.down(), key="Down")


screen.update()
game_is_on = True
while game_is_on:
    scoreboard.print_score()
    screen.update()
    time.sleep(.1)
    snake.move_forward()
    screen.listen()

    # detect collision with food
    if snake.head.distance(food) < 15:
        food.refresh()
        snake.extend()
        scoreboard.update_score()

    # detect collision with wall
    if snake.head.xcor() > 280 or snake.head.xcor() < -280 or snake.head.ycor() > 280 or snake.head.ycor() < -280:
        game_is_on = False
        scoreboard.game_over()

    # detect collision with tail
    for segment in snake.segments[1:]:
        if snake.head.distance(segment) < 15:
            game_is_on = False
            scoreboard.game_over()

screen.exitonclick()
