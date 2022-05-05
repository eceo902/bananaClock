import sqlite3
import datetime

alarm_db = "/var/jail/home/team41/alarms/alarmtimes.db" 

def request_handler(request):
    if request["method"]=="POST":
        #parse the body of the incoming POST
        try:
            username = request['form']['user']
            times = request['form']['alarm_time'][1:-1].replace(' ','').split(',')
            alarm_music = request['form']['music'][1:-1].split(',')
        except:
            return "Error with username, alarm time, set state, or music"   
        
        try:
            alarms = [t[1:-1] for t in times]
            musics = [int(m) for m in alarm_music]
        except:
            alarms = []
            musics = []

        if len(alarms) != len(musics):
            return "Error with alarm and music matching"

        with sqlite3.connect(alarm_db) as c:
            c.cursor()  # move cursor into database (allows us to execute commands)
            c.execute("""CREATE TABLE IF NOT EXISTS alarm_data (user text, alarm_time text, music int);""")
            # some other query(ies) about inserting data

            try:
                c.execute('''DELETE FROM alarm_data WHERE user == ?''', (username,))
            except:
                print('user not in database yet')

            if alarms == []:
                return "Data POSTED successfully"
            
            for i in range(len(alarms)):
                print( alarms[i], musics[i])
                c.execute('''INSERT into alarm_data VALUES (?,?,?);''',(username, alarms[i], musics[i]))

        return "Data POSTED successfully"
    else:
        return "invalid HTTP method for this url."