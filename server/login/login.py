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
  background-color: yellow;
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
        background-color: lightblue;
    }
</style>
</head>
<body>
    <center> <h1> BananaClock Login</h1> </center>
    <form action="" method="POST">
        <div class="container">
            <label>Username : </label>
            <input type="text" placeholder="Enter Username" name="username" required>
            <label>Password : </label>
            <input type="password" placeholder="Enter Password" name="password" required>
            <button type="submit">Login</button>
        </div>
    </form>
    <a href="http://608dev-2.net/sandbox/sc/team41/login/create_login.py">Create an account</a>
</body>
</html>"""

error_html = """<!DOCTYPE html>
<html>
<head>
<meta name="viewport" content="width=device-width, initial-scale=1">
<title> Login Page </title>
<style>
Body {
  font-family: Calibri, Helvetica, sans-serif;
  background-color: yellow;
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
        background-color: lightblue;
    }
</style>
</head>
<body>
    <center> <h1> BananaClock Login</h1> </center>
    <div class="container">
        <p>Username or password incorrect</p>
    </div>
    <form action="" method="POST">
        <div class="container">
            <label>Username : </label>
            <input type="text" placeholder="Enter Username" name="username" required>
            <label>Password : </label>
            <input type="password" placeholder="Enter Password" name="password" required>
            <button type="submit">Login</button>
        </div>
    </form>
    <a href="http://608dev-2.net/sandbox/sc/team41/login/create_login.py">Create an account</a>
</body>
</html>"""


def request_handler(request):
    if request["method"]=="GET":
        return html
    elif request["method"]=="POST":
        #parse the body of the incoming POST
        try:
            username = request['form']['username'].lower()
            password = request['form']['password'].lower()
        except:
            return "This request is missing a username or password"

        with sqlite3.connect(ht_db) as c:
            c.execute("""CREATE TABLE IF NOT EXISTS user_pw_data (user text, password text);""")
            users = [i for i in c.execute('''SELECT * FROM user_pw_data WHERE user == ?;''',(username,))]

            #print(users, users[0][1])
            if users:
                if users[0][1] != password:
                    return "Incorrect password"

                return f"""
                        <!DOCTYPE html>
                        <html>
                        <head>
                        <title>HTML Meta Tag</title>
                       <meta http-equiv = "refresh" content = "0; url = https://608dev-2.net/sandbox/sc/team41/two_tables.py?user={username}"/>
                       </head>
                        </html>
                        """

 # "Login complete"
            else:
                return error_html
    else:
      return "invalid HTTP method for this url."