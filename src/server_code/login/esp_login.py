import sqlite3
import datetime

ht_db = '/var/jail/home/team41/login/userpassword.db' 

def request_handler(request):
    if request["method"]=="GET":
        #parse the body of the incoming POST
        try:
            username = request['values']['user']
            pw = request['values']['password']
        except:
            return "This request is missing a username or password"   
        
        with sqlite3.connect(ht_db) as c:
            
            #check if exists
            users = [i for i in c.execute('''SELECT * FROM user_pw_data WHERE user == ?;''',(username,))]
            
            #print(users, users[0][1])
            if users:
                if users[0][1] != pw:
                    return "Incorrect password"

                return "Login complete"
            else:
                return "Incorrect username or user does not exist"
            
            
    else:
      return "invalid HTTP method for this url."
