import sqlite3
import datetime

ht_db = '/var/jail/home/team41/login/userpassword.db' 

def request_handler(request):
    if request["method"]=="POST":
        #parse the body of the incoming POST
        try:
            username = request['form']['user']
            pw = request['form']['password']
        except:
            return "Error with username or password"   
        
        with sqlite3.connect(ht_db) as c:
            c.cursor()  # move cursor into database (allows us to execute commands)
            c.execute("""CREATE TABLE IF NOT EXISTS user_pw_data (user text, password text);""")
            # some other query(ies) about inserting data
            
            #if already in database
            users = [i for i in c.execute('''SELECT * FROM user_pw_data WHERE user == ?;''',(username,))]
            
            if users:
                return "Username is taken"
            else:
                c.execute('''INSERT into user_pw_data VALUES (?,?);''',(username, pw))
        
        return "Data POSTED successfully"
    else:
        return "invalid HTTP method for this url."
