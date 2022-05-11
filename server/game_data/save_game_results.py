import sqlite3
import datetime

game_db = '/var/jail/home/team41/game_data/leaderboard.db' 

def request_handler(request):
    if request["method"]=="POST":
        #parse the body of the incoming POST
        try:
            username = request['form']['user']
        except:
            return "user error"
        try:
            gamename = request['form']['game_name']
        except:
            return "game error"
        try:
            gamelength = request['form']['length']
        except:
            return "Error with game length"  
        try:
            on_board = request['form']['on_leaderboard']
        except:
            return "Error with leaderboard privacy"
        
        with sqlite3.connect(game_db) as c:
            c.cursor()  # move cursor into database (allows us to execute commands)
            c.execute("""CREATE TABLE IF NOT EXISTS all_game_data (user text, date Datetime, game_name text, length float, on_leaderboard text);""")
            # some other query(ies) about inserting data
            c.execute('''INSERT into all_game_data VALUES (?,?,?,?,?);''',(username, datetime.datetime.now(), gamename, gamelength, on_board))

        return "Data POSTED successfully"
    else:
        return "invalid HTTP method for this url."
