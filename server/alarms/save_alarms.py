import sqlite3
import datetime

alarm_db = "/var/jail/home/team41/alarms/alarmtimes.db" 

def request_handler(request):
    if request["method"]=="POST":
        #parse the body of the incoming POST
        try:
            username = request['form']['user']
            time = request['form']['alarm_time']
            set = request['form']['set_alarm']
            alarm_music = request['form']['music']
        except:
            return "Error with username, alarm time, set state, or music"   
        
        with sqlite3.connect(alarm_db) as c:
            c.cursor()  # move cursor into database (allows us to execute commands)
            c.execute("""CREATE TABLE IF NOT EXISTS alarm_data (user text, alarm_time float, set_alarm text, music text);""")
            # some other query(ies) about inserting data
            c.execute('''INSERT into alarm_data VALUES (?,?,?,?);''',(username, time, set, alarm_music))

        return "Data POSTED successfully"
    else:
        return "invalid HTTP method for this url."