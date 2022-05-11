from bokeh.plotting import figure
from bokeh.io import show
from bokeh.embed import components
from bokeh.models import ColumnDataSource, TableColumn, DataTable, CustomJS, Range1d, Select, Panel, Tabs, DatetimeTickFormatter
from bokeh.plotting import figure, output_file, show
from bokeh.layouts import column, row, gridplot
import pandas as pd
import sqlite3
import datetime

game_db = '/var/jail/home/team41/game_data/leaderboard.db'

def request_handler(request):
    if request["method"]=="GET":
        #parse the body of the incoming POST
        try:
            username = request['values']['user']
        except:
            return "Error with username"

        with sqlite3.connect(game_db) as c:
            c.cursor()  # move cursor into database (allows us to execute commands)
    
            games = [j[0] for j in c.execute('''SELECT DISTINCT game_name FROM all_game_data WHERE user == ? AND on_leaderboard == "True";''',(username,))]
            print('g', games)
            tab_list = []
            for g in games:

                # table
                users = [i[0] for i in c.execute('''SELECT DISTINCT user FROM all_game_data WHERE game_name == ?;''',(g,))]
    
                scores = []
                for u in users:
                    top_score = [i for i in c.execute('''SELECT * FROM all_game_data WHERE user == ? AND game_name == ? ORDER BY length ASC LIMIT 1;''',(u,g))][0][3]
                    scores.append(top_score)
    
                df = pd.DataFrame({'User': users, 'Best Score': scores}).sort_values(by=['Best Score'])
                source = ColumnDataSource(df)
                columns = [TableColumn(field='User', title='User'),TableColumn(field='Best Score', title='Best Score')]
                Table = DataTable(source=source, columns=columns)

                #graph
                user_games = [(i[1], i[3]) for i in c.execute('''SELECT * FROM all_game_data WHERE user == ? AND game_name == ? ORDER BY date ASC;''',(u,g))]
    
                t = [datetime.datetime.strptime(u[0],'%Y-%m-%d %H:%M:%S.%f') for u in user_games]
                time = [m.strftime("%m/%d/%Y, %H:%M:%S") for m in t]
                score = [u[1] for u in user_games]
                print(time)
                print(score)
    
                df_1 = pd.DataFrame({'Time': time, 'Score': score}).sort_values(by=['Score'], ascending=True)
                source_1 = ColumnDataSource(df_1)
                columns_1 = [TableColumn(field='Time', title='Time'),TableColumn(field='Score', title='Score')]
                Table_1 = DataTable(source=source_1, columns=columns_1)
    
                combination = row(Table, Table_1)
                tab = Panel(child=combination, title=g)
                tab_list.append(tab)
    
            div1, script1 = components(Tabs(tabs=tab_list))

            # return tab_list
            return f'''<!DOCTYPE html>
                    <html> <script src="https://cdn.bokeh.org/bokeh/release/bokeh-2.4.2.min.js"></script>

<script src="https://cdn.bokeh.org/bokeh/release/bokeh-widgets-2.4.2.min.js"
        crossorigin="anonymous"></script>
<script src="https://cdn.bokeh.org/bokeh/release/bokeh-tables-2.4.2.min.js"
        crossorigin="anonymous"></script>
<script src="https://cdn.bokeh.org/bokeh/release/bokeh-gl-2.4.2.min.js"
        crossorigin="anonymous"></script>
<script src="https://cdn.bokeh.org/bokeh/release/bokeh-mathjax-2.4.2.min.js"
        crossorigin="anonymous"></script>
                    <body>
                    <b>Banana Clock Game Results for {username}</b>
                        {div1}
                    </body>
                    {script1}
                    </html>
                    '''
    else:
        return "invalid HTTP method for this url."