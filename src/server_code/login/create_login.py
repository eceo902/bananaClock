import sqlite3
import datetime

ht_db = '/var/jail/home/team41/login/userpassword.db'

html = """<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title> Login Page </title>
<style>
Body {
  font-family: Calibri, Helvetica, sans-serif;
  background-color: white;
}
button {
       background-color: #4CAF50;
       width: 100%;
        color: orange;
        padding: 15px;
        margin: 10px 0px;
        border: none;
        cursor: pointer;
         }
 form {
        border: 3px solid #f1f1f1;
    }
 input[type=text], input[type=password] {
        width: 100%;
        margin: 8px 0;
        padding: 12px 20px;
        display: inline-block;
        border: 2px solid green;
        box-sizing: border-box;
    }
 button:hover {
        opacity: 0.7;
    }
  .cancelbtn {
        width: auto;
        padding: 10px 18px;  
        margin: 10px 5px;  
    }   
    
    
 .container {   
        padding: 25px;   
        background-color: pink;
    }   
</style>   
</head>    
<body>    
    <center> <h1> Create BananaClock Account</h1> </center>   
    <form action="" method="POST">  
        <div class="container">   
            <label>Username : </label>   
            <input type="text" placeholder="Enter Username" name="username" required>  
            <label>Password : </label>   
            <input type="password" placeholder="Enter Password" name="password" required>  
            <label>Confirm Password : </label>   
            <input type="password" placeholder="Confirm Password" name="confirmPassword" required>  
            <button type="submit">Create</button>   
        </div>   
    </form>    
</body>    
</html>"""


def request_handler(request):
    if request["method"] == "GET":
        return html
    if request["method"] == "POST":
        #parse the body of the incoming POST
        try:
            username = request['form']['username'].lower()
            password = request['form']['password'].lower()
            confirmPassword = request['form']['confirmPassword'].lower()
        except:
            return "Error with username, password, or confirmPassword"

        if password != confirmPassword:
            return "password and confirmed password are not equal"

        with sqlite3.connect(ht_db) as c:
            c.execute("""CREATE TABLE IF NOT EXISTS user_pw_data (user text, password text);""")
            # some other query(ies) about inserting data

            #if already in database
            users = [i for i in c.execute('''SELECT * FROM user_pw_data WHERE user == ?;''',(username,))]

            if users:
                return "Username is taken"
            else:
                c.execute('''INSERT into user_pw_data VALUES (?,?);''',(username, password))

        return f"""
                <!DOCTYPE html>
                <html>
                <head>
                 <title>HTML Meta Tag</title>
                 <meta http-equiv = "refresh" content = "0, url = https://608dev-2.net/sandbox/sc/team41/login/login.py"/>
                </head>
                </html>
                """

 #"Data POSTED successfully"
    else:
        return "invalid HTTP method for this url."