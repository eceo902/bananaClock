import sqlite3
import datetime

game_db = '/var/jail/home/team41/game_data/leaderboard.db' 

def request_handler(request):
    if request["method"]=="GET":
        #parse the body of the incoming POST
        try:
            username = request['values']['user']
            gamename = request['values']['game_name']
        except:
            return "Error with username or user has not played this game"   
        
        
        with sqlite3.connect(game_db) as c:
            c.cursor()  # move cursor into database (allows us to execute commands)
            users_games = [i for i in c.execute('''SELECT * FROM all_game_data WHERE user == ?;''',(username,))]
            print(users_games)

        return users_games
    else:
        return "invalid HTTP method for this url."
