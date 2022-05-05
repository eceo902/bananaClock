import sqlite3
import datetime

alarm_db = '/var/jail/home/team41/alarms/alarmtimes.db' 

def alarm_request_handler(request):
    if request["method"]=="GET":
        try:
            username = request['values']['user']
        except:
            return "This request is missing a username"   
        
        with sqlite3.connect(alarm_db) as c:
            
            #check if exists
            users = [i for i in c.execute('''SELECT * FROM alarm_data WHERE user == ?;''',(username,))]
            
            if not users:
                return {"doc": {"alarm_time": {"a0": "", "a1": "", "a2": "", "a3": "", "a4": ""}, 
                                "music":{"m0": -1, "m1": -1, "m2": -1, "m3": -1, "m4": -1}}} # "User does not exist or has no alarms set"

            times = [i[1] for i in users]
            music = [j[1] for j in users]
            
            timedict = dict()
            musicdict = dict()
            for i in range(5):
                if i < len(users):
                    timedict["a"+ str(i)] = times[i]
                    musicdict["m"+ str(i)] = music[i]
                else:
                    timedict["a"+ str(i)] = ""
                    musicdict["m"+ str(i)] = -1
            
        return {"doc": {"alarm_time": timedict, "music": musicdict}} #{'alarm_time': times, 'music': music}
    else:
      return "invalid HTTP method for this url."