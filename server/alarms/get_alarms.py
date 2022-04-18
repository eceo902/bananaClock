import sqlite3
import datetime

alarm_db = '/var/jail/home/team41/alarms/alarmtimes.db' 

def request_handler(request):
    if request["method"]=="GET":
        try:
            username = request['values']['user']
        except:
            return "This request is missing a username"   
        
        with sqlite3.connect(alarm_db) as c:
            
            #check if exists
            users = [i for i in c.execute('''SELECT * FROM alarm_data WHERE user == ?;''',(username,))]
            
            if not users:
                return "User does not exist or has no alarms set"
            
        return users
    else:
      return "invalid HTTP method for this url."