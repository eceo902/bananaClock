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
    
                p = figure(title="Score over time", width=300, height=300, x_axis_type="datetime",x_range=[ datetime.datetime(2022, 5, 7, 16, 42, 55, 629441),  datetime.datetime(2022, 5, 20, 16, 42, 55, 629441)], y_range=[0, 60])
                p.xaxis.axis_label = "Datetime"
                p.yaxis.axis_label = "Time"
    
    
                time = [datetime.datetime.strptime(u[0],'%Y-%m-%d %H:%M:%S.%f') for u in user_games]
                score = [u[1] for u in user_games]
                print(time)
                print(score)
                p.xaxis.formatter=DatetimeTickFormatter(
    
                                days=["%m/%d %H:%M"],
                                        months=["%m/%d %H:%M"],
                                        hours=["%m/%d %H:%M"],
                                        minutes=["%m/%d %H:%M"]
                            )   
                p.xaxis.major_label_orientation = 3.14/2

                p.line(time, score)

                combination = row(Table, p)
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
                        {div1}
                    </body>
                    {script1}
                    </html>
                    '''
    else:
        return "invalid HTTP method for this url."